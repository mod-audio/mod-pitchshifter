#include <cmath>
#include <cstring>
#include <string>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include "../../PolyOctaver/src/OctaveEngine.h"
#include "Sequence.h"
#include "BlockAdapter.h"
#include "Wisdom.h"

/**********************************************************************************************************************************************************/

#define PLUGIN_URI "http://moddevices.com/plugins/mod-devel/PitchArpeggiator"

enum {IN, OUT, EVENTS, SYNC, BPM, DIVISION, PATTERN, ARP_MODE, OCTAVE, OCTAVE_SPREAD, OCTAVE_MODE, NOTE_LENGTH, FREEZE, DRY, LEVEL, PLUGIN_PORT_COUNT};

// Step length per Division setting, in quarter notes: 1/1, 1/2, 1/3, 1/4, 1/4., 1/4T, 1/8, 1/8., 1/8T, 1/16, 1/16., 1/16T, 1/32.
// The same thirteen as the MIDI Arpeggiator.
#define NDIVISIONS 13
static const double kStepBeats[NDIVISIONS] = { 4.0, 2.0, 4.0/3.0, 1.0, 0.75, 2.0/3.0, 0.5, 0.375, 1.0/3.0, 0.25, 0.1875, 1.0/6.0, 0.125 };

struct Settings
{
    double beatsPerSample;  // tempo
    double stepBeats;       // length of a step, quarter notes
    int pattern, arpMode, octave, spread, octaveMode;
    float noteLength;       // 0..1 of the step
    bool freeze;
    float dry, level;       // linear
};

/**********************************************************************************************************************************************************/

class PitchArpeggiator
{
public:
    PitchArpeggiator(double samplerate, const std::string& wfile, const LV2_Feature* const* features)
    {
        SampleRate = samplerate;

        const bool haveWisdom = ImportWisdom(wfile.c_str());
        const OctaveEngine::Config config = OctaveEngine::DefaultConfig(samplerate);
        engine = new OctaveEngine(config, haveWisdom);
        hop = engine->Hop();
        wet = new float[hop]();
        adapter = new BlockAdapter(hop, 1);

        // A frame takes the interval that is current when it is built, and is heard as a window
        // around a point half a synthesis window later. Frames come one frame hop apart, so a change
        // lands up to half a hop either side of where it is aimed (+-2.7 ms); looking this far ahead
        // puts the middle of that range on the beat (test/arp_test.cpp measures it).
        lead = config.synth/2 - config.hop/4;

        // The gate opens in 3 ms and closes in 8 ms.
        gateUp = (float)(1.0/(0.003*samplerate));
        gateDown = (float)(1.0/(0.008*samplerate));

        map = NULL;
        for (int i = 0; features && features[i]; i++)
            if (!strcmp(features[i]->URI, LV2_URID__map))
                map = (LV2_URID_Map*)features[i]->data;
        if (map)
        {
            uridBlank    = map->map(map->handle, LV2_ATOM__Blank);
            uridObject   = map->map(map->handle, LV2_ATOM__Object);
            uridFloat    = map->map(map->handle, LV2_ATOM__Float);
            uridDouble   = map->map(map->handle, LV2_ATOM__Double);
            uridInt      = map->map(map->handle, LV2_ATOM__Int);
            uridLong     = map->map(map->handle, LV2_ATOM__Long);
            uridPosition = map->map(map->handle, LV2_TIME__Position);
            uridBar      = map->map(map->handle, LV2_TIME__bar);
            uridBarBeat  = map->map(map->handle, LV2_TIME__barBeat);
            uridBpb      = map->map(map->handle, LV2_TIME__beatsPerBar);
            uridBpm      = map->map(map->handle, LV2_TIME__beatsPerMinute);
            uridSpeed    = map->map(map->handle, LV2_TIME__speed);
        }

        ports[EVENTS] = NULL;
        hostBpm = 0.0;
        hostBeatsPerBar = 4.0;
        hostRolling = false;
        Restart();
    }
    ~PitchArpeggiator()
    {
        delete engine;
        delete[] wet;
        delete adapter;
    }

    void Restart()
    {
        engine->Reset();
        beat = 0.0;
        gate = 0.0f;
        dry_1 = level_1 = 0.0f;
    }

    // Level controls are 0 to 100 %, squared: 100 % is unity, 70 % about -6 dB.
    static float LevelToGain(float percent)
    {
        if (!(percent > 0.0f)) return 0.0f;
        if (percent > 100.0f) percent = 100.0f;
        return percent * percent * 1e-4f;
    }

    static double AtomNumber(const LV2_Atom* a, const PitchArpeggiator* p, double otherwise)
    {
        if (!a) return otherwise;
        if (a->type == p->uridFloat)  return ((const LV2_Atom_Float*)a)->body;
        if (a->type == p->uridDouble) return ((const LV2_Atom_Double*)a)->body;
        if (a->type == p->uridInt)    return ((const LV2_Atom_Int*)a)->body;
        if (a->type == p->uridLong)   return (double)((const LV2_Atom_Long*)a)->body;
        return otherwise;
    }

