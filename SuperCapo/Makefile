
# installation path
INSTALL_PATH = /usr/local/lib/lv2/

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS += -I. -I../Shared_files -O3 -ffast-math -msse -msse3 -mfpmath=sse -Wall -Wextra -march=native -mtune=native -c -fPIC -DPIC `pkg-config --cflags fftw3f`
LDFLAGS += -shared -lc -lm -fPIC -DPIC

# libs
LIBS = `pkg-config --libs fftw3f` -lm -larmadillo

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp) $(wildcard ../Shared_files/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = mod-supercapo.so

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

gentable:
	cd ./../Shared_files && ./AtanTable.py
	cd ./../Shared_files && ./CxExpTable.py

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(INSTALL_PATH)/mod-supercapo.lv2/
	cp $(PLUGIN) $(INSTALL_PATH)/mod-supercapo.lv2/
	cp ttl/*.ttl $(INSTALL_PATH)/mod-supercapo.lv2/
	test -d ttl/modgui && cp -r ttl/modgui/ $(INSTALL_PATH)/mod-supercapo.lv2/ || true
