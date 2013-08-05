
# installation path
INSTALL_PATH = /usr/local/lib/lv2/mod-whammy.lv2

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -O2 -ffast-math -msse -msse3 -mfpmath=sse -Wall -Wextra -c -fPIC -DPIC
LDFLAGS = -shared -lc -lm -fPIC -DPIC -nostartfiles

# libs
LIBS = -lfftw3 -lm

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = mod-whammy.so

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(INSTALL_PATH)
	cp $(PLUGIN) $(INSTALL_PATH)
	cp ttl/*.ttl $(INSTALL_PATH)
