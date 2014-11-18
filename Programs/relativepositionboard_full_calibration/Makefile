#
# (c) 2006 - 2008 EPFL, Lausanne, Switzerland
# Sven Gowal
#

# List of standard modules that this program needs
MODULES	:= relativepositionboard windsensorboard i2c_stream i2cal nmea khepera4

# List of modules that you created on your own (and path to your modules)
#MY_MODULES	:= ../../Khepera4Modules/relativepositionboard ../../../WindSensorBoard/Khepera4Modules/windsensorboard

# The file name of the executable
TARGET	:= relativepositionboard_full_calibration

# If the K4_ROOT environment variable is set, you can comment out the following line
LIBS_ADD := -lpthread

CFLAGS_ADD := -O3 -fno-strict-aliasing

# Include the Makefile
include ./Makefile.include
