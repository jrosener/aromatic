#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>

#include "arome_grib_parser.h"
#include "utils.h"

Arome_grib_parser::Arome_grib_parser(const std::string &fpath) : file_path(fpath)
{
}

//
// Get meteo value from an AROME GRIB file at lat/long.
// Return an average value of the extracted data table.
//
float Arome_grib_parser::get_meteo_value(const float &latitude, const float &longitude,
                                         const std::string &param_short_name)
{
    // Extract meteo values.
    std::string grib_get_params = std::to_string(latitude) + "," + std::to_string(longitude) + ",1"
            + " -w shortName=" + param_short_name
            + " " + this->file_path;
    std::string raw_val_str = exec("grib_get -l " + grib_get_params);
    if (raw_val_str.empty() == true)
    {
        std::cout << "ERROR: grib_get -l " << grib_get_params << std::endl;
        return 0.0;
    }

    // Split the results (most of the time there are 4 space-separated values).
    std::vector<float> values;
    std::stringstream ss(raw_val_str);
    std::string item;
    while (std::getline(ss, item, ' '))
    {
        values.push_back(atof(item.c_str()));
    }

#if 0
    // Calculate the average value.
    float sum = 0.0;
    for (float &v : values) sum += v;
    float val = sum / (float)values.size();
#else
    // Or get the first value only.
    float val = values[0];
#endif

    return val;
}

std::time_t Arome_grib_parser::get_start_date()
{
    // Extract start date and time from meteo params.
    // Get only the first occurence.
    std::string date = exec("grib_get -p dataDate " + this->file_path + " | head -n1");
    std::string time = exec("grib_get -p dataTime " + this->file_path + " | head -n1");
    if (time.length() == 3)
    {
        time = "0" + time; // Prepend leading "0" if necessary.
    }
    std::string date_time = date + time;

    // Convert date and time from string to structured C++ std type.
    std::tm tm = {};
    std::stringstream ss(date_time.c_str());
    ss >> std::get_time(&tm, "%Y%m%d%H%M");

    // Return as time_t type.
    return std::mktime(&tm);
}

int Arome_grib_parser::get_start_date_offset()
{
    // Extract the step from meteo params.
    // Get only the first occurence.
    std::string offset = exec("grib_get -p step " + this->file_path + " | head -n1");

    if (is_number(offset))
        return std::stoi(offset);
    else
        return -1;
}

Wind Arome_grib_parser::get_wind(const float &latitude, const float &longitude)
{
    Wind w;
    w.set_components_u_v(this->get_meteo_value(latitude, longitude, "10u"),
                         this->get_meteo_value(latitude, longitude, "10v"));

    return w;
}

Temperature Arome_grib_parser::get_temperature(const float &latitude, const float &longitude)
{
    Temperature t;
    t.set(this->get_meteo_value(latitude, longitude, "2t"));

    return t;
}
