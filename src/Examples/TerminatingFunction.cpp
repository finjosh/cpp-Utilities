#include "include/Examples/TerminatingFunction.hpp"

void TerminatingFunctionTest::test()
{
    using namespace std;

    // There is some setup for Terminating functions
    // to find them please look down in the update for the window (while loop)

    //! Depends on FuncHelper
    cout << "Outputs for Terminating Functions" << endl;

    // By default the data will have a state of finished
    TerminatingFunction::Add([](TData* data){ cout << "Terminating after first run" << endl; });
    // If you dont take in any data then it will be set to the default state
    TerminatingFunction::Add([](){ cout << "Also runs only once" << endl; });
    
    // to remove a function you need to store its id when added
    std::string temp = TerminatingFunction::Add([](){ cout << "This never runs" << endl; });
    TerminatingFunction::forceRemove(temp);

    // this would clear all current functions
    // TerminatingFunction::clear();

    // data has the delta time the total running time and the state of the function
    // here is an example usage with total time
    TerminatingFunction::Add([](TData* data){ 
        cout << "Total Time Running: " << data->totalTime << endl;
        if (data->totalTime <= 2) 
        {
            data->setRunning();
        }
    });

    // infinitely running function
    TerminatingFunction::Add({[](TData* data){         
        data->setRunning();
    }});

    // normally infinitely running function but has max of 20 seconds
    TerminatingFunction::Add({[](TData* data){
        // note you can account for a early stop
        if (data->isForceStop)
        {
            std::cout << "Early stop at 20 seconds" << std::endl;
            // do stuff
        }
        data->setRunning();
    }}, 20.f);
}
