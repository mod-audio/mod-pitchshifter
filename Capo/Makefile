
# installation path
INSTALL_PATH = /usr/local/lib/lv2/mod-capo.lv2

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -O3 -ffast-math -Wall -Wextra -march=native -mtune=native -c -fPIC -DPIC
LDFLAGS = -shared -lc -lm -fPIC -DPIC

# libs
LIBS = -lfftw3 -lm -larmadillo

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = mod-capo.so

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
