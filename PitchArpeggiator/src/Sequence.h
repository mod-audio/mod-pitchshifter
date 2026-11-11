#ifndef MOD_PITCHARPEGGIATOR_SEQUENCE_H
#define MOD_PITCHARPEGGIATOR_SEQUENCE_H

/* Which interval, in semitones from what is played, step number `step` of the arpeggio has.
 *
 * A pure function of the step number: under host sync the step number comes from the song
 * position, so the same bar always plays the same notes, and there is no state to reset.
 *
 * A MIDI arpeggiator plays the notes that are held; this one has no notes, so the chord is a
 * setting (Pattern) and whatever comes in is its root.
 */

namespace Sequence
{

enum ArpMode { ARP_UP = 0, ARP_DOWN, ARP_UP_DOWN, ARP_UP_DOWN_ALT, ARP_RANDOM, NARPMODES };
enum OctaveMode { OCT_UP = 0, OCT_DOWN, OCT_UP_DOWN, OCT_DOWN_UP, OCT_CYCLE, NOCTAVEMODES };

static const int kMaxNotes = 5;
static const int kMaxSpread = 3;
static const int kMaxSemitones = 24; // the engine goes two octaves either way

struct Pattern { const char* name; int size; int notes[kMaxNotes]; };

static const Pattern kPatterns[] = {
	{ "Major",            3, { 0, 4, 7 } },
	{ "Minor",            3, { 0, 3, 7 } },
	{ "Major 7",          4, { 0, 4, 7, 11 } },
	{ "Minor 7",          4, { 0, 3, 7, 10 } },
	{ "Dominant 7",       4, { 0, 4, 7, 10 } },
	{ "Sus 4",            3, { 0, 5, 7 } },
	{ "Diminished",       4, { 0, 3, 6, 9 } },
	{ "Fifths",           2, { 0, 7 } },
	{ "Octaves",          1, { 0 } },
	{ "Major Pentatonic", 5, { 0, 2, 4, 7, 9 } },
	{ "Minor Pentatonic", 5, { 0, 3, 5, 7, 10 } },
};
static const int kNumPatterns = sizeof(kPatterns) / sizeof(kPatterns[0]);

// 0 1 2 1 0 1 2 ...: the ends once
inline int UpDown(long n, int size)
{
	if (size <= 1) return 0;
	const int period = 2 * size - 2;
	const int m = static_cast<int>(n % period);
	return m < size ? m : period - m;
}

// 0 1 2 2 1 0 0 1 ...: the ends twice
inline int UpDownAlt(long n, int size)
{
	const int m = static_cast<int>(n % (2 * size));
	return m < size ? m : 2 * size - 1 - m;
}

inline unsigned Hash(unsigned long n)
{
	unsigned x = static_cast<unsigned>(n) * 2654435761u;
	x ^= x >> 15; x *= 2246822519u; x ^= x >> 13; x *= 3266489917u; x ^= x >> 16;
	return x;
}

// Position `n` of mode `mode` in a list of `size` entries.
inline int Walk(long n, int size, int mode)
{
	switch (mode)
	{
		case ARP_DOWN:        return size - 1 - static_cast<int>(n % size);
		case ARP_UP_DOWN:     return UpDown(n, size);
		case ARP_UP_DOWN_ALT: return UpDownAlt(n, size);
		case ARP_RANDOM:      return static_cast<int>(Hash(static_cast<unsigned long>(n)) % static_cast<unsigned>(size));
		default:              return static_cast<int>(n % size);
	}
}

/* `octave`: where the arpeggio starts, in octaves from the input (-2 .. +1).
 * `spread`: how many octaves it covers (1 .. 3).
 * `octaveMode`: Cycle walks the chord through the whole range as one long list, which is what
 * Up, Down and the others then apply to. The other four keep the walk inside one octave and move
 * the octave on every step instead, as the MIDI Arpeggiator does. */
inline int Semitones(long step, int pattern, int arpMode, int octave, int spread, int octaveMode)
{
	if (step < 0) step = 0;
	if (pattern < 0 || pattern >= kNumPatterns) pattern = 0;
	if (arpMode < 0 || arpMode >= NARPMODES) arpMode = ARP_UP;
	if (octaveMode < 0 || octaveMode >= NOCTAVEMODES) octaveMode = OCT_CYCLE;
	if (spread < 1) spread = 1;
	if (spread > kMaxSpread) spread = kMaxSpread;

	const Pattern& p = kPatterns[pattern];
	int note, oct;
	if (octaveMode == OCT_CYCLE)
	{
		const int at = Walk(step, p.size * spread, arpMode);
		note = p.notes[at % p.size];
		oct = at / p.size;
	}
	else
	{
		note = p.notes[Walk(step, p.size, arpMode)];
		switch (octaveMode)
		{
			case OCT_DOWN:    oct = spread - 1 - static_cast<int>(step % spread); break;
			case OCT_UP_DOWN: oct = UpDown(step, spread); break;
			case OCT_DOWN_UP: oct = spread - 1 - UpDown(step, spread); break;
			default:          oct = static_cast<int>(step % spread); break;
		}
	}

	int semitones = 12 * (octave + oct) + note;
	while (semitones > kMaxSemitones) semitones -= 12;
	while (semitones < -kMaxSemitones) semitones += 12;
	return semitones;
}

}

#endif
