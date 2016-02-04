# Aromatic
A forecast parser for Meteo France AROME files.
http://jrosener.github.io/aromatic/

# Features
* Download the latest run of AROME 0.01 meteo files.
* Parse them and extract wind speed/direction and temperature at a specific location.
* 42 hours forecast for several locations exported as html file or txt format.

# Known issues
* Sometimes Meteo France servers are not providing AROME files properly: the name of the retrieved file is wrong (and the content as well). For the moment the only solution is to run again `aromatic` and hope that it will get the file ;-)
* The whole software was written in a couple of hours without quality in mind. As a consequence, there are no tests and most of the returned errors are not checked. It obviously needs to be improved.

# Runtime dependencies
2 command line tools are required:
* `curl`: needed to download AROME files from Meteo France servers.
* `grib_get`: needed to parse AROME files and extract meteo data at a specific location (this tool is part of the GRIB API distribution: https://software.ecmwf.int/wiki/display/GRIB/Home).

# Build requirements
* `g++ 5.x`
* `Qt 5.x` (`qmake` and `QtConcurrent` are necessary)

# Build instructions
* Install build dependencies (ex. for Ubuntu 15.10): `sudo apt-get install build-essential qt5-default libgrib-api-tools`
* Generate makefile: `qmake aromatic.pro`
* Build: `make`
* Run (the forecast is generated for locations defined in `main.cpp`, so this is the place to add your favorite spots): `./aromatic`

# General architecture
```
                                               +--------+
                                               |  main  |
                                               +---+----+
                                                   |
                 +----------------------------------------------------------------------+
                 |                                 |                                    |
                 v                                 v                                    v
+------------------------------+ +-----------------------------------+ +--------------------------------+
|  Arome_grib_downloader       | |      Meteo_forecast_processor     | |      Meteo_forecast_printer    |
+------------------------------+ +-----------------------------------+ +--------------------------------+
|                              | |                                   | |                                |
| bool run()                   | | Meteo_forecast run(location_name, | | list<Meteo_forecast> forecasts |
| list<string> get_file_list() | |                    country,       | |                                |
|                              | |                    latitude,      | +--------------------------------+
+------------------------------+ |                    longitude,     | |                                |
                                 |                    arome_files)   | | string get_html()              |
                                 |                                   | | string get_txt()               |
                                 +-----------------+-----------------+ |                                |
                                                   |                   +----------------+---------------+
                                                   |                                    |
                                      +------------+-----------------------+            |
                                      |                                    |            |
                                      v                                    v            v
                +-------------------------------------------+  +-----------------------------------------+
                |              Arome_grib_parser            |  |               Meteo_forecast            |
                +-------------------------------------------+  +-----------------------------------------+
                |                                           |  |                                         |
                | file_path                                 |  | latitude                                |
                |                                           |  | longitude                               |
                +-------------------------------------------+  | location_name                           |
                |                                           |  | country                                 |
                | date get_start_date()                     |  | start_date                              |
                | Wind get_wind(latitude, longitude)        |  | list<Wind,Temperature> forecast         |
                | Temperature get_temp(latitude, longitude) |  |                                         |
                |                                           |  +-----------------------------------------+
                +---------------------+---------------------+  |                                         |
                                      |                        | void set_wind(wind, hour_offset)        |
                                      |                        | void set_temp(temperature, hour_offset) |
                                      |                        | Wind get_wind(hour_offset)              |
                                      |                        | Temperature get_temp(hour_offset)       |
                                      |                        |                                         |
                                      |                        +---------------------+-------------------+
                                      |                                              |
                                      +----------------------------------------------+
                                      |                                              |
                                      v                                              v
                     +-------------------------------+                       +---------------+
                     |               Wind            |                       |  Temperature  |
                     +-------------------------------+                       +---------------+
                     |                               |                       |               |
                     | speed                         |                       | value         |
                     | direction                     |                       |               |
                     |                               |                       +---------------+
                     +-------------------------------+                             
                     |                               |                             
                     | void set_components_u_v(u, v) |                             
                     |                               |
                     +-------------------------------+
```
