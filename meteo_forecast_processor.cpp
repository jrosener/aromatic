#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <QtConcurrentMap>

#include "meteo_forecast_processor.h"
#include "arome_grib_parser.h"

Meteo_forecast_processor::Meteo_forecast_processor()
{
}

static Meteo_offset parse_arome_file_static(const Meteo_proc_params &params)
{
    Meteo_offset mto_offset;

    Arome_grib_parser parser(params.file_path);
    mto_offset.wind   = parser.get_wind(params.latitude, params.longitude);
    mto_offset.temp   = parser.get_temperature(params.latitude, params.longitude);
    mto_offset.offset = parser.get_start_date_offset();
    std::cout << "Parsing " << params.file_path << "... Done" << std::endl;

    return mto_offset;
}

Meteo_forecast Meteo_forecast_processor::run(const std::string &location_name,
                                             const std::string &country,
                                             const float &latitude,
                                             const float &longitude,
                                             const std::vector<std::string> &arome_files,
                                             const std::vector<std::pair<std::string,std::string>> &urls)
{
    std::cout << std::endl;
    std::cout << "Getting forecast @ " << location_name << ":" << std::endl;

    // Prepare a forecast object.
    Meteo_forecast mto_forecast(location_name, country, urls);
    mto_forecast.set_latitude(latitude);
    mto_forecast.set_longitude(longitude);
    this->latitude = latitude;
    this->longitude = longitude;

    // Extract the start time from the first AROME file.
    std::cout << "Getting start date from " << arome_files[0] << "..." << std::flush;
    Arome_grib_parser grib(arome_files[0]);
    mto_forecast.set_start_date(grib.get_start_date());
    std::cout << " Done (" << mto_forecast.get_start_date_as_str("%a %d %b - %H:%M") << ")" << std::endl;

    // Prepare a list of files+geolocation (needed by parallel proc above).
    std::vector<Meteo_proc_params> filepaths_params;
    for (auto &f : arome_files)
    {
        Meteo_proc_params p;
        p.latitude = latitude;
        p.longitude = longitude;
        p.file_path = f;
        filepaths_params.push_back(p);
    }

    // Iterate over AROME files, extract meteo data and set results in a vector.
#if 1 // parallel
    std::vector<Meteo_offset> parser_results = QtConcurrent::blockingMapped<std::vector<Meteo_offset>>(filepaths_params, parse_arome_file_static);
#else // sequential
    std::vector<Meteo_offset> parser_results;
    for (auto &params : filepaths_params)
    {
        parser_results.push_back(parse_arome_file_static(params));
    }
#endif

    // Iterate over results and populate the forecast object.
    for (auto &parser_result : parser_results)
    {
        mto_forecast.set_wind(parser_result.wind, parser_result.offset);
        mto_forecast.set_temperature(parser_result.temp, parser_result.offset);
    }

    return mto_forecast;
}
