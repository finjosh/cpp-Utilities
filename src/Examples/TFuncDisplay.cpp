#include "include/Examples/TFuncDisplay.hpp"

void TFuncDisplayTest::test()
{
    //! Depends on TerminatingFunction, SFML Graphics, TGUI, and can be used in combination with CommandHandler and CommandPrompt

    //! NOTE - you must call the init function
    // You should call the close function before the program closes to prevent crashing
    // You must call the update function once every frame or else the terminating functions will not be updated
    // For examples of the above functions in use please look in the main
    
    // Other than the mandatory functions you can open, close, minimize, and maximize the window
    
    // I'm going to use terminating functions to show what each function does

    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 2)
        {
            TFuncDisplay::setVisible(true);
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 8)
        {
            TFuncDisplay::minimize();
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 10)
        {
            TFuncDisplay::maximize();
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 12)
        {
            TFuncDisplay::setVisible(false);
            return;
        }
        data->setRunning();
    });
}
