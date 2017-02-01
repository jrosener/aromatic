#include <iostream>
#include <sstream>
#include <iomanip>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include "location_ini_parser.h"

Location_ini_parser::Location_ini_parser(const std::string &ini_default_file,
                                         const std::string &ini_file) :
                                            file(ini_file)
{
    // Create aromatic.ini based on aromatic-default.ini
    QFileInfo locations_file(QString::fromStdString(this->file));
    if (locations_file.exists() == false)
    {
        std::cout << "INFO: " << ini_file << " does not exists, use "<< ini_default_file << " as a base." << std::endl;
        QFile::copy(QString::fromStdString(ini_default_file), QString::fromStdString(this->file));
    }
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
        std::string country = locations_ini.value("country").toString().toStdString();
        std::string season = locations_ini.value("season").toString().toStdString();
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
        Location loc_forecast(name, latitude, longitude, country, season, web_urls);
        loc_forecasts.push_back(loc_forecast);
    }
}
