#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <utility>
#include <QThreadPool>
#include <fstream>

#include "temperature.h"
#include "wind.h"
#include "meteo_forecast.h"
#include "utils.h"
#include "arome_grib_downloader.h"
#include "meteo_forecast_processor.h"
#include "meteo_forecast_printer.h"

int main()
{
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

    // Extract forecast at specific location from AROME files.
    Meteo_forecast_processor forecast_proc;
    std::vector<Meteo_forecast> forecasts;

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
    forecasts.push_back(forecast_proc.run("Crozet-Lelex", "FR", 46.30, 5.98, dl.get_file_list(), czt));

    // Col de la Croix de Fer
    std::vector<std::pair<std::string,std::string>> cdf;
    forecasts.push_back(forecast_proc.run("Col de la Croix de Fer", "FR", 45.24, 6.19, dl.get_file_list(), cdf));

    // Le Semnoz
    std::vector<std::pair<std::string,std::string>> smz;
    forecasts.push_back(forecast_proc.run("Le Semnoz", "FR", 45.79, 6.10, dl.get_file_list(), smz));

    // Col du Lautaret
    std::vector<std::pair<std::string,std::string>> lau;
    forecasts.push_back(forecast_proc.run("Col du Lautaret", "FR", 45.04, 6.41, dl.get_file_list(), lau));

    // Le Salève
    std::vector<std::pair<std::string,std::string>> slv;
    forecasts.push_back(forecast_proc.run("Le Salève", "FR", 46.14, 6.18, dl.get_file_list(), slv));

    // Col du Grand Saint Bernard
    std::vector<std::pair<std::string,std::string>> gsb;
    forecasts.push_back(forecast_proc.run("Col du Grand Saint Bernard", "CH", 45.87, 7.19, dl.get_file_list(), gsb));

    // Col du Simplon
    std::vector<std::pair<std::string,std::string>> spl;
    forecasts.push_back(forecast_proc.run("Col du Simplon", "CH", 46.25, 8.03, dl.get_file_list(), spl));

    // Carteau
    std::vector<std::pair<std::string,std::string>> cta;
    forecasts.push_back(forecast_proc.run("Carteau", "FR", 43.38, 4.85, dl.get_file_list(), cta));

    // Etang de Thau - Pont Levis
    std::vector<std::pair<std::string,std::string>> tha;
    forecasts.push_back(forecast_proc.run("Etang de Thau - Pont Levis", "FR", 43.40, 3.65, dl.get_file_list(), tha));


    // Print forecasts.
    Meteo_forecast_printer printer(forecasts);
    std::cout << printer.get_txt() << std::endl;

    // Write html forecasts to file.
    std::string html = printer.get_html();
    std::ofstream output_file("forecast.html");
    output_file << html;
    //std::cout << std::endl << html << std::endl;

    return 0;
}

