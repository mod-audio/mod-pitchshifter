
# installation path
INSTALL_PATH = /usr/local/lib/lv2/

# compiler and linker
CC = g++
LD = g++

# flags
CFLAGS += -I. -I../Shared_files -O3 -ffast-math -Wall -Wextra -c -fPIC -DPIC `pkg-config --cflags fftw3f`
LDFLAGS += -shared -lc -lm -fPIC -DPIC

# libs
LIBS = `pkg-config --libs fftw3f` -lm -larmadillo

# remove command
RM = rm -f

# sources and objects
SRC = $(wildcard src/*.cpp) $(wildcard ../Shared_files/*.cpp)
OBJ = $(SRC:.cpp=.o)

# plugin name
PLUGIN = mod-2voices.so

$(PLUGIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PLUGIN) $(LIBS)

gentable:
	cd ./../Shared_files && ./AtanTable.py
	cd ./../Shared_files && ./SinTable.py
	cd ./../Shared_files && ./CosTable.py

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) src/*.o $(PLUGIN)

install:
	mkdir -p $(INSTALL_PATH)/mod-2voices.lv2/
	cp -r ttl/* $(INSTALL_PATH)/mod-2voices.lv2/
	cp $(PLUGIN) $(INSTALL_PATH)/mod-2voices.lv2/
