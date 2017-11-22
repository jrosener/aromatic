#include "location.h"

Location::Location(const std::string &loc_name,
                                     const float &lat,
                                     const float &longit,
                                     const std::string &map,
                                     const std::string &season,
                                     const std::vector<std::pair<std::string, std::string>> &urls) :
                                        location_name(loc_name),
                                        latitude(lat),
                                        longitude(longit),
                                        map_link(map),
                                        season_type(season),
                                        urls(urls)
{
}

float Location::get_latitude()
{
    return this->latitude;
}

float Location::get_longitude()
{
    return this->longitude;
}

std::string Location::get_location_name()
{
    return this->location_name;
}

std::string Location::get_map()
{
    return this->map_link;
}

std::string Location::get_season_type()
{
    return this->season_type;
}


std::vector<std::pair<std::string,std::string>> Location::get_urls()
{
    return this->urls;
}
