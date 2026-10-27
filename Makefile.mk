# Build rules shared by the plugin directories (each one's Makefile includes this file).
# The plugin's bundle is mod-<directory name in lower case>.lv2.
# Honoured from the environment or the command line: CXX, CXXFLAGS, LDFLAGS, DESTDIR,
# INSTALL_PATH (default /usr/local/lib/lv2), NOOPT=true (no x86 tuning flags), MACOS=true.

CXX ?= g++
INSTALL_PATH ?= /usr/local/lib/lv2

NAME   := mod-$(shell echo $(notdir $(CURDIR)) | tr A-Z a-z)
LIB    := $(NAME).so
BUNDLE := $(DESTDIR)$(INSTALL_PATH)/$(NAME).lv2

SOURCES := $(wildcard src/*.cpp) $(wildcard ../Shared_files/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

CXXFLAGS += -O3 -ffast-math -Wall -fPIC -DPIC $(shell pkg-config --cflags fftw3f) -I. -I../Shared_files -std=gnu++11
LDFLAGS  += -shared $(shell pkg-config --libs fftw3f) -larmadillo -lm

ifneq ($(MACOS),true)
LDFLAGS  += -Wl,-O1,--as-needed,--no-undefined,--strip-all
endif

ifneq ($(NOOPT),true)
CXXFLAGS += -mtune=generic -msse -msse2 -mfpmath=sse
endif

.PHONY: all install clean

all: $(LIB)

$(LIB): $(OBJECTS)
	$(CXX) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@

# ttl/ holds a link to ../../Shared_files/harmonizer.wisdom: copy what it points to
install: all
	mkdir -p $(BUNDLE)
	cp -rL $(LIB) ttl/* $(BUNDLE)

clean:
	rm -f *.so src/*.o
