#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>
#include <QThreadPool>
#include <fstream>
#include <QCoreApplication>
#include <QCommandLineParser>

#include "temperature.h"
#include "wind.h"
#include "location.h"
#include "utils.h"
#include "arome_grib_downloader.h"
#include "meteo_forecast.h"
#include "meteo_forecast_processor.h"
#include "meteo_forecast_printer.h"
#include "location_ini_parser.h"

#define LOCATIONS_FILE_DEFAULT "aromatic-default.ini"
#define LOCATIONS_FILE         "aromatic.ini"

int main(int argc, char *argv[])
{
    // Create a command line parser.
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("aromatic");
    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser cmd_parser;
    cmd_parser.setApplicationDescription("A forecast parser for Meteo France AROME files");
    cmd_parser.addHelpOption();
    cmd_parser.addVersionOption();

    // -s, --summer: generate report for summer spots only.
    QCommandLineOption summer_opt(QStringList() << "s" << "summer",
                                  QCoreApplication::translate("main", "Get forecast report for summer spots."));
    cmd_parser.addOption(summer_opt);

    // -w, --winter: generate report for winter spots only.
    QCommandLineOption winter_opt(QStringList() << "w" << "winter",
                                  QCoreApplication::translate("main", "Get forecast report for winter spots."));
    cmd_parser.addOption(winter_opt);

    // -f, --full_report: generate report for all spots.
    QCommandLineOption full_opt(QStringList() << "f" << "full_report",
                                QCoreApplication::translate("main", "Get forecast report for all spots (same as --summer --winter)."));
    cmd_parser.addOption(full_opt);

    // Process the actual command line arguments given by the user.
    cmd_parser.process(app);
    bool do_summer_report = cmd_parser.isSet(summer_opt);
    bool do_winter_report = cmd_parser.isSet(winter_opt);
    bool do_full_report   = cmd_parser.isSet(full_opt);
    if (do_full_report == true)
    {
        do_summer_report = true;
        do_winter_report = true;
    }

    // No report type specified: show help and exit.
    if ((do_summer_report == false) &&
        (do_winter_report == false) &&
        (do_full_report == false))
    {
        cmd_parser.showHelp(0);
    }

    // Get the latest AROME GRIB files from Meteo France.
    Arome_grib_downloader dl;
    if (dl.run() == false)
    {
        std::cout << "ERROR: can not download AROME GRIB file." << std::endl;
        return -1;
    }

    // Force locale (needed by std::float::to_string() for example).
    std::setlocale(LC_ALL, "en_US.UTF-8");
    std::setlocale(LC_NUMERIC, "en_EN");

    // Get locations defined in aromatic.ini file.
    std::vector<Location> locations;
    Location_ini_parser ini_parser(LOCATIONS_FILE_DEFAULT, LOCATIONS_FILE);
    ini_parser.run(locations);

    // Prepare forecast for locations required by the user.
    std::vector<Meteo_forecast> meteo_forecasts;
    for (auto &location : locations)
    {
        if (((do_winter_report == true) && (location.get_season_type() == "winter"))
           || ((do_summer_report == true) && (location.get_season_type() == "summer")))
        {
            meteo_forecasts.push_back(Meteo_forecast(location));
        }
    }

    // Compute forecasts.
    for (auto &mto : meteo_forecasts)
    {
        Meteo_forecast_processor forecast_proc;
        forecast_proc.run(mto, dl.get_file_list());
    }

    // Print forecasts.
    std::vector<std::pair<std::string,std::string>> txt_sections;
    txt_sections.push_back(std::make_pair("Meteo Caplain",   "http://mto38.free.fr"));
    txt_sections.push_back(std::make_pair("Meteo France",    "http://www.meteofrance.com"));
    txt_sections.push_back(std::make_pair("Meteo Parapente", "http://www.meteoparapente.com"));
    txt_sections.push_back(std::make_pair("Unhooked Spots",  "http://www.unhooked.ch/2008/spotguide/"));
    std::string forecast_name;
    if (do_full_report == true)
    {
        forecast_name = "";
    }
    else if (do_summer_report == true)
    {
        forecast_name = "Summer";
    }
    else if (do_winter_report == true)
    {
        forecast_name = "Winter";
    }
    Meteo_forecast_printer printer(forecast_name, dl.get_run_date("%a %d %b %Y %H:%M"), meteo_forecasts, txt_sections);
    std::cout << printer.get_txt() << std::endl;

    // Write forecasts as HTML file.
    std::string html = printer.get_html();
    std::ofstream output_file("forecast.html");
    output_file << html;

    return 0;
}

