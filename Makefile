
all: Shared_files/arc.h Shared_files/Exponencial.h Shared_files/Cos.h Shared_files/Sin.h Shared_files/Sin.h Shared_files/harmonizer.wisdom
	$(MAKE) -C 2Voices
	$(MAKE) -C Capo
	$(MAKE) -C Drop
	$(MAKE) -C SuperCapo
	$(MAKE) -C SuperWhammy
	$(MAKE) -C Harmonizer
	$(MAKE) -C Harmonizer2
	$(MAKE) -C HarmonizerCS

install: all
	$(MAKE) -C 2Voices install
	$(MAKE) -C Capo install
	$(MAKE) -C Drop install
	$(MAKE) -C SuperCapo install
	$(MAKE) -C SuperWhammy install
	$(MAKE) -C Harmonizer install
	$(MAKE) -C Harmonizer2 install
	$(MAKE) -C HarmonizerCS install

clean:
	$(MAKE) -C 2Voices clean
	$(MAKE) -C Capo clean
	$(MAKE) -C Drop clean
	$(MAKE) -C SuperCapo clean
	$(MAKE) -C SuperWhammy clean
	$(MAKE) -C Harmonizer clean
	$(MAKE) -C Harmonizer2 clean
	$(MAKE) -C HarmonizerCS clean
	rm -f Shared_files/*.o
	rm -f Shared_files/arc.h
	rm -f Shared_files/Exponencial.h
	rm -f Shared_files/Cos.h
	rm -f Shared_files/Sin.h
	rm -f Shared_files/harmonizer.wisdom

Shared_files/arc.h:
	@echo "Generating arc.h file, this might take a while..."
	cd Shared_files && ./AtanTable.py

Shared_files/Exponencial.h:
	@echo "Generating Exponencial.h file, this might take a while..."
	cd Shared_files && ./CxExpTable.py # 1000

Shared_files/Cos.h:
	@echo "Generating Cos.h file, this might take a while..."
	cd Shared_files && ./CosTable.py

Shared_files/Sin.h:
	@echo "Generating Sin.h file, this might take a while..."
	cd Shared_files && ./SinTable.py

Shared_files/harmonizer.wisdom:
	@echo "Generating harmonizer.wisdom file, this might take a while..."
	fftwf-wisdom -n -x -o $@ \
	rof1024 rob1024 rof1536 rob1536 rof2048 rob2048 rof2176 rob2176 rof2304 rob2304 rof2432 rob2432 rof2560 rob2560 rof3072 rob3072 rof4096 rob4096
