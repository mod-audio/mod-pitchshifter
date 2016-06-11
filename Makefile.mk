
# compiler
CXX ?= g++

# flags
CXXFLAGS += -O3 -ffast-math -Wall -fPIC -DPIC -I. -I../Shared_files
LDFLAGS += -shared -Wl,-O1 -Wl,--as-needed -Wl,--no-undefined -Wl,--strip-all -larmadillo -lNE10 -lm

ifneq ($(NOOPT),true)
CXXFLAGS += -mtune=generic -msse -msse2 -mfpmath=sse
endif

# remove command
RM = rm -f

# plugin name
PLUGIN = mod-$(shell basename $(shell pwd) | tr A-Z a-z)
PLUGIN_SO = $(PLUGIN).so

# effect path
EFFECT_PATH = $(PLUGIN).lv2

# installation path
ifndef INSTALL_PATH
INSTALL_PATH = /usr/local/lib/lv2
endif
INSTALLATION_PATH = $(DESTDIR)$(INSTALL_PATH)/$(EFFECT_PATH)

# sources and objects
SRC = $(wildcard src/*.cpp) $(wildcard ../Shared_files/*.cpp)
OBJ = $(SRC:.cpp=.o)

## rules
all: $(PLUGIN_SO)

$(PLUGIN_SO): $(OBJ)
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	$(RM) *.so src/*.o

install: all
	mkdir -p $(INSTALLATION_PATH)
	cp -rL $(PLUGIN_SO) ttl/* $(INSTALLATION_PATH)

%.o: %.cpp
	$(CXX) $< $(CXXFLAGS) -c -o $@
