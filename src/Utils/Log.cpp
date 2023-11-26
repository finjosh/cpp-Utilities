#include "include\Utils\Log.h"

std::string Log::LogFile = "DebugLog.txt";
std::ofstream Log::file = std::ofstream(Log::LogFile, std::ios_base::out | std::ios_base::app);
const std::map<const LogType, const std::string> Log::LogDictionary = {
    {LogType::Error, "Error"},
    {LogType::Debug, "Debug"},
    {LogType::Info, "Info"},
    {LogType::Warning, "Warning"},
    {LogType::Fatal, "Fatal"} };

void Log::CreateLog(LogType logType, std::string message)
{
    auto now = std::chrono::system_clock::now();
 
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::string temp = std::ctime(&time);
    temp.erase(temp.begin() + temp.size()-1);

    Log::file << '[' << temp << "] [" << Log::LogDictionary.find(logType)->second << "] -- " << message << std::endl;
    Log::file.flush();
}
