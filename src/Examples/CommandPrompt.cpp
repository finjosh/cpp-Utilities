#include "include/Examples/CommandPrompt.hpp"

void CommandPromptTest::test()
{    
    //! Depends on SFML Graphics, StringHelper, and CommandHandler

    // Note that there are some functions that need to be called in the main
    // init, UpdateEvent, and close

    // Other that the required functions you have
    // openPrompt which is for opening the window
    TFunc::Add([](TData* data){ if (data->totalTime >= 3) CommandPrompt::setVisible(); else data->state = TState::Running; });

    // closePrompt
    TFunc::Add([](TData* data){ if (data->totalTime >= 5) CommandPrompt::setVisible(false); else data->state = TState::Running; });

    // print which prints to the command prompt
    CommandPrompt::print("Something was printed", Command::color(0,255,0));

    // isPrintAllowed which checks if print is functional at the moment
    CommandPrompt::print("Is printing allowed:");
    CommandPrompt::print(std::to_string(CommandPrompt::isPrintAllowed()));

    // allowPrint changes if printing is allowed
    CommandPrompt::allowPrint(false);
    CommandPrompt::print("This will not be printed");
    CommandPrompt::allowPrint(true);

    CommandPrompt::print("To open the prompt click \"~\"");
    CommandPrompt::print("To get a list of commands type \"help\"");

    // everything else about the command prompt is used during runtime
    // any command that is added to the CommandHandler will but in the CommandPrompt
    // There is command suggestions and command history
}