    // What the host says about tempo and song position. While the transport runs, `beat` follows
    // the song; stopped, it runs on freely at the host's tempo.
    void ReadHostTime(bool follow)
    {
        const LV2_Atom_Sequence* const seq = (const LV2_Atom_Sequence*)ports[EVENTS];
        if (!seq || !map) return;

        LV2_ATOM_SEQUENCE_FOREACH(seq, ev)
        {
            if (ev->body.type != uridObject && ev->body.type != uridBlank) continue;
            const LV2_Atom_Object* const obj = (const LV2_Atom_Object*)&ev->body;
            if (obj->body.otype != uridPosition) continue;

            const LV2_Atom *bar = NULL, *barBeat = NULL, *bpb = NULL, *bpm = NULL, *speed = NULL;
            lv2_atom_object_get(obj, uridBar, &bar, uridBarBeat, &barBeat, uridBpb, &bpb, uridBpm, &bpm, uridSpeed, &speed, 0);

            const double newBpm = AtomNumber(bpm, this, hostBpm);
            if (newBpm >= 10.0 && newBpm <= 1000.0) hostBpm = newBpm;
            const double newBpb = AtomNumber(bpb, this, hostBeatsPerBar);
            if (newBpb >= 1.0 && newBpb <= 64.0) hostBeatsPerBar = newBpb;
            if (speed) hostRolling = AtomNumber(speed, this, 0.0) != 0.0;

            if (follow && hostRolling && bar && barBeat)
            {
                // the song position at the start of this block; a difference under a thousandth of
                // a beat is rounding, and following it could play a step boundary twice
                const double here = AtomNumber(bar, this, 0.0)*hostBeatsPerBar + AtomNumber(barBeat, this, 0.0)
                                  - (double)ev->time.frames*hostBpm/(60.0*SampleRate);
                if (here >= 0.0 && fabs(here - beat) > 1e-3) beat = here;
            }
        }
    }

    // One hop of audio.
    void Process(const float *in, float *out, const Settings& set)
    {
        // the engine's interval: that of the step the frame built now will be heard in
        const long step = (long)floor((beat + lead*set.beatsPerSample)/set.stepBeats);
        const int semitones = Sequence::Semitones(step, set.pattern, set.arpMode, set.octave, set.spread, set.octaveMode);

        float gains[OctaveEngine::NVOICES] = { 0, 0, 0, 0, 0 }, detune[OctaveEngine::NVOICES] = { 1, 1, 1, 1, 1 };
        gains[OctaveEngine::UNISON] = set.level > 0.0f || level_1 > 0.0f ? 1.0f : 0.0f;
        detune[OctaveEngine::UNISON] = (float)pow(2.0, semitones/12.0);

        engine->Hold(set.freeze);
        engine->Process(in, wet, gains, detune);

        // The gate, on the beat to the sample: open for Note Length of every step, never closed at 100 %.
        const bool legato = set.noteLength >= 0.995f;
        const double length = set.noteLength < 0.05f ? 0.05 : set.noteLength;
        const double perSample = set.beatsPerSample/set.stepBeats;
        double position = beat/set.stepBeats;
        position -= floor(position);

        const float dryStep = (set.dry - dry_1)/hop;
        const float levelStep = (set.level - level_1)/hop;

        for (uint32_t i = 0; i < hop; i++)
        {
            if (legato || position < length)
            {
                gate += gateUp;
                if (gate > 1.0f) gate = 1.0f;
            }
            else
            {
                gate -= gateDown;
                if (gate < 0.0f) gate = 0.0f;
            }
            position += perSample;
            if (position >= 1.0) position -= 1.0;

            dry_1 += dryStep;
            level_1 += levelStep;
            out[i] = dry_1*in[i] + level_1*gate*wet[i];
        }
        dry_1 = set.dry;
        level_1 = set.level;

        beat += hop*set.beatsPerSample;
    }

    static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features);
    static void activate(LV2_Handle instance);
    static void deactivate(LV2_Handle instance);
    static void connect_port(LV2_Handle instance, uint32_t port, void *data);
    static void run(LV2_Handle instance, uint32_t n_samples);
    static void cleanup(LV2_Handle instance);
    static const void* extension_data(const char* uri);
    void *ports[PLUGIN_PORT_COUNT];

    OctaveEngine *engine;
    BlockAdapter *adapter;
    float *wet;

    uint32_t hop;
    int lead;               // samples the engine's step runs ahead of the gate's
    double SampleRate;
    double beat;            // quarter notes, at the next sample to process
    float gate, gateUp, gateDown;
    float dry_1, level_1;

    double hostBpm, hostBeatsPerBar;
    bool hostRolling;

    LV2_URID_Map *map;
    LV2_URID uridBlank, uridObject, uridFloat, uridDouble, uridInt, uridLong;
    LV2_URID uridPosition, uridBar, uridBarBeat, uridBpb, uridBpm, uridSpeed;
};

