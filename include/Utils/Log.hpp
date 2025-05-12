#ifndef LOG_H
#define LOG_H

#pragma once

#include <string>
#include <fstream>
#include <map>
#include <cstdint>

/*
TODO list:
    - Logs are created per launch
    - Logs are removed once a memory limit is passed
    - Log folder can be set during runtime 
        - No logs will be moved just the new logs location
    - opt: Memory limit can be set and changed during runtime?
    - opt: ZIP Logs?
*/

#define GET_FUNCTION_INFO() __FILE__ + ':' + std::to_string(__LINE__) + " " + __PRETTY_FUNCTION__

class Log
{
public:
    enum Type : int8_t
    {
        Error = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Fatal = 4
    };

    /// @brief directory is based on the working directory
    static void setLogFolder(std::string path = "Logs");
    static std::string getLogFolder();
    static void setPrintToCout(bool printToCout);
    static bool getPrintToCout();

    static void add(Log::Type logType, std::string message);

protected:
    Log();

    static std::string getCurrentTime();
    static std::string getCurrentDate();

private:
    static bool printToCout;
    static std::string LogFolder;
    static std::ofstream file;
    static const std::map<const Log::Type, const std::string> LogDictionary;
};

#endif
