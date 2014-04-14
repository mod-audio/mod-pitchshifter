
# installation path
INSTALL_PATH = /usr/local/lib/lv2/

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS = -I. -O3 -ffast-math -msse -msse3 -mfpmath=sse -Wall -Wextra -march=native -mtune=native -c -fPIC -DPIC
LDFLAGS = -shared -lc -lm -fPIC -DPIC

# libs
LIBS = -lfftw3 -lm -larmadillo

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = mod-superwhammy.so

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(INSTALL_PATH)/mod-superwhammy28.lv2/
	cp $(PLUGIN) $(INSTALL_PATH)/mod-superwhammy28.lv2/
	cp ttl/*.ttl $(INSTALL_PATH)/mod-superwhammy28.lv2/
	test -d ttl/modgui && cp -r ttl/modgui/ $(INSTALL_PATH)/mod-superwhammy28.lv2/ || true
