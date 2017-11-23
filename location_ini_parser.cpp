#include <iostream>
#include <sstream>
#include <iomanip>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include "location_ini_parser.h"

Location_ini_parser::Location_ini_parser(const std::string &ini_file)
{
    // Use aromatic-default.ini if no INI file specified.
    if (ini_file == "")
    {
        std::cout << "INFO: no locations file specified, use default locations." << std::endl;
        this->file = ":/aromatic-default.ini";
        return;
    }

    // Use aromatic-default.ini if specified INI file does not exists.
    QFileInfo locations_file(QString::fromStdString(ini_file));
    if (locations_file.exists() == false)
    {
        std::cout << "INFO: " << ini_file << " does not exists, use default locations." << std::endl;
        this->file = ":/aromatic-default.ini";
        return;
    }


    // Specified INI file exists, use it.
    std::cout << "INFO: using locations file " << ini_file << std::endl;
    this->file = ini_file;

    return;
}

void Location_ini_parser::run(std::vector<Location> &loc_forecasts)
{
    // Get list of locations where the forecast has to be computed (aromatic.ini).
    QSettings locations_ini(QString::fromStdString(this->file), QSettings::IniFormat);
    locations_ini.setIniCodec("UTF-8");
    QStringList location_sections = locations_ini.childGroups();

    // Iterate over sections "[location.x]".
    for (unsigned short int i = 0; i < location_sections.size(); i++)
    {
        // Select subsection [location.x].
        locations_ini.beginGroup(location_sections.at(i));

        // Get name, lat, long,... for this location.
        std::string name = locations_ini.value("full_name").toString().toStdString();
        std::string map = locations_ini.value("map").toString().toStdString();
        float latitude = locations_ini.value("latitude").toFloat();
        float longitude = locations_ini.value("longitude").toFloat();

        // Get web links for this location.
        QStringList location_subkeys = locations_ini.allKeys();
        std::vector<std::pair<std::string,std::string>> web_urls;
        for (unsigned short int j = 0; j < location_subkeys.size(); j++)
        {
            if (location_subkeys.at(j).startsWith("web.") == true)
            {
                web_urls.push_back(std::make_pair(locations_ini.value(location_subkeys.at(j)).toString().split('|').at(0).toStdString(),
                                                  locations_ini.value(location_subkeys.at(j)).toString().split('|').at(1).toStdString()));
            }
        }

        // Switch back to main section.
        locations_ini.endGroup();

        // Store location.
        Location loc_forecast(name, latitude, longitude, map, web_urls);
        loc_forecasts.push_back(loc_forecast);
    }
}
