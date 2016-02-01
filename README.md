# Aromatic
A forecast parser for Meteo France AROME files.

# Features
* Download the latest run of AROME 0.01 meteo files.
* Parse them and extract wind speed/direction and temperature at a specific location.
* 42 hours forecast for several locations exported as html file or txt format.

# Runtime dependencies
2 command line tools are required:
* curl: needed to download AROME files from Meteo France servers.
* grib_get: needed to parse AROME files and extract meteo data at a specific location (this tool is part of the GRIB API distribution: https://software.ecmwf.int/wiki/display/GRIB/Home).

# Build requirements
* g++ 5.x
* Qt 5.x (qmake and QtConcurrent are necessary)

# Build instructions
* Install build dependencies (ex. for Ubuntu 15.10): sudo apt-get install build-essential qt5-default libgrib-api-tools
* Generate makefile: qmake aromatic.pro
* Build: make
* Run (the forecast is generated for locations defined in main.cpp): ./aromatic

