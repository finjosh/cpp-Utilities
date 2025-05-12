#include "include/Examples/CommandHandler.hpp"
#include <math.h>

// TODO test addSubCommand, removeAllCommands, removeCommand
void CommandHandlerTest::test()
{
    using namespace std;

    //! Depends on FuncHelper and StringHelper
    cout << "Outputs for CommandHandler: " << endl;

    // Note that command handler is almost pointless unless paired with the command prompt

    // There are some pre-made colors which might be useful
    // Command::WARNING_COLOR;
    // Command::ERROR_COLOR;
    // Command::INVALID_INPUT_COLOR;

    // With the command handler each part of a command is a "Token"
    // i.e. the command: "cp getRandom 5 7" has 4 tokens which are "cp", "getRandom", "5", "7"
    // Note that we can have commands in commands
    // this is done by putting the second command in round brackets with a $ in front of them
    // i.e "cp getRandom $(cp getRandom 5 7) 10"

    //* Next we have the Data class
    // This is the data which a command is able to take in when its called
    // The main point is that you can get tokens that are for input to the command to do stuff
    // you can also set the return string and color
    // eg. the command "cp getRandom 7 100" would get data that contains the tokens "7" and "100"
    // usage of this class will be covered more in later examples

    // First note that isValidInput is quite useful when creating commands
    // it tries to convert the given string to the value wanted
    //      - int, float, bool, unsigned int, unsigned long
    int value;
    if (!Command::isValidInput<int>("12546573", value, 0))
    {
        cout << "Not a valid int" << endl;
    }

    // print function
    // works with commands you just need to supply the string it will output
    // eg.
    Command::Definition someCommand1 = Command::Definition("Some description", funcHelper::funcDynamic<Command::Data*>(Command::print, "Something to print"));
    Command::Definition someCommand2 = Command::Definition("Some description", funcHelper::funcDynamic<Command::Data*>(Command::print, to_string(rand())));

    //* Commands
    // Examples of creating commands
    Command::Definition("A description for the command", /*The function*/{Command::print, "This is a command"}, {},
        /*Scoped/Sub commands*/ {{"Scoped Command Name", Command::Definition("It's description", {Command::print, "A sub command"})}});
        Command::Definition roundCommand = Command::Definition("Rounds the given float", {[](Command::Data* data)
    {
        float value;
        if (!Command::isValidInput<float>(data->getToken(0), value, 0))
        {
            data->setReturnStr("Invalid float entered");
            return;
        }
        data->setReturnStr(to_string(round(value)));
    }});

    //* CommandHandler
    // Adding commands to the CommandHandler
    Command::Handler::get().addCommand("round", roundCommand);

    // Calling a command
    cout << "Calling the round command" << endl;
    cout << Command::Handler::get().invokeCommand("round 7.5").getReturnStr() << endl;

    // finding all commands that start with the given string
    cout << "All commands that start with \"rou\":" << endl;
    for (string s: Command::Handler::get().autoFillSearch("rou"))
    {
        cout << s << endl;
    }

    // finding all commands that start with the given string
    cout << "All commands that start with \"ro\":" << endl;
    for (string s: Command::Handler::get().autoFillSearch("ro"))
    {
        cout << s << endl;
    }

    Command::Handler::get()
    .addCommand("getRandom", "[min = 0] [max = " + std::to_string(RAND_MAX) + " ] [amount = 1]" +
                        " | prints n random numbers with a total max of " + std::to_string(RAND_MAX) + " and total min of 0",
        [](Command::Data* data)
        {
            int min = 0;
            int max = RAND_MAX;

            if (data->getNumTokens() > 0 && (!Command::isValidInput<int>(data->getToken(0), min, 0) || min < 0))
            {
                data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid min entered (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")");
                return;
            }
            if (data->getNumTokens() > 1 && (!Command::isValidInput<int>(data->getToken(1), max, RAND_MAX) || max < min || max > RAND_MAX))
            {
                data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid max entered (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")");
                return;
            }

            unsigned long amount = 1;
            if (data->getNumTokens() == 3 && (!Command::isValidInput<unsigned long>(data->getToken(2), amount, 1) || amount < 1))
            {
                data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid amount entered (amount=" + std::to_string(amount) + ")");
            }

            while (amount != 0)
            {
                data->addToReturnStr(std::to_string((min + (rand())%(max+1 - min))) + " ");
                amount--;
            }
        });

    std::cout << "Calling a command with nested commands as input" << std::endl;
    std::cout << Command::Handler::get().invokeCommand("getRandom $(getRandom 0 10) $(getRandom 10 100) 100").getReturnStr() << std::endl;

    std::cout << "Calling a command with nested commands as input - including bad syntax that does not cause an error" << std::endl;
    auto data = Command::Handler::get().invokeCommand("getRandom $(getRandom 0 10) $(getRandom 10 100");
    for (auto error: data.getErrors())
    {
        std::cout << error << std::endl;
    }
    for (auto warning: data.getWarnings())
    {
        std::cout << warning << std::endl;
    }
    std::cout << data.getReturnStr() << std::endl;

    std::cout << "Calling a command with nested commands as input - including bad syntax that does not cause an error" << std::endl;
    // Note that nested commands are evaluated from right to left so first "$(getRandom 10 100" is called then "$(getRandom 0 10" is called with the result of "$(getRandom 10 100" as input
    data = Command::Handler::get().invokeCommand("getRandom $(getRandom 0 10 $(getRandom 10 100");
    for (auto error: data.getErrors())
    {
        std::cout << error << std::endl;
    }
    for (auto warning: data.getWarnings())
    {
        std::cout << warning << std::endl;
    }
    std::cout << data.getReturnStr() << std::endl;
}
