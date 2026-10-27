# Builds the eight pitch shifter plugins. Shared_files/harmonizer.wisdom (FFTW plans for the frame
# sizes the plugins use) is generated here when missing; builders copy a per-device file in first.

PLUGINS = 2Voices Capo Drop SuperCapo SuperWhammy Harmonizer Harmonizer2 HarmonizerCS
WISDOM  = Shared_files/harmonizer.wisdom
SIZES   = 128 192 256 384 512 640 768 1024 1280 1536 2048 2176 2304 2432 2560 3072 4096

.PHONY: all install clean

all: $(WISDOM)
	for p in $(PLUGINS); do $(MAKE) -C $$p || exit 1; done

install: all
	for p in $(PLUGINS); do $(MAKE) -C $$p install || exit 1; done

clean:
	for p in $(PLUGINS); do $(MAKE) -C $$p clean || exit 1; done
	rm -f Shared_files/*.o $(WISDOM)

$(WISDOM):
	@echo "Generating $@ (this takes a while)"
	fftwf-wisdom -v -n -x -o $@ $(foreach n,$(SIZES),rof$(n) rob$(n))
