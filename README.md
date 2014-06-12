mod-pitchshifter
================

Pitch Shifter effects developed by mod team

Installation:

	Dependencies:
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
