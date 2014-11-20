cd 2Voices
make install
cd ..
cd Capo
make install
cd ..
cd Drop
make install
cd ..
cd Harmonizer
make install
cd ..
cd Harmonizer2
make install
cd ..
cd HarmonizerCS
make install
cd ..
cd SuperCapo
make install
cd ..
cd SuperWhammy
make install
cd ..
mkdir -p /etc/fftw/wisdom
echo "Creating fftw wisdom file"
cd /etc/fftw/wisdom
fftwf-wisdom -n -x -o my.wisdom rof1024 rob1024 rof1536 rob1536 rof2048 rob2048 rof2176 rob2176 rof2304 rob2304 rof2432 rob2432 rof2560 rob2560 rof3072 rob3072 rof4096 rob4096