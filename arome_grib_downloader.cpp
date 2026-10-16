#include <unistd.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <QtConcurrentMap>
#include <QString>
#include <QFile>

#include "arome_grib_downloader.h"
#include "utils.h"

#define DL_DIR "dl"
#define TMP_DIR "tmp"
#define METEO_FRANCE_DL_URL "https://meteofrance-pnt.s3.rbx.io.cloud.ovh.net/pnt"
#define METEO_FRANCE_NB_HOUR_OFFSET 51

// Name of the AROME 0.01 SP1 GRIB file for a run and an hour offset.
// Ex: arome__001__SP1__00H__2026-10-16T03:00:00Z.grib2
static std::string get_grib_file_name(const std::string &run_date, const std::string &run_time, const std::string &offset)
{
    return "arome__001__SP1__" + offset + "H__" + run_date + "T" + run_time + ":00:00Z.grib2";
}

// URL of the AROME 0.01 SP1 GRIB file for a run and an hour offset.
// Ex: https://meteofrance-pnt.s3.rbx.io.cloud.ovh.net/pnt/2026-10-16T03:00:00Z/arome/001/SP1/arome__001__SP1__00H__2026-10-16T03:00:00Z.grib2
static std::string get_grib_url(const std::string &run_date, const std::string &run_time, const std::string &offset)
{
    return std::string(METEO_FRANCE_DL_URL) + "/" + run_date + "T" + run_time + ":00:00Z/arome/001/SP1/"
           + get_grib_file_name(run_date, run_time, offset);
}

Arome_grib_downloader::Arome_grib_downloader()
{
    // Store current directory.
    char cur_path[256];
    std::string d(getcwd(cur_path, sizeof(cur_path)));
    this->orig_dir = d;

    // Set the download path.
    this->dl_dir = this->orig_dir + "/" + DL_DIR;
}

std::string Arome_grib_downloader::get_date_str(time_t date_t, const std::string &format)
{
    // Convert date as string (UTC because Meteo France runs are identified by their UTC date and time).
    std::tm date = *std::gmtime(&date_t);
    std::stringstream ss;
    ss << std::put_time(&date, format.c_str());

    return ss.str();
}

std::string Arome_grib_downloader::get_run_date(const std::string &format)
{
    return this->get_date_str(this->run_date_time, format);
}

std::string Arome_grib_downloader::get_last_available_run_time(const std::string &run_date)
{
    std::string run_time = "-1";

    // Based on the current date, check if run from 21:00 exists, if not check 18:00, etc...
    // Files of a run are published progressively (~50 min between 00H and 51H),
    // so check the last hour offset to only select a complete run.
    const char *run_times[] = {"21", "18", "15", "12", "09", "06", "03", "00"};
    std::string last_offset = std::to_string(METEO_FRANCE_NB_HOUR_OFFSET);
    std::cout << "Looking for the latest run:" << std::endl;
    for (const std::string rt : run_times)
    {
        if (this->url_exists(get_grib_url(run_date, rt, last_offset)) == true)
        {
            run_time = rt;
            break;
        }
    }
    std::cout << "Use run " << run_time << "H" << std::endl;

    return run_time;
}

static std::string dl_file_static(const std::string &url)
{
    // Use curl to download the file.
    // Silent mode but still return an error message in case of failure.
    std::string res = exec("curl -s -S -f -O -L \"" + url + "\" 2>&1");

    // Check if curl returns an error (ex: 404 file not found,...).
    if (res.find("curl: (") != std::string::npos)
    {
        std::cout << "Downloading " << url << "... Failed" << std::endl << " (" << res << ")" << std::endl;
        // Remove any partially downloaded file so that it is downloaded again on retry.
        exec("rm -f \"" + url.substr(url.find_last_of('/') + 1) + "\"");
    }
    else
        std::cout << "Downloading " << url << "... Done" << std::endl;

    return res;
}

