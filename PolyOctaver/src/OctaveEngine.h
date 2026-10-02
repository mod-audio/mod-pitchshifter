#ifndef MOD_POLYOCTAVER_OCTAVEENGINE_H
#define MOD_POLYOCTAVER_OCTAVEENGINE_H

#include <stdint.h>
#include <fftw3.h>

/* Four octave voices (-2, -1, +1, +2), and a unison for when the dry signal is to be detuned,
 * from one FFT and one inverse FFT per frame.
 *
 * The family's PSSinthesis stretches every bin on its own and resamples, one inverse FFT per
 * voice. That is fine going down, but going up an octave it loses 15 to 25 dB, unevenly across
 * partials, and leaves spurious content 10 to 25 dB under the wanted one (test/baseline_test.cpp).
 * For fixed octaves there is a better way, after Laroche & Dolson's peak-shifting vocoder:
 *
 *  - find the spectral peaks; every bin belongs to the region of its nearest peak;
 *  - move each region as a block to `ratio` times the peak's frequency. The lobe keeps its shape,
 *    so the partial keeps its level, and the voices are summed in the frequency domain;
 *  - a partial's leakage reaches into its neighbours' regions, and would move with them to the
 *    wrong pitch: 40 Hz away it is only 22 dB down with the low-latency window below. So each
 *    partial is first taken out of the spectrum as a whole, skirt included, using the window's own
 *    transform, and written back at its new frequency; only what is left over moves by region;
 *  - rotate the region so the new partial's phase is `ratio` times the old one. For +1/+2 that is
 *    a multiplication, no accumulated state. For -1/-2 the halved phase has two (four) candidates;
 *    the one closest to where the last frame predicts is taken, the same decision an analog
 *    octave divider's flip-flop makes. Every voice stays phase-locked to the input, so a mix of
 *    dry and voices holds its timbre instead of slowly phasing.
 *
 * Latency is set by the synthesis window, not by the frame: the analysis window is long and
 * asymmetric (frequency resolution of the whole frame), the synthesis window covers only the
 * newest `synth` samples, and their product is a Hann window so the overlap-add is exact.
 *
 * Nothing is allocated and no FFTW plan is made after construction.
 */
class OctaveEngine
{
public:
	enum Voice { SUB2 = 0, SUB1, UNISON, UP1, UP2, NVOICES };

	struct Config
	{
		int frame;  // analysis length, power of two
		int synth;  // synthesis window length, <= frame; == frame gives plain symmetric Hann windows
		int hop;    // samples between frames: even, and synth / hop an integer >= 2 (3 or more sounds better)
		int skirt;  // bins either side of a partial that are modelled and moved with it; 0 = move by region only
		int band;   // highest input bin that is analysed; 0 = all of them
	};

	static Config DefaultConfig(double sampleRate);

	OctaveEngine(const Config& config, bool haveWisdom);
	~OctaveEngine();

	int Hop() const { return T; }             // samples per Process() call: half the frame hop
	int Latency() const { return M - H + T; } // samples, input to voices

	void Reset();

	/* One tick: `in` and `wet` hold Hop() samples. `gains` are linear, one per voice; a voice at 0
	 * costs nothing. `detune`, if given, is a frequency ratio per voice on top of its octave (1 = in tune). */
	void Process(const float* in, float* wet, const float* gains, const float* detune = 0);

private:
	OctaveEngine(const OctaveEngine&);
	OctaveEngine& operator=(const OctaveEngine&);

	void BuildLobeTable();
	void FindPeaks();
	float PeakOffset(int bin) const;
	void ModelPartials();
	void Cleaned(int partial, int bin, float& re, float& im) const;
	float OffsetFromNeighbours(float left, float right) const;
	float LobeMagnitude(float offset) const // linear between table points
	{
		const float x = (offset < 0 ? -offset : offset) * kLobeSteps;
		const int idx = static_cast<int>(x);
		const float f = x - idx;
		return (1.0f - f) * lobeMag[idx] + f * lobeMag[idx + 1];
	}
	void Lobe(float offset, float& re, float& im) const
	{
		const float a = offset < 0 ? -offset : offset;
		const int idx = static_cast<int>(a * kLobeSteps + 0.5f);
		re = lobeRe[idx];
		im = offset < 0 ? -lobeIm[idx] : lobeIm[idx];
	}
	void AddVoice(int voice, float gain, float detune);
	void AddLobe(float position, float re, float im);

	const int N, M, H, T, NB; // frame, synthesis length, frame hop, tick (H/2), bins (N/2 + 1)
	const int K;              // modelled skirt, bins
	const int LB;             // last input bin analysed

	float* ring;       // newest N input samples
	float* winA;       // analysis window, N
	float* winS;       // synthesis window over the last M samples of the frame, overlap-add gain included
	float* timeIn;     // windowed frame
	float* timeOut;    // inverse FFT
	float* ola;        // M
	float* pending;    // H: a frame's worth of output, sent out over two ticks
	int ringAt;        // ring is circular; this is its oldest sample
	fftwf_complex* X;  // spectrum, phases referred to the centre of the synthesis window
	fftwf_complex* Y;  // sum of the voices, same reference
	float* refRe;      // e^{j 2 pi k centre / N}
	float* refIm;
	float* mag2;
	fftwf_plan fwd, inv;

	// the analysis window's transform, same phase reference, kLobeSteps points per bin, offsets 0..K+1
	static const int kLobeSteps = 64;
	float* lobeRe;
	float* lobeIm;
	float* lobeMag;
	static const int kOffsetSteps = 256;
	float offsetRatio[kOffsetSteps + 1];

	// A six-string chord has about sixty partials worth the name; anything past the strongest
	// kMaxPartials local maxima moves with the region it is in.
	static const int kMaxPartials = 64;

	int nCandidates;
	int* candidate;    // local maxima, strongest first
	float* explained;  // magnitude the accepted partials account for, per bin
	char* accepted;

	int nPeaks;
	float* ampRe;      // complex amplitude of each partial
	float* ampIm;
	int* peakBin;
	int* regionLo;
	int* regionHi;
	float* peakPos;    // fractional bin
	float* peakPhase;

	float* lastPhase[2];   // SUB2, SUB1: output phase given to each input bin in the previous frame
	float* drift[NVOICES]; // phase a detuned voice has run ahead by, per input bin

	static const int kFracSteps = 32;
	float fracRe[kFracSteps + 1], fracIm[kFracSteps + 1]; // 1 / (what linear interpolation does to a partial)

	int silentTicks;
	bool analysed;       // the last analysis tick found something to build voices from
	bool synthesisNext;
};

#endif
