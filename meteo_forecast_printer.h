#ifndef METEO_FORECAST_PRINTER_H
#define METEO_FORECAST_PRINTER_H

#include <string>

#include "meteo_forecast.h"

class Meteo_forecast_printer
{
 private:
    std::vector<Meteo_forecast> forecasts;

 public:
    explicit Meteo_forecast_printer(const std::vector<Meteo_forecast> &fcasts);
    std::string get_html();
    std::string get_txt();
};

#endif // METEO_FORECAST_H
