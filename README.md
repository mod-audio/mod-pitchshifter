mod-pitchshifter
================

Pitch Shifter lv2 effects developed by mod team

These effects are supposed to work well with the
following values of Frames/Period: 64, 128, 256, 512

It's advisable that you use a RT kernel configured for audio.
It will allow you to use lower values of Frames/Period (-> less latency) without x-runs.

You need a PC with at least 2Gb of ram to compile the source code.

Installation:

	Dependencies:
	-python-mpmath
	-FFTW (configure fftw with the flags --enable-shared and --enable-float)
	-Armadillo

	./make.sh
	sudo ./make_install.sh
	
(Be pacient, may take more than 20 minutes to compile)
	
You also can install only the plugins you want.
To do this, enter the folder of the effect that you want and type the following comands:
	
	make gentable (only on the first plugin that you compile)
	make
	sudo make install

In this way, the default instalation path is /usr/local/lib/lv2/, and this can be modified passing the variable INSTALL_PATH to make install, e.g.:

	sudo make install INSTALL_PATH=/usr/lib/lv2
