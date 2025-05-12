#include "Utils/Log.hpp"
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iostream>
#include <cassert>

bool Log::printToCout = false;
std::string Log::LogFolder = "Logs";
std::ofstream Log::file;

Log::Log()
{
    // per day
    constexpr const int maxLogs = 10000;

    if (!std::filesystem::exists(Log::LogFolder))
    {
        std::filesystem::create_directory(Log::LogFolder);
    }

    std::string currentDate = Log::getCurrentDate();
    std::string fileName = Log::LogFolder + "/" + currentDate + ".log";
    for (int i = 0; i < maxLogs; ++i)
    {
        if (std::filesystem::exists(fileName))
        {
            fileName = Log::LogFolder + "/" + currentDate + "-" + std::to_string(i) + ".log";
        }
        else
        {
            break;
        }
    }

    Log::file.open(fileName, std::ios::out | std::ios::app);
    if (!Log::file.is_open())
    {
        std::cerr << "Error opening log file: " << Log::LogFolder << std::endl;
    }
}

const std::map<const Log::Type, const std::string> Log::LogDictionary = {
    {Log::Type::Error, "Error"},
    {Log::Type::Debug, "Debug"},
    {Log::Type::Info, "Info"},
    {Log::Type::Warning, "Warning"},
    {Log::Type::Fatal, "Fatal"} };

void Log::setPrintToCout(bool printToCout)
{
    printToCout = printToCout;
}

bool Log::getPrintToCout()
{
    return printToCout;
}

void Log::add(Log::Type logType, std::string message)
{
    assert(Log::file.is_open() && "Log file failed to open, no logs will be saved");

    auto now = std::chrono::system_clock::now();
 
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::string temp = std::ctime(&time);
    temp.erase(temp.begin() + temp.size()-1);

    Log::file << '[' << temp << "] [" << Log::LogDictionary.find(logType)->second << "] -- " << message << std::endl;
    Log::file.flush();
}

std::string Log::getCurrentTime()
{
    std::time_t time = std::time(nullptr);
    constexpr const char timeFormat[] = "%T%";
    char rtn[sizeof("HH:MM:SS")];
    std::strftime(rtn, sizeof(rtn), timeFormat, std::localtime(&time));
    return std::string(rtn);
}

std::string Log::getCurrentDate()
{
    std::time_t time = std::time(nullptr);
    constexpr const char timeFormat[] = "%F%";
    char rtn[sizeof("yyyy-mm-dd")];
    std::strftime(rtn, sizeof(rtn), timeFormat, std::localtime(&time));
    return std::string(rtn);
}
