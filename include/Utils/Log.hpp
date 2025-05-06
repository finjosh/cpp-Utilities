#ifndef LOG_H
#define LOG_H

#pragma once

#include <string>
#include <chrono>
#include <fstream>
#include <ctime>
#include <map>

enum LogType : char
{
    Error = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Fatal = 4
};

class Log
{
public:

    static void CreateLog(LogType logType, std::string message);
    // /// @brief creates commands for dumping memory only logs to a file
    // /// @note also dumps 
    // static void initCommands();

private:

    static std::string LogFile;
    static std::ofstream file;
    static const std::map<const LogType, const std::string> LogDictionary;

};

#endif
