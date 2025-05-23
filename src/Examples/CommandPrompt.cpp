#include "include/Examples/CommandPrompt.hpp"

void CommandPromptTest::test()
{    
    //! Depends on SFML Graphics, StringHelper, and CommandHandler

    // Note that there are some functions that need to be called in the main
    // init, UpdateEvent, and close

    // Other that the required functions you have
    // openPrompt which is for opening the window
    TFunc::Add([](TData* data){ if (data->getTotalTime() >= 3) Command::Prompt::setVisible(); else data->setRunning(); });

    // closePrompt
    TFunc::Add([](TData* data){ if (data->getTotalTime() >= 5) Command::Prompt::setVisible(false); else data->setRunning(); });

    // TODO update this
    // // print which prints to the command prompt
    // Command::Prompt::print("Something was printed");

    // // isPrintAllowed which checks if print is functional at the moment
    // Command::Prompt::print("Is printing allowed:");
    // Command::Prompt::print(std::to_string(Command::Prompt::isPrintAllowed()));

    // // allowPrint changes if printing is allowed
    // Command::Prompt::allowPrint(false);
    // Command::Prompt::print("This will not be printed");
    // Command::Prompt::allowPrint(true);

    // Command::Prompt::print("To open the prompt click \"~\"");
    // Command::Prompt::print("To get a list of commands type \"help\"");

    // everything else about the command prompt is used during runtime
    // any command that is added to the CommandHandler will but in the CommandPrompt
    // There is command suggestions and command history
}
