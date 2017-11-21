#ifndef LOCATION_INI_PARSER_H
#define LOCATION_INI_PARSER_H

#include <string>
#include <vector>
#include "location.h"

class Location_ini_parser
{
 private:
    std::string file;

 public:
    Location_ini_parser(const std::string &ini_file);

    void run(std::vector<Location> &loc_forecasts);
};

#endif // LOCATION_INI_PARSER_H