/**********************************************************************************************************************************************************/

static const LV2_Descriptor Descriptor = {
    PLUGIN_URI,
    PitchArpeggiator::instantiate,
    PitchArpeggiator::connect_port,
    PitchArpeggiator::activate,
    PitchArpeggiator::run,
    PitchArpeggiator::deactivate,
    PitchArpeggiator::cleanup,
    PitchArpeggiator::extension_data
};

/**********************************************************************************************************************************************************/

LV2_SYMBOL_EXPORT
const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
    if (index == 0) return &Descriptor;
    else return NULL;
}

/**********************************************************************************************************************************************************/

LV2_Handle PitchArpeggiator::instantiate(const LV2_Descriptor* descriptor, double samplerate, const char* bundle_path, const LV2_Feature* const* features)
{
    std::string wisdomFile = bundle_path;
    wisdomFile += "/harmonizer.wisdom";
    PitchArpeggiator *plugin = new PitchArpeggiator(samplerate, wisdomFile, features);
    return (LV2_Handle)plugin;
}

/**********************************************************************************************************************************************************/

void PitchArpeggiator::activate(LV2_Handle instance)
{
    PitchArpeggiator *plugin = (PitchArpeggiator *) instance;
    plugin->Restart();
}

/**********************************************************************************************************************************************************/

void PitchArpeggiator::deactivate(LV2_Handle instance){}

/**********************************************************************************************************************************************************/

void PitchArpeggiator::connect_port(LV2_Handle instance, uint32_t port, void *data)
{
    PitchArpeggiator *plugin;
    plugin = (PitchArpeggiator *) instance;
    if (port < PLUGIN_PORT_COUNT)
        plugin->ports[port] = data;
}

/**********************************************************************************************************************************************************/

static inline float control(void* const* ports, int index)
{
    return *(const float*)ports[index];
}

static inline int choice(void* const* ports, int index, int lowest, int highest)
{
    const float v = control(ports, index);
    const int i = (int)floor(v + 0.5f);
    return !(v == v) || i < lowest ? lowest : i > highest ? highest : i;
}

void PitchArpeggiator::run(LV2_Handle instance, uint32_t n_samples)
{
    PitchArpeggiator *plugin;
    plugin = (PitchArpeggiator *) instance;
    void* const* const ports = plugin->ports;

    const float *in  = (const float*)ports[IN];
    float       *out = (float*)ports[OUT];

    const bool hostSync = choice(ports, SYNC, 0, 1) == 1;
    plugin->ReadHostTime(hostSync);

    // Host sync without a host tempo (nothing received yet) runs on the BPM control.
    double bpm = control(ports, BPM);
    if (hostSync && plugin->hostBpm > 0.0) bpm = plugin->hostBpm;
    if (!(bpm >= 20.0)) bpm = 20.0;
    if (bpm > 300.0) bpm = 300.0;

    Settings set;
    set.beatsPerSample = bpm/(60.0*plugin->SampleRate);
    set.stepBeats   = kStepBeats[choice(ports, DIVISION, 0, NDIVISIONS - 1)];
    set.pattern     = choice(ports, PATTERN, 0, Sequence::kNumPatterns - 1);
    set.arpMode     = choice(ports, ARP_MODE, 0, Sequence::NARPMODES - 1);
    set.octave      = choice(ports, OCTAVE, -2, 1);
    set.spread      = choice(ports, OCTAVE_SPREAD, 1, Sequence::kMaxSpread);
    set.octaveMode  = choice(ports, OCTAVE_MODE, 0, Sequence::NOCTAVEMODES - 1);
    set.noteLength  = control(ports, NOTE_LENGTH)*0.01f;  // percent of the step
    set.freeze      = control(ports, FREEZE) > 0.5f;
    set.dry         = LevelToGain(control(ports, DRY));
    set.level       = LevelToGain(control(ports, LEVEL));

    // The engine runs on a fixed hop. Host blocks that are a whole number of hops (128 and 256 on
    // the MOD devices) go straight through; anything else goes through the adapter, one hop late.
    const uint32_t hop = plugin->hop;
    if (!plugin->adapter->Buffered() && n_samples % hop == 0)
    {
        for (uint32_t off = 0; off < n_samples; off += hop)
            plugin->Process(in + off, out + off, set);
        return;
    }

    float* const outs[1] = { out };
    plugin->adapter->Run(in, outs, n_samples, [&](const float *hin, float* const* houts)
    {
        plugin->Process(hin, houts[0], set);
    });
}

/**********************************************************************************************************************************************************/

void PitchArpeggiator::cleanup(LV2_Handle instance)
{
    delete ((PitchArpeggiator *) instance);
}

/**********************************************************************************************************************************************************/

const void* PitchArpeggiator::extension_data(const char* uri)
{
    return NULL;
}
