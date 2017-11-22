#ifndef LOCATION_FORECAST_H
#define LOCATION_FORECAST_H

#include <string>
#include <vector>

class Location
{
 private:
    std::string location_name;
    float latitude;
    float longitude;
    std::string map_link;
    std::string season_type;
    std::vector<std::pair<std::string, std::string>> urls;

 public:
    Location(const std::string &loc_name,
             const float &lat,
             const float &longit,
             const std::string &map,
             const std::string &season,
             const std::vector<std::pair<std::string, std::string>> &urls);

    float get_latitude();
    float get_longitude();
    std::string get_location_name();
    std::string get_map();
    std::string get_season_type();
    std::vector<std::pair<std::string,std::string>> get_urls();
};

#endif // LOCATION_FORECAST_H
