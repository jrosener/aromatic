#include <iomanip>
#include <sstream>
#include <iostream>

#include "meteo_forecast.h"

Meteo_forecast::Meteo_forecast(const Location &loc) :
                                   location(loc)
{

}

void Meteo_forecast::set_start_date(const std::string &date, const std::string &format)
{
    // Convert date and time from string to structured C++ std type.
    std::tm tm = {};
    std::stringstream ss(date.c_str());
    ss >> std::get_time(&tm, format.c_str());

    // Store it as time_t type.
    this->start_date = std::mktime(&tm);
}

void Meteo_forecast::set_start_date(const std::time_t &date)
{
    this->start_date = date;
}

std::time_t Meteo_forecast::get_start_date()
{
    return this->start_date;
}

std::string Meteo_forecast::get_start_date_as_str(const std::string &format)
{
    // Convert time_t start date to a structured C++ std type.
    std::tm tm = *std::localtime(&this->start_date);

    // Convert the structured type to a string based on the specified format.
    std::stringstream ss;
    ss << std::put_time(&tm, format.c_str());

    return ss.str();
}

// Return the start date incremented by an hour offset.
std::string Meteo_forecast::get_start_date_offset_as_str(const unsigned int &hour_offset,
                                                         const std::string &format)
{
    // Convert time_t start date to a structured C++ std type (add the specified hours).
    std::tm tm = *std::localtime(&this->start_date);
    tm.tm_hour += hour_offset;
    time_t t_offset = mktime(&tm);
    std::tm tm_offset = *std::localtime(&t_offset);

    // Convert the structured type to a string based on the specified format.
    std::stringstream ss;
    ss << std::put_time(&tm_offset, format.c_str());

    return ss.str();
}

void Meteo_forecast::ensure_forecast_offset_exists(const unsigned int &hour_offset)
{
    // Create a current meteo for the specified offset hour if it does not exists.
    while (this->forecast.size() <= hour_offset)
    {
        Meteo_current mto;
        this->forecast.push_back(mto);
    }

    return;
}

void Meteo_forecast::set_temperature(const Temperature &temp, const unsigned int &hour_offset)
{
    // Put the temperature at the specified position in the forecast.
    this->ensure_forecast_offset_exists(hour_offset);
    this->forecast[hour_offset].temp = temp;
}

Temperature Meteo_forecast::get_temperature(const unsigned int &hour_offset)
{
    Temperature temp;

    // Get the temperature from the forecast list.
    if (this->forecast.size() > hour_offset)
    {
        temp = this->forecast[hour_offset].temp;
    }

    return temp;
}

void Meteo_forecast::set_wind(const Wind &wind, const unsigned int &hour_offset)
{
    // Put the wind at the specified position in the forecast.
    this->ensure_forecast_offset_exists(hour_offset);
    this->forecast[hour_offset].wind = wind;
}

Wind Meteo_forecast::get_wind(const unsigned int &hour_offset)
{
    Wind wind;

    // Get the wind from the forecast list.
    if (this->forecast.size() > hour_offset)
    {
        wind = this->forecast[hour_offset].wind;
    }

    return wind;
}

unsigned int Meteo_forecast::get_max_offset()
{
    return this->forecast.size();
}

