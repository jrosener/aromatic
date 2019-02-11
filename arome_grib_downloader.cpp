#include <unistd.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <QtConcurrentMap>

#include "arome_grib_downloader.h"
#include "utils.h"

#define DL_DIR "dl"
#define TMP_DIR "tmp"
#define METEO_FRANCE_DL_URL "http://dcpc-nwp.meteo.fr/services/PS_GetCache_DCPCPreviNum?token=__5yLVTdr-sGeHoPitnFc7TZ6MhBcJxuSsoZp6y0leVHU__&model=AROME&grid=0.01&package=SP1"
#define METEO_FRANCE_NB_HOUR_OFFSET 36

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
    // Convert date as string.
    std::tm date = *std::localtime(&date_t);
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
    std::string base_url(METEO_FRANCE_DL_URL);

    // Based on the current date, check if run from 18:00 exists, if not check 12:00, etc...
    std::cout << "Looking for the latest run:" << std::endl;
    if (this->url_exists(base_url + "&time=00H&referencetime=" + run_date + "T18:00:00Z") == true)
        run_time = "18";
    else if (this->url_exists(base_url + "&time=00H&referencetime=" + run_date + "T12:00:00Z") == true)
        run_time = "12";
    else if (this->url_exists(base_url + "&time=00H&referencetime=" + run_date + "T06:00:00Z") == true)
        run_time = "06";
    else if (this->url_exists(base_url + "&time=00H&referencetime=" + run_date + "T03:00:00Z") == true)
        run_time = "03";
    else if (this->url_exists(base_url + "&time=00H&referencetime=" + run_date + "T00:00:00Z") == true)
        run_time = "00";
    std::cout << "Use run " << run_time << "H" << std::endl;

    return run_time;
}

static std::string dl_file_static(const std::string &url)
{
    // Use curl to download the file.
    // Silent mode but still return an error message in case of failure.
    std::string res = exec("curl -s -S -f -O -J -L \"" + url + "\" 2>&1");

    // Hack for Meteo France server: if the downloaded file name starts with
    // "PS_GetCache_DCPCPreviNum" it means that the download failed, so try again.
    unsigned int timeout = 0;
    if (res.find("curl: (") == std::string::npos) // no curl error.
    {
        size_t offset_idx = url.find("H&referencetime", 0);
        if (offset_idx != std::string::npos)
        {
            std::string offset = url.substr(offset_idx - 2, 3);
            //std::cout << "no error, check PS_GetCache: " << exec("ls | grep PS_GetCache_DCPCPreviNum | grep " + offset) << std::endl;
            while ((exec("ls | grep PS_GetCache_DCPCPreviNum | grep " + offset) != "") && (timeout < 10))
            {
                std::cout << "cannot dl " << url << " : remove PS_GetCache and try again" << std::endl;
                exec("rm PS_GetCache_DCPCPreviNum*" + offset + "*");
                res = exec("curl -s -S -f -O -J -L \"" + url + "\" 2>&1");
                timeout++;
            }
        }
    }

    // Check if curl returns an error (ex: 404 file not found,...).
    if ((res.find("curl: (") != std::string::npos) || (timeout == 10))
        std::cout << "Downloading " << url << "... Failed" << std::endl << " (" << res << ")" << std::endl;
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
    std::string base_url(METEO_FRANCE_DL_URL);
    time_t today = time(0);
    std::string run_date = this->get_date_str(today, "%Y%m%d");
    std::string run_date_dash = this->get_date_str(today, "%Y-%m-%d");
    std::string run_time = this->get_last_available_run_time(run_date_dash);

    if (run_time == "-1")
    {
        // Try downloading the previous day if the current day is still not available.
        std::tm prev_day = *std::localtime(&today);
        prev_day.tm_mday = prev_day.tm_mday - 1;
        time_t prev_day_t = mktime(&prev_day);
        run_date = this->get_date_str(prev_day_t, "%Y%m%d");
        run_date_dash = this->get_date_str(prev_day_t, "%Y-%m-%d");
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

    // Store the run date/time.
    std::tm run_date_time = *std::localtime(&today);
    run_date_time.tm_min = 0;
    run_date_time.tm_sec = 0;
    run_date_time.tm_hour = std::stoi(run_time);
    this->run_date_time = mktime(&run_date_time);

    // Download all files from the last run.
    if (result == true)
    {
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
                if (!std::ifstream("AROME_0.01_SP1_" + offset + "H_" + run_date + run_time + "00.grib2"))
                {
                    urls.push_back(base_url + "&time=" + offset + "H&referencetime=" + run_date_dash + "T" + run_time + ":00:00Z");
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
        files.push_back(this->dl_dir + "/" + item.c_str());
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
