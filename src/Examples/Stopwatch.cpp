#include "include/Examples/Stopwatch.h"

void StopwatchTest::test()
{
    using namespace std;
    using namespace timer;

    cout << "Outputs for Stopwatch: " << endl;

    // when creating the stopwatch it starts the timer
    Stopwatch timer; 
    
    // but you can restart the timer by calling "start"
    timer.start();
    std::this_thread::sleep_for(std::chrono::duration<double, std::ratio<1,1>>(1)); // sleeping this thread for 1 second
    cout << timer.lap<Stopwatch::TimeFormat::MILLISECONDS>() << endl;
    cout << float(timer.elapsed<Stopwatch::TimeFormat::MILLISECONDS>())/1000.0 << endl; // printing seconds as float
}
