#ifndef METEO_FORECAST_H
#define METEO_FORECAST_H

#include <ctime>
#include <string>
#include <vector>

#include "temperature.h"
#include "wind.h"

struct Meteo_current
{
    Temperature temp;
    Wind wind;
};

class Meteo_forecast
{
 private:
    float latitude = 0.0;
    float longitude = 0.0;
    std::string location_name;
    std::string country;
    std::vector<std::pair<std::string, std::string>> urls;
    std::time_t start_date;
    std::vector<Meteo_current> forecast;

 public:
    Meteo_forecast(const std::string &location_name,
                   const std::string &country,
                   const std::vector<std::pair<std::string,std::string>> &urls);

    void set_latitude(const float &latitude);
    void set_longitude(const float &latitude);
    float get_latitude();
    float get_longitude();
    std::string get_location_name();
    std::string get_country();
    std::vector<std::pair<std::string,std::string>> get_urls();

    void set_start_date(const std::string &date, const std::string &format);
    void set_start_date(const std::time_t &date);
    std::time_t get_start_date();
    std::string get_start_date_as_str(const std::string &format);
    std::string get_start_date_offset_as_str(const unsigned int &hour_offset,
                                             const std::string &format); // Return the start date incremented by an hour offset.

    void set_temperature(const Temperature &temp, const int unsigned &hour_offset);
    Temperature get_temperature(const unsigned int &hour_offset);

    void set_wind(const Wind &wind, const int unsigned &hour_offset);
    Wind get_wind(const unsigned int &hour_offset);
    unsigned int get_max_offset();

 private:
    void ensure_forecast_offset_exists(const unsigned int &hour_offset);
};

#endif // METEO_FORECAST_H
