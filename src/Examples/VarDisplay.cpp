#include "include/Examples/VarDisplay.hpp"

void VarDisplayTest::test()
{
    //! Depends on LiveVar, SFML Graphics, TGUI, and can be used in combination with CommandHandler and CommandPrompt

    //! NOTE - you must call the init function before any vars are added for them to show up in the display
    // You should call the close function before the program closes to prevent crashing
    // You must call the update function once every frame or else the variables will not be updated
    // For examples of the above functions in use please look in the main
    
    // Other than the mandatory functions you can open, close, minimize, and maximize the window
    
    // I'm going to use terminating functions to show what each function does

    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 2)
        {
            VarDisplay::openWindow();
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 4)
        {
            VarDisplay::minimizeWindow();
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 6)
        {
            VarDisplay::maximizeWindow();
            return;
        }
        data->setRunning();
    });
    TFunc::Add([](TData* data)
    {
        if (data->totalTime >= 8)
        {
            VarDisplay::closeWindow();
            return;
        }
        data->setRunning();
    });
}
