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
bool Arome_grib_parser::get_meteo_value(const float &latitude, const float &longitude,
                                        const std::string &param_short_name,
                                        float &out_value)
{
    // Extract meteo values.
    std::string grib_get_params = " -l " + std::to_string(latitude) + "," + std::to_string(longitude) + ",1"
            + " -w shortName=" + param_short_name
            + " " + this->file_path;
    std::string raw_val_str = exec("grib_get" + grib_get_params);
    if ((raw_val_str.empty() == true) ||
        (raw_val_str.find("ERROR") != std::string::npos) ||
        (raw_val_str.find("unreadable message") != std::string::npos))
    {
        std::cout << "ERROR: grib_get" << grib_get_params << std::endl;
        std::cout << "      RET = " << raw_val_str << std::endl;
        out_value = 0.0;
        return false;
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
    out_value = sum / (float)values.size();
#else
    // Or get the first value only.
    out_value = values[0];
#endif

    return true;
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
    std::string cmd = "grib_get -p step " + this->file_path + " | head -n1";
    std::string offset = exec(cmd);

    if ((offset.find("ERROR") != std::string::npos) ||
        (offset.find("unreadable message") != std::string::npos))
    {
        std::cout << "ERROR: " << cmd << std::endl;
        std::cout << "      RET = " << offset << std::endl;
        return -1;
    }
    else
    {
        return std::stoi(offset);
    }
}

Wind Arome_grib_parser::get_wind(const float &latitude, const float &longitude)
{
    Wind w;
    float u;
    float v;
    
    if ((this->get_meteo_value(latitude, longitude, "10u", u) == true) &&
        (this->get_meteo_value(latitude, longitude, "10v", v) == true))
    {
        w.set_components_u_v(u, v);
    }
    else
    {
        w.set_defined(false);
    }
    
    return w;
}

Temperature Arome_grib_parser::get_temperature(const float &latitude, const float &longitude)
{
    Temperature t;
    float t_val;
    
    if (this->get_meteo_value(latitude, longitude, "2t", t_val) == true)
    {
        t.set(t_val);
    }
    else
    {
        t.set_defined(false);
    }

    return t;
}
