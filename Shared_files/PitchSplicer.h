#pragma once
#include <cstddef>
#include <memory>

/**
 * Correlation-spliced pitch shifter for the MOD-pitchshifter family's "Ultra"
 * Fidelity level. Ported from tone3000-plugin/plugin/src/PitchShift.{h,cpp}
 * (MIT licensed, see that file and its docs/pitch-shift.md + PITCHSHIFT-SIMD.md
 * for the full engine research). Deviations from the original: process() takes
 * separate input/output pointers instead of an in-place AudioBuffer (LV2 audio
 * ports), prepare() does not need a block size (the engine is per-sample and
 * never allocates after it), and the tonality crossover was dropped along with
 * Params.tonalityHz — every MOD usage is a pure shift, which was also the
 * tone3000 default.
 *
 * Engine in one paragraph: one read tap runs through a 20-60 ms ring buffer at
 * the pitch ratio; when it reaches the end of its range it splices to where the
 * recent waveform best matches (normalised cross-correlation, scored as damage
 * per splice over splices per second) and the two taps crossfade. On periodic
 * material that is a whole number of periods, so the joint is inaudible; on a
 * chord the fade stretches to hide partials that don't line up. Pick attacks re-
 * sync the tap to the freshest end of the buffer, so felt latency follows the
 * floor (2 ms), not the drift position. Time-domain: bass shifts as cleanly as
 * guitar — no FFT anywhere in the audio path, and block size is irrelevant
 * (which also makes this level immune to the STFT family's FIDELITY-table /
 * host-block-size sensitivity). CPU ~0.15-0.35% of one core; a drift splice's
 * lag search is spread over the 4 ms before it lands, so no block carries a
 * burst beyond what tone3000 measured (worst 16-sample callback ~100 us at
 * 48 kHz). Added latency: the tap's mean delay, (floor + window) / 2 —
 * 11/16/21/31 ms for the four windows.
 */

class PitchSplicer {
public:
  static constexpr int kMaxChannels = 2;
  // The delay buffer the read tap drifts across; one detent each, in ms. The
  // floor is the tap's closest approach to the write head (kMinDelayMs), where
  // attacks are re-synced to.
  enum class Window { ms20, ms30, ms40, ms60 };
  static int windowMs(Window w);   // buffer size in ms: 20/30/40/60 (table lives with the engine)
  static constexpr Window kDefaultWindow = Window::ms30;
  static constexpr double kMinDelayMs = 2.0;

  /** The user-facing controls in real units. Semitones is continuous (the LV2
      STEP ports hand over whole semitones); the engine never knows about
      stepping. */
  struct Params {
    float semitones = 0.0f;
    Window window = kDefaultWindow;
  };

  PitchSplicer();
  ~PitchSplicer();

  /** Sizes everything for this rate (rings hold the largest window). Must be
      called before any audio-thread method. */
  void prepare(double sampleRate);

  /** Power, kept from the original: off blends the shift out against dry and
      isRunning() stays true until that lands; on restarts at the floor and
      blends in once the ring is primed (no click onto cold state). LV2 usage
      enables it right after prepare(). */
  void setEnabled(bool on);
  bool isRunning() const;

  /** Audio thread, once per block. Each field early-outs when unchanged. */
  void setParams(const Params& p);

  /** Audio thread: shifts numChannels (1 or 2) from in to out for numSamples.
      The buffers must not overlap. A no-op unless running; the channels share
      one control path (onset detection and lag search run on their mean), so
      a stereo image never smears — each channel keeps its own ring. */
  void process(const float* const* in, float** out, int numSamples, int numChannels);

  /** Added latency for a window at the given rate: the tap's mean delay —
      (floor + window) / 2, i.e. ~11/16/21/31 ms for the four windows at any
      sane rate. The semitone value never moves it. */
  static int minDelaySamples(double sampleRate);
  static int windowSamples(Window w, double sampleRate);
  static int latencySamples(Window w, double sampleRate) {
    return (minDelaySamples(sampleRate) + windowSamples(w, sampleRate)) / 2;
  }

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
