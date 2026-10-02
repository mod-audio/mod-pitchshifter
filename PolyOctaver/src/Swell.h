#ifndef MOD_POLYOCTAVER_SWELL_H
#define MOD_POLYOCTAVER_SWELL_H

#include <cmath>

/* The Attack control: every new note or chord fades in over the set time.
 *
 * Finding the notes is the hard part. The overall level will not do: two strings beating rise
 * out of a null exactly like a new note, and a note picked over a ringing chord hardly moves it.
 * Above 1.5 kHz it is different: what rings there dies away quickly, and every pick puts a burst
 * there within a millisecond or two. So a note starts where the level of that band jumps, within
 * 4 ms, to well over (1.6 times) the highest it has been in the last tenth of a second or so.
 * A ringing chord's waveform comes in bursts too, once per period of its common fundamental,
 * but each burst is only as high as the last one; beating rises too slowly; a decaying chord does
 * not rise at all. The first note out of silence is caught by the input coming up through
 * -54 dBFS.
 *
 * On a new note the gain goes back to zero (in 2 ms, so that cutting what still sounds does not
 * click) and then up again over the attack time, slowly at first. Nothing is allocated after
 * construction. */
class Swell
{
public:
	explicit Swell(double sampleRate)
		: rate(sampleRate)
	{
		highpass = std::exp(-2.0 * M_PI * 1500.0 / rate);
		bandRelease = std::exp(-1.0 / (0.005 * rate));
		heldRelease = std::exp(-1.0 / (0.100 * rate));
		levelRelease = std::exp(-1.0 / (0.050 * rate));
		fallStep = static_cast<float>(1.0 / (0.002 * rate));
		holdSamples = static_cast<int>(0.050 * rate);
		lookback = static_cast<int>(0.004 * rate);
		history = new float[lookback]();
		Reset();
	}

	~Swell() { delete[] history; }

	void Reset()
	{
		for (int i = 0; i < lookback; ++i) history[i] = 0;
		at = 0;
		lp1 = lp2 = 0;
		band = held = level = 0;
		ramp = applied = 1;
		hold = 0;
		quiet = true;
		triggers = 0;
	}

	int Triggers() const { return triggers; }

	/* Gains for one block of input, into `gain`. attackMs < 1 means off: all ones. */
	void Process(const float* in, float* gain, int n, float attackMs)
	{
		const bool on = attackMs >= 1.0f;
		const float rise = on ? static_cast<float>(1000.0 / (attackMs * rate)) : 1.0f;

		for (int i = 0; i < n; ++i)
		{
			const float x = in[i];

			// two one-pole high-passes in a row
			lp1 = static_cast<float>(x + (lp1 - x) * highpass);
			const float h1 = x - lp1;
			lp2 = static_cast<float>(h1 + (lp2 - h1) * highpass);
			const float high = std::fabs(h1 - lp2);

			band = high > band ? high : static_cast<float>(band * bandRelease);
			const float before = history[at]; // the band's level 4 ms ago
			history[at] = band;
			if (++at == lookback) at = 0;
			const float heldBefore = held;
			held = before > held ? before : static_cast<float>(held * heldRelease);

			const float magnitude = std::fabs(x);
			level = magnitude > level ? magnitude : static_cast<float>(level * levelRelease);

			if (hold > 0) --hold;
			bool start = false;
			if (hold == 0)
			{
				// a pick; the floor keeps the last flickers of a dying chord from counting as one
				const float floor = level * 0.01f > 3e-4f ? level * 0.01f : 3e-4f;
				if (band > 1.6f * heldBefore + floor && band > 2.0f * before) start = true;
				if (quiet && level > 0.002f) start = true;                   // the first sound after silence
			}
			if (level > 0.002f) quiet = false;
			else if (level < 0.0005f) quiet = true;

			if (start)
			{
				ramp = 0;
				hold = holdSamples;
				++triggers;
			}

			ramp += rise;
			if (ramp > 1.0f) ramp = 1.0f;

			const float target = on ? ramp * ramp : 1.0f;
			applied = target < applied - fallStep ? applied - fallStep : target;
			gain[i] = applied;
		}
		if (band < 1e-20f) band = 0;
		if (held < 1e-20f) held = 0;
		if (level < 1e-20f) level = 0;
		if (std::fabs(lp1) < 1e-20f) lp1 = 0;
		if (std::fabs(lp2) < 1e-20f) lp2 = 0;
	}

private:
	Swell(const Swell&);
	Swell& operator=(const Swell&);

	const double rate;
	double highpass, bandRelease, heldRelease, levelRelease;
	float fallStep;
	int holdSamples, lookback;

	float* history;       // the band's level over the last 4 ms
	int at;
	float lp1, lp2;
	float band, held;     // level above 1.5 kHz, and the highest it has been lately
	float level;          // level of the whole input
	float ramp, applied;  // 0..1 since the last note; the gain actually given out
	int hold;             // samples before another note may start
	bool quiet;
	int triggers;
};

#endif
