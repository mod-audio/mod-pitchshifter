#ifndef MOD_PITCHSHIFTER_BLOCKADAPTER_H
#define MOD_PITCHSHIFTER_BLOCKADAPTER_H

#include <stdint.h>

/* Feeds a processor that works on a fixed hop size from host blocks of any size.
 *
 * The pitch shifters are built at instantiate for one hop (the host's nominal
 * block length). While the host delivers exactly that, audio goes straight
 * through with no added latency. The first time it delivers something else the
 * adapter switches, permanently, to buffering: input is collected until a full
 * hop is available, and output is what the previous hop produced, which adds
 * one hop of latency. Nothing is allocated after construction, so this is safe
 * on the audio thread; the old code re-created the whole FFT engine instead.
 */
class BlockAdapter
{
public:
	BlockAdapter(uint32_t hop, int nOutputs)
		: hop(hop), nout(nOutputs), fill(0), buffered(false)
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

	/* process(const float* in, float* const* outs) is called once per full hop. */
	template<class F>
	void Run(const float* in, float* const* outs, uint32_t n, F process)
	{
		if (!buffered && n == hop)
		{
			process(in, outs);
			return;
		}

		buffered = true;

		for (uint32_t i = 0; i < n; i++)
		{
			inbuf[fill] = in[i];
			for (int k = 0; k < nout; k++)
				outs[k][i] = outbuf[k][fill];

			if (++fill == hop)
			{
				process(inbuf, outbuf);
				fill = 0;
			}
		}
	}

private:
	const uint32_t hop;
	const int nout;
	uint32_t fill;
	bool buffered;
	float* inbuf;
	float** outbuf;
};

#endif
