# Offline checks, run on the build machine: `make -C PitchArpeggiator/test run`. Needs fftw3f and the LV2 headers.

CXX ?= g++
CXXFLAGS += -O2 -std=gnu++11 -Wall -I. -I../src -I../../PolyOctaver/src -I../../PolyOctaver/test -I../../Shared_files $(shell pkg-config --cflags fftw3f)
LDLIBS += $(shell pkg-config --libs fftw3f) -lm

ENGINE = ../../PolyOctaver/src/OctaveEngine.cpp ../../PolyOctaver/src/OctaveEngine.h

all: interval_test arp_test mod-pitcharpeggiator.so

interval_test: interval_test.cpp $(ENGINE)
	$(CXX) $(CXXFLAGS) interval_test.cpp ../../PolyOctaver/src/OctaveEngine.cpp ../../Shared_files/Wisdom.cpp $(LDLIBS) -o $@

# the plugin itself, built for this machine (the bundle Makefile cross-builds and wants armadillo)
mod-pitcharpeggiator.so: ../src/PitchArpeggiator.cpp ../src/Sequence.h ../src/Engine.cpp $(ENGINE)
	$(CXX) $(CXXFLAGS) -fPIC -shared ../src/PitchArpeggiator.cpp ../src/Engine.cpp ../../Shared_files/Wisdom.cpp $(LDLIBS) -o $@

arp_test: arp_test.cpp ../src/Sequence.h mod-pitcharpeggiator.so
	$(CXX) $(CXXFLAGS) arp_test.cpp $(LDLIBS) -ldl -o $@

run: all
	./interval_test
	./arp_test ./mod-pitcharpeggiator.so ../ttl/

clean:
	rm -f interval_test arp_test mod-pitcharpeggiator.so *.wav

.PHONY: all run clean
