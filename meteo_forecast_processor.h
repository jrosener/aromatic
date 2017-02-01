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
 public:
    Meteo_forecast_processor();
    void run(Meteo_forecast &fcast,
             const std::vector<std::string> &arome_files);
};

#endif // METEO_FORECAST_PROCESSOR_H
