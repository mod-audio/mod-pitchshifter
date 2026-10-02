# Offline measurements, run on the build machine: `make -C PolyOctaver/test run`.
# Needs fftw3f and, for the baseline only, the armadillo headers.

CXX ?= g++
CXXFLAGS += -O2 -std=gnu++11 -Wall -I. -I../src -I../../Shared_files $(shell pkg-config --cflags fftw3f)
LDLIBS += $(shell pkg-config --libs fftw3f) -lm

all: engine_test baseline_test plugin_smoke swell_test

engine_test: engine_test.cpp measure.h ../src/OctaveEngine.cpp ../src/OctaveEngine.h
	$(CXX) $(CXXFLAGS) engine_test.cpp ../src/OctaveEngine.cpp ../../Shared_files/Wisdom.cpp $(LDLIBS) -o $@

baseline_test: baseline_test.cpp measure.h
	$(CXX) $(CXXFLAGS) -DARMA_DONT_USE_WRAPPER -Wno-unused-variable baseline_test.cpp \
	../../Shared_files/PitchShifterClasses.cpp ../../Shared_files/Wisdom.cpp ../../Shared_files/angle.cpp \
	../../Shared_files/Exp.cpp ../../Shared_files/window.cpp $(LDLIBS) -o $@

swell_test: swell_test.cpp measure.h ../src/Swell.h
	$(CXX) $(CXXFLAGS) swell_test.cpp $(LDLIBS) -o $@

# the plugin itself, built for this machine (the bundle Makefile cross-builds and wants armadillo)
mod-polyoctaver.so: ../src/PolyOctaver.cpp ../src/OctaveEngine.cpp ../src/OctaveEngine.h ../src/Swell.h
	$(CXX) $(CXXFLAGS) -fPIC -shared ../src/PolyOctaver.cpp ../src/OctaveEngine.cpp ../../Shared_files/Wisdom.cpp $(LDLIBS) -o $@

plugin_smoke: plugin_smoke.cpp measure.h mod-polyoctaver.so
	$(CXX) $(CXXFLAGS) plugin_smoke.cpp $(LDLIBS) -ldl -o $@

run: all
	./baseline_test
	./engine_test
	./swell_test
	./plugin_smoke ./mod-polyoctaver.so ../ttl/

clean:
	rm -f engine_test baseline_test plugin_smoke swell_test mod-polyoctaver.so

.PHONY: all run clean
