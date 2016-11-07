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
#include "meteo_forecast.h"
#include "utils.h"
#include "arome_grib_downloader.h"
#include "meteo_forecast_processor.h"
#include "meteo_forecast_printer.h"


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

    // Limit number of threads (see calls to QtConcurrent::blockingMapped()).
    if (QThreadPool::globalInstance()->maxThreadCount() > 1)
    {
        QThreadPool::globalInstance()->setMaxThreadCount(QThreadPool::globalInstance()->maxThreadCount() - 1);
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
    //std::setlocale(LC_TIME, "ja_JP");

    // Extract forecast at specific location from AROME files.
    Meteo_forecast_processor forecast_proc;
    std::vector<Meteo_forecast> forecasts;

    if (do_winter_report == true)
    {
        // Col du Petit Saint Bernard
        std::vector<std::pair<std::string,std::string>> psb;
        psb.push_back(std::make_pair("balise ffvl",      "http://balisemeteo.com/balise_histo.php?idBalise=115"));
        psb.push_back(std::make_pair("balise la thuile", "http://www.meteolathuile.com/"));
        psb.push_back(std::make_pair("webcams",          "http://www.larosiere.net/webcams.html"));
        psb.push_back(std::make_pair("meteofrance",      "http://www.meteofrance.com/previsions-meteo-montagne/la-rosiere-1850/73700"));
        psb.push_back(std::make_pair("meteoblue",        "https://www.meteoblue.com/fr/meteo/prevision/multimodel/col-du-petit-saint-bernard_france_2998085"));
        forecasts.push_back(forecast_proc.run("Col du Petit Saint Bernard", "FR", 45.68, 6.88, dl.get_file_list(), psb));

        // Crozet - Lelex
        std::vector<std::pair<std::string,std::string>> czt;
        czt.push_back(std::make_pair("balise",        "http://romma.fr/station_24.php?id=108&tempe=1"));
        czt.push_back(std::make_pair("webcam",        "http://m.webcam-hd.com/mont-jura/lelex-1680"));
        czt.push_back(std::make_pair("meteofrance",   "http://www.meteofrance.com/previsions-meteo-montagne/monts-jura/01410"));
        czt.push_back(std::make_pair("meteocentrale", "http://www.meteocentrale.ch/fr/europe/suisse/temps-la-dole/details/S067020/"));
        czt.push_back(std::make_pair("meteoblue",     "https://www.meteoblue.com/fr/meteo/prevision/multimodel/46.3N5.98E1529"));
        forecasts.push_back(forecast_proc.run("Crozet-Lelex", "FR", 46.32, 5.99, dl.get_file_list(), czt));

        // Le Salève
        std::vector<std::pair<std::string,std::string>> slv;
        slv.push_back(std::make_pair("balise",          "http://www.romma.fr/station_24.php?id=73&tempe=1"));
        slv.push_back(std::make_pair("balise ffvl",     "http://www.balisemeteo.com/balise_histo.php?idBalise=167"));
        slv.push_back(std::make_pair("balise pioupiou", "http://pioupiou.fr/fr/70"));
        slv.push_back(std::make_pair("webcams",         "http://meteocams.fr/fr/haute-savoie/mont-saleve/"));
        slv.push_back(std::make_pair("meteofrance",     "http://www.meteofrance.com/previsions-meteo-france/collonges-sous-saleve/74160"));
        slv.push_back(std::make_pair("meteoblue",       "https://www.meteoblue.com/fr/meteo/prevision/multimodel/le-grand-piton-%28mont-sal%C3%A8ve%29_france_6932127"));
        forecasts.push_back(forecast_proc.run("Le Salève", "FR", 46.14, 6.18, dl.get_file_list(), slv));

        // Le Semnoz
        std::vector<std::pair<std::string,std::string>> smz;
        smz.push_back(std::make_pair("balise ffvl",     "http://balisemeteo.com/balise_histo.php?idBalise=67"));
        smz.push_back(std::make_pair("webcams",         "http://m.webcam-hd.com/semnoz"));
        smz.push_back(std::make_pair("meteofrance",     "http://www.meteofrance.com/previsions-meteo-montagne/le-semnoz/74540"));
        smz.push_back(std::make_pair("meteoblue",       "https://www.meteoblue.com/fr/meteo/prevision/multimodel/montagne-de-semnoz_france_2975160"));
        forecasts.push_back(forecast_proc.run("Le Semnoz", "FR", 45.79, 6.10, dl.get_file_list(), smz));

        // Col des Supeyres
        std::vector<std::pair<std::string,std::string>> sup;
        sup.push_back(std::make_pair("balise",      "http://www.lechaletdesgentianes.fr/meteo/index.htm"));
        sup.push_back(std::make_pair("webcams",     "http://lechaletdesgentianes.fr/chalet/infos/webcams"));
        sup.push_back(std::make_pair("meteofrance", "http://www.meteofrance.com/previsions-meteo-montagne/chalmazel/42920"));
        sup.push_back(std::make_pair("meteoblue",   "https://www.meteoblue.com/fr/meteo/prevision/multimodel/pierre-sur-haute_france_2987226"));
        forecasts.push_back(forecast_proc.run("Col des Supeyres", "FR", 45.60, 3.84, dl.get_file_list(), sup));

        // Col de la Croix de Fer
        std::vector<std::pair<std::string,std::string>> cdf;
        cdf.push_back(std::make_pair("webcam",      "http://m.webcam-hd.com/le-corbier/pointe"));
        cdf.push_back(std::make_pair("meteofrance", "http://www.meteofrance.com/previsions-meteo-montagne/saint-sorlin-darves/73530"));
        cdf.push_back(std::make_pair("meteoblue",   "https://www.meteoblue.com/fr/meteo/prevision/multimodel/col-de-la-croix-de-fer_france_3022355"));
        forecasts.push_back(forecast_proc.run("Col de la Croix de Fer", "FR", 45.24, 6.19, dl.get_file_list(), cdf));

        // Col du Lautaret
        std::vector<std::pair<std::string,std::string>> lau;
        lau.push_back(std::make_pair("balise",      "http://romma.fr/station_24.php?id=21"));
        lau.push_back(std::make_pair("webcam",      "https://www.jardinalpindulautaret.fr/jardin/visite-virtuelle-et-images/webcam"));
        lau.push_back(std::make_pair("meteofrance", "http://www.meteofrance.com/previsions-meteo-montagne/serre-chevalier/05100"));
        lau.push_back(std::make_pair("meteoblue",   "https://www.meteoblue.com/fr/meteo/prevision/multimodel/col-du-lautaret_france_3005951"));
        forecasts.push_back(forecast_proc.run("Col du Lautaret", "FR", 45.04, 6.41, dl.get_file_list(), lau));

        // Col du Grand Saint Bernard
        std::vector<std::pair<std::string,std::string>> gsb;
        gsb.push_back(std::make_pair("balise",    "http://www.meteociel.fr/temps-reel/obs_villes.php?code2=6717"));
        gsb.push_back(std::make_pair("meteoblue", "https://www.meteoblue.com/fr/meteo/prevision/multimodel/col-du-grand-saint-bernard_conf%C3%A9d%C3%A9ration-suisse_2660518"));
        forecasts.push_back(forecast_proc.run("Col du Grand Saint Bernard", "CH", 45.87, 7.19, dl.get_file_list(), gsb));

        // Col du Simplon
        std::vector<std::pair<std::string,std::string>> spl;
        spl.push_back(std::make_pair("webcam",    "https://www.gsbernard.ch/simplon/webcam.php"));
        spl.push_back(std::make_pair("meteoblue", "https://www.meteoblue.com/fr/meteo/prevision/multimodel/simplonpass_conf%C3%A9d%C3%A9ration-suisse_2658580"));
        forecasts.push_back(forecast_proc.run("Col du Simplon", "CH", 46.25, 8.03, dl.get_file_list(), spl));
    }

    if (do_summer_report == true)
    {
        // Carteau
        std::vector<std::pair<std::string,std::string>> cta;
        cta.push_back(std::make_pair("meteofrance",    "http://www.meteofrance.com/previsions-meteo-france/port-saint-louis-du-rhone/13230"));
        cta.push_back(std::make_pair("meteoblue",      "https://www.meteoblue.com/fr/meteo/prevision/multimodel/port-saint-louis-du-rh%C3%B4ne_france_2985934"));
        cta.push_back(std::make_pair("windguru",       "http://www.windguru.cz/fr/index.php?sc=48609"));
        cta.push_back(std::make_pair("balise windsup", "http://www.winds-up.com/spot-port-st-louis-windsurf-kitesurf-30-observations-releves-vent.html"));
        forecasts.push_back(forecast_proc.run("Carteau", "FR", 43.38, 4.85, dl.get_file_list(), cta));

        // Etang de Thau - Pont Levis
        std::vector<std::pair<std::string,std::string>> tha;
        tha.push_back(std::make_pair("meteofrance", "http://www.meteofrance.com/previsions-meteo-plages/sete/34200"));
        tha.push_back(std::make_pair("meteoblue",   "https://www.meteoblue.com/fr/meteo/prevision/multimodel/s%C3%A8te_france_2974733"));
        tha.push_back(std::make_pair("windguru",    "http://www.windguru.cz/fr/index.php?sc=193009"));
        forecasts.push_back(forecast_proc.run("Etang de Thau - Pont Levis", "FR", 43.40, 3.65, dl.get_file_list(), tha));

        // Lac Léman - Excenevex
        std::vector<std::pair<std::string,std::string>> exc;
        exc.push_back(std::make_pair("meteofrance", "http://www.meteofrance.com/previsions-meteo-france/excenevex/74140"));
        exc.push_back(std::make_pair("meteoblue",   "https://www.meteoblue.com/fr/meteo/prevision/multimodel/excenevex_france_6613597"));
        exc.push_back(std::make_pair("windguru",    "http://www.windguru.cz/int/index.php?sc=198539"));
        exc.push_back(std::make_pair("windspots",   "http://www.windspots.com/spots"));
        forecasts.push_back(forecast_proc.run("Lac Léman - Excenevex", "FR", 46.35, 6.36, dl.get_file_list(), exc));
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
    Meteo_forecast_printer printer(forecast_name, dl.get_run_date("%a %d %b %Y %H:%M"), forecasts, txt_sections);
    std::cout << printer.get_txt() << std::endl;

    // Write forecasts as HTML file.
    std::string html = printer.get_html();
    std::ofstream output_file("forecast.html");
    output_file << html;

    return 0;
}

