#include <memory>
#include <algorithm>

#include "utils.h"

//
// Execute a shell command.
// Return the command result as a string.
//
std::string exec(const std::string &cmd)
{
    // Execute command.
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        return "ERROR";
    }

    // Get string result.
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get()))
    {
        if (fgets(buffer, 128, pipe.get()) != NULL)
        {
            result += buffer;
        }
    }

    // Remove trailing line feed.
    if (result.length() >= 1)
    {
        result.pop_back();
    }

    return result;
}

//
// Check if a string is a number.
//
bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
