#ifndef METEO_FORECAST_PROCESSOR_H
#define METEO_FORECAST_PROCESSOR_H

#include <string>

#include "temperature.h"
#include "wind.h"
#include "meteo_forecast.h"

struct Meteo_offset
{
    Temperature temp;
    Wind wind;
    int offset;
};

struct Meteo_proc_params
{
    std::string file_path;
    float latitude;
    float longitude;
};

class Meteo_forecast_processor
{
 private:
    float latitude = 0.0;
    float longitude = 0.0;

 public:
    Meteo_forecast_processor();
    Meteo_forecast run(const std::string &location_name,
                       const std::string &country,
                       const float &latitude,
                       const float &longitude,
                       const std::vector<std::string> &arome_files,
                       const std::vector<std::pair<std::string, std::string>> &urls);
};

#endif // METEO_FORECAST_PROCESSOR_H
