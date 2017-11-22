#ifndef AROME_GRIB_PARSER_H
#define AROME_GRIB_PARSER_H

#include <string>
#include <ctime>

#include "temperature.h"
#include "wind.h"

class Arome_grib_parser
{
 private:
    std::string file_path;

 public:
    explicit Arome_grib_parser(const std::string &fpath);

    std::time_t get_start_date();
    int get_start_date_offset();
    Wind get_wind(const float &latitude, const float &longitude);
    Temperature get_temperature(const float &latitude, const float &longitude);

 private:
    bool get_meteo_value(const float &latitude, const float &longitude,
                         const std::string &param_short_name, float &out_value);
};

#endif // AROME_GRIB_PARSER_H
