#include "include/Examples/UpdateLimiter.hpp"

void UpdateLimiterTest::test()
{
    using namespace std;

    cout << "Outputs for Update Limiter" << endl;
    // limits the rate at which a thread can run
    // the limit is set in frames per second

    UpdateLimiter limit(60);

    // you can also update the limit while it is in use
    limit.updateLimit(16);

    // Using stopwatch to print to console how long its taking
    timer::Stopwatch timer;

    for (int i = 0; i < 48; i++)
    {
        timer.start();
    
        // showing we can update limit on the fly
        if (i == 16)
        {
            limit.updateLimit(32);
            cout << "update limit is now 144" << endl;
        }

        // must call wait when you want the thread to wait till next update time
        limit.wait();
        cout << timer.lap<timer::Stopwatch::MILLISECONDS>()/1000.f << "s" << endl;
    }
}