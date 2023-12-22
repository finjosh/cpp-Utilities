#include "include/Examples/Log.h"

void LogTest::test()
{
    // When you run the program there will be a DebugLog.txt file created which is where all of the Logs will be printed
    // To create a log you use the command CreateLog
    Log::CreateLog(LogType::Debug, "Creating a log");
    // When it writes the String to the file it will include the infomation about which Log type it is and the time is was added
}