bool Arome_grib_downloader::run()
{
    bool result = true;

    // Find the last available meteo files.
    std::string tmp_dir = this->orig_dir + "/" + TMP_DIR;
    exec("rm -rf " + tmp_dir);
    exec("mkdir " + tmp_dir);
    if (chdir(tmp_dir.c_str()) != 0)
    {
        std::cout << "ERROR: Can not change directory to " << tmp_dir << std::endl;
        return false;
    }
    time_t run_day = time(0);
    std::string run_date = this->get_date_str(run_day, "%Y%m%d");
    std::string run_date_dash = this->get_date_str(run_day, "%Y-%m-%d");
    std::string run_time = this->get_last_available_run_time(run_date_dash);

    if (run_time == "-1")
    {
        // Try downloading the previous day if the current day is still not available.
        run_day = run_day - 24 * 60 * 60;
        run_date = this->get_date_str(run_day, "%Y%m%d");
        run_date_dash = this->get_date_str(run_day, "%Y-%m-%d");
        run_time = this->get_last_available_run_time(run_date_dash);
    }
    if (run_time == "-1")
    {
        std::cout << "ERROR: Can not download GRIB files from Meteo France server." << std::endl;
        return false;
    }
    if (chdir(this->orig_dir.c_str()) != 0)
    {
        std::cout << "ERROR: Can not change directory to " << this->orig_dir << std::endl;
        return false;
    }

    // Store the run date/time (UTC).
    std::tm run_date_time = *std::gmtime(&run_day);
    run_date_time.tm_min = 0;
    run_date_time.tm_sec = 0;
    run_date_time.tm_hour = std::stoi(run_time);
    this->run_date_time = timegm(&run_date_time);

    // Download all files from the last run.
    if (result == true)
    {
        // Store the run date/time to a file.
        QString run_file = ".run_date";
        QFile file(run_file);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << QString::fromStdString(run_date) << "-" << QString::fromStdString(run_time) << "00" << Qt::endl;
        }

        // Create/switch to download dir.
        this->dl_dir = this->dl_dir + "-" + run_date + "-" + run_time + "00";
        exec("mkdir " + this->dl_dir);
        if (chdir(this->dl_dir.c_str()) != 0)
        {
            std::cout << "ERROR: Can not change directory to " << this->dl_dir << std::endl;
            return false;
        }

        // Perform download 5 times (because it often happens that some download are failing).
        int max_retry = 5;
        while ((max_retry > 0) && (this->get_file_list().size() <= METEO_FRANCE_NB_HOUR_OFFSET)) // Hack: because sometimes files exists but are not retrived by the server, then try again.
        {
            // Prepare a list of URLs to download.
            std::vector<std::string> urls;
            for (int i = 0; i <= METEO_FRANCE_NB_HOUR_OFFSET; i++)
            {
                std::string offset = std::to_string(i);
                if (offset.length() == 1) offset = '0' + offset;
                // Download only if it does not exists.
                if (!std::ifstream(get_grib_file_name(run_date_dash, run_time, offset)))
                {
                    urls.push_back(get_grib_url(run_date_dash, run_time, offset));
                }
            }
            // Download all URLs.
            std::cout << std::endl;
            std::cout << "Downloading AROME files:" << std::endl;
            if (urls.size() == 0)
            {
                std::cout << "Nothing to download" << std::endl;
            }
        #if 0 // parallel
            // Set number of threads for paralellization to 2.
            // This is a MeteoFrance limitation:
            // "Depuis le 5 avril 2017, les téléchargements directs de données issues des modèles
            //  seront limités à deux connexions concurrentielles pour une même IP, afin de répartir
            //  de le trafic de manière plus équitable".
            QThreadPool::globalInstance()->setMaxThreadCount(2);
            QtConcurrent::blockingMapped<std::vector<std::string>>(urls, dl_file_static);
        #else // sequential
            // Use sequential download because it seems thar even 2 parallel
            // downloads are rejected by MeteoFrance.
            for (auto &u : urls)
            {
                dl_file_static(u);
            }
        #endif
            max_retry--;
        }

        // Switch back to original dir.
        if (chdir(this->orig_dir.c_str()) != 0)
        {
            std::cout << "ERROR: Can not change directory to " << this->orig_dir << std::endl;
            return false;
        }
    }

    return result;
}

std::vector<std::string> Arome_grib_downloader::get_file_list()
{
    // Get list of files in the download directory.
    std::string ls_res = exec("ls " + this->dl_dir + " | grep grib2");

    // Put file names in a table.
    std::vector<std::string> files;
    std::stringstream ss(ls_res);
    std::string item;
    while (std::getline(ss, item, '\n'))
    {
        QString it = QString::fromStdString(item);
        files.push_back(this->dl_dir + "/" + it.replace(" ", "\\ ")
                                               .replace("\"", "\\\"")
                                               .replace(",", "\\,")
                                               .toStdString().c_str());
    }

    return files;
}

bool Arome_grib_downloader::url_exists(const std::string &url)
{
    // Try to download the file.
    std::string res = dl_file_static(url);

    // Check if curl returned an error (file not found).
    if (res.find("curl: (") != std::string::npos)
    {
        return false;
    }

    return true;
}
