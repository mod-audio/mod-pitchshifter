
all: Shared_files/harmonizer.wisdom
	$(MAKE) -C Gx2Voices
	$(MAKE) -C GxCapo
	$(MAKE) -C GxDrop
	$(MAKE) -C GxSuperCapo
	$(MAKE) -C GxSuperWhammy
	$(MAKE) -C GxHarmonizer
	$(MAKE) -C GxHarmonizer2
	$(MAKE) -C GxHarmonizerCS

install: all
	$(MAKE) -C Gx2Voices install
	$(MAKE) -C GxCapo install
	$(MAKE) -C GxDrop install
	$(MAKE) -C GxSuperCapo install
	$(MAKE) -C GxSuperWhammy install
	$(MAKE) -C GxHarmonizer install
	$(MAKE) -C GxHarmonizer2 install
	$(MAKE) -C GxHarmonizerCS install

clean:
	$(MAKE) -C Gx2Voices clean
	$(MAKE) -C GxCapo clean
	$(MAKE) -C GxDrop clean
	$(MAKE) -C GxSuperCapo clean
	$(MAKE) -C GxSuperWhammy clean
	$(MAKE) -C GxHarmonizer clean
	$(MAKE) -C GxHarmonizer2 clean
	$(MAKE) -C GxHarmonizerCS clean
	rm -f Shared_files/*.o
	rm -f Shared_files/harmonizer.wisdom

Shared_files/harmonizer.wisdom:
	@echo "Generating harmonizer.wisdom file, this might take a while..."
	fftwf-wisdom -v -n -x -o $@ \
	rof128 rob128 rof192 rob192 rof256 rob256 rof384 rob384 rof512 rob512 rof640 rob640 rof768 rob768 \
	rof1024 rob1024 rof1280 rob1280 rof1536 rob1536 rof2048 rob2048 rof2176 rob2176 rof2304 rob2304 \
	rof2432 rob2432 rof2560 rob2560 rof3072 rob3072 rof4096 rob4096
