#ifndef MOD_PITCHSHIFTER_BLOCKADAPTER_H
#define MOD_PITCHSHIFTER_BLOCKADAPTER_H

#include <stdint.h>
#include <string.h>

/* The engine hop for a host block length: never below 128.
 *
 * The engine's frame is a whole number of hops, so a 64-sample hop runs the FFTs,
 * the pitch detection and the resynthesis twice as often for the same output. With
 * a 128-sample hop at 64 frames the engine does what it does at 128 frames, and
 * sounds the same; the adapter below adds one hop of latency, which is what a 128
 * period costs anyway. */
static inline uint32_t EngineHop(uint32_t hostBlock)
{
	return hostBlock < 128 ? 128 : hostBlock;
}

/* Feeds a processor that works on a fixed hop size from host blocks of any size.
 *
 * Each hop is processed in two steps: analyse(in, outs) and synthesise(outs).
 * analyse either writes the hop's output itself and returns false (silence,
 * warm-up), or returns true and leaves the output to synthesise.
 *
 * While the host delivers exactly one hop per run(), both steps run in that call
 * and audio goes straight through with no added latency. Otherwise (a host block
 * smaller than the hop, or any block that differs from it) the adapter switches,
 * permanently, to buffering: input is collected until a full hop is available,
 * output is what the previous hop produced, which adds one hop of latency. When a
 * hop completes on the last sample of a run(), synthesise is deferred to the start
 * of the next run(): at 64 frames with a 128 hop the analysis and the synthesis
 * fall in different cycles, so the worst cycle does about half a hop's work.
 * Nothing is allocated after construction, so this is safe on the audio thread. */
class BlockAdapter
{
public:
	BlockAdapter(uint32_t hop, int nOutputs)
		: hop(hop), nout(nOutputs), fill(0), buffered(false), pending(false)
	{
		inbuf = new float[hop]();
		outbuf = new float*[nout];
		for (int k = 0; k < nout; k++)
			outbuf[k] = new float[hop]();
	}

	~BlockAdapter()
	{
		for (int k = 0; k < nout; k++)
			delete[] outbuf[k];
		delete[] outbuf;
		delete[] inbuf;
	}

	uint32_t Hop() const { return hop; }
	bool Buffered() const { return buffered; }

	/* The engine was reset between the two steps of a hop: that hop comes out silent. */
	void Cancel()
	{
		if (!pending)
			return;
		pending = false;
		for (int k = 0; k < nout; k++)
			memset(outbuf[k], 0, sizeof(float)*hop);
	}

	/* analyse(const float* in, float* const* outs) -> bool and
	 * synthesise(float* const* outs) work on one full hop. */
	template<class A, class S>
	void Run(const float* in, float* const* outs, uint32_t n, A analyse, S synthesise)
	{
		if (!buffered && n == hop)
		{
			if (analyse(in, outs))
				synthesise(outs);
			return;
		}

		buffered = true;

		if (pending)
		{
			synthesise(outbuf);
			pending = false;
		}

		for (uint32_t i = 0; i < n; i++)
		{
			inbuf[fill] = in[i];
			for (int k = 0; k < nout; k++)
				outs[k][i] = outbuf[k][fill];

			if (++fill == hop)
			{
				fill = 0;
				pending = analyse(inbuf, outbuf);
				if (pending && i + 1 < n)
				{
					synthesise(outbuf);
					pending = false;
				}
			}
		}
	}

private:
	const uint32_t hop;
	const int nout;
	uint32_t fill;
	bool buffered;
	bool pending;
	float* inbuf;
	float** outbuf;
};

#endif
