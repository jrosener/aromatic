#ifndef AROME_GRIB_DOWNLOADER_H
#define AROME_GRIB_DOWNLOADER_H

#include <string>
#include <vector>

class Arome_grib_downloader
{
 private:
    std::string orig_dir;
    std::string dl_dir;

 public:
    Arome_grib_downloader();
    bool run();
    std::vector<std::string> get_file_list();

 private:
    std::string get_date_str(time_t date_t, const std::string &format);
    std::string get_last_available_run_time(const std::string &run_date);
    bool url_exists(const std::string &url);
};

#endif // AROME_GRIB_DOWNLOADER_H

