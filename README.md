mod-pitchshifter
================

Pitch Shifter lv2 effects

These effects ONLY work with the following buffer sizes: 64, 128, 256 and 512.


You need a PC with at least 2Gb of ram to compile the source code.

Dependencies:

	- Armadillo
	- FFTW (configured with --enable-float)
	- python-mpmath

On a Debian or Ubuntu system you can use this command to install all dependencies:

	sudo apt-get install libarmadillo-dev libfftw3-dev python3-mpmath

Installation:

	make
	sudo make install

(Be patient, may take more than 20 minutes to compile)


The default instalation path is /usr/local/lib/lv2/, and this can be modified passing the variable INSTALL_PATH to make install, e.g.:

	sudo make install INSTALL_PATH=/usr/lib/lv2
