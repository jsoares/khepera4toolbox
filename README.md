# Khepera IV Toolbox

## Welcome to the Khepera IV Toolbox!

This is a ported version of the Khepera III Toolbox, developed over the years by multiple contributors (listed below). It has been adapted to work with the Khepera IV mobile robot from K-Team:

	https://www.k-team.com/mobile-robotics-products/khepera-iv

More information about this toolbox and the Khepera IV can be found in our publication:

	https://infoscience.epfl.ch/record/211875/files/k4_paper.pdf	

This README file contains only a few pointers and quick start information. The documentation of the Khepera III toolbox is available online as wikibook, and should mostly apply to the current library:

	http://en.wikibooks.org/wiki/Khepera_III_Toolbox
	
The documentation also includes installation guidelines, a short introduction for new users and a lot of examples.

##  Quick start

1. Set your environment variables:
	export K4_ROOT=/path/to/the/khepera4toolbox
	export PATH=$PATH:$K4_ROOT/Scripts

2. Install the light toolchain from K-Team. 

For the original Gumstix, you'll find it at

	http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM/light_tools/khepera4-oetools-light-kb1.0.tar.bz2
	
The manual with the straightforward installation instructions (5.2.1) is available at:

	http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM/UserManual/Khepera%20IV%20User%20Manual.pdf

Note: The library hasn't been tested with more recent K4 versions carrying other Gumstix devices. You may, nevertheless, try adapting the toolchain in the Makefile.include.

3. Configure your network parameters in both Setup/setup_kh4.py and Scripts/k4_commons

4. Connect each robot using the USB cable and run the setup script (Setup/setup_kh4.py)

5. Compile the modules and programs

6. Try uploading a single program (e.g. k4put +<robot id> battery)

7. Enjoy working with the Khepera IV robot!

## Credits

Khepera III Toolbox developed by 
- Thomas Lochmatter (Main developer)
- Iñaki Navarro
- Jim Pugh
- Gilles Caprari (K-Team)
- Frederic Lambercy (K-Team)
- Sven Gowal
- Michael Karlen
- Bob Andries
- Guillermo Fernandez Rodriguez
- Pierre-François Laquerre
- Jorge Soares

Khepera IV Toolbox ported by
- Jorge Soares
