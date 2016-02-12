#ifndef METEO_FORECAST_PRINTER_H
#define METEO_FORECAST_PRINTER_H

#include <string>

#include "meteo_forecast.h"

class Meteo_forecast_printer
{
 private:
    std::string run_date;
    std::vector<Meteo_forecast> forecasts;
    std::vector<std::pair<std::string, std::string>> txt_sections; // Text written before forecat tables.
                                                                   // It is a pair of ["The text", url/nothing].
 public:
    Meteo_forecast_printer(const std::string run_date,
                           const std::vector<Meteo_forecast> &fcasts,
                           const std::vector<std::pair<std::string, std::string>> &txt_sections);
    std::string get_html();
    std::string get_txt();
};

#endif // METEO_FORECAST_H
