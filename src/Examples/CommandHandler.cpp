#include "include/Examples/CommandHandler.hpp"

// TODO test addSubCommand, removeAllCommands, removeCommand
void CommandHandlerTest::test()
{
    using namespace std;

    //! Depends on FuncHelper and StringHelper
    cout << "Outputs for CommandHandler: " << endl;

    // Note that command handler is almost pointless unless paired with the command prompt

    // First some classes that are used by command handler
    //* First the color class is used for the wanted color of the output
    // this is primarily used by command prompt
    Command::color color(155,145,123,5);
    // There are some pre-made colors which might be useful
    // Command::WARNING_COLOR;
    // Command::ERROR_COLOR;
    // Command::INVALID_INPUT_COLOR;

    // With the command handler each part of a command is a "Token"
    // i.e. the command: "cp getRandom 5 7" has 4 tokens which are "cp", "getRandom", "5", "7"
    // Note that we can have commands in commands
    // this is done by putting the second command in round brackets
    // i.e "cp getRandom (cp getRandom 5 7) 10"

    //* Next we have the Data class
    // This is the data which a command is able to take in when its called
    // The main point is that you can get tokens that are for input to the command to do stuff
    // you can also set the return string and color
    // eg. the command "cp getRandom 7 100" would get data that contains the tokens "7" and "100"
    // usage of this class will be covered more in later examples

    // First note that isValidInput is quite useful when creating commands
    // it tries to convert the given string to the value wanted
    //      - int, float, bool, unsigned int, unsigned long
    // It will update the return string for the data given if the value is invalid
    Command::Data data;
    int value;
    if (!Command::isValidInput<int>("Not a valid int", data, "12546573", value, 0))
    {
        cout << data.getReturnStr() << endl;
    }

    // print function
    // works with commands you just need to supply the string it will output
    // eg.
    Command::command("SomeCommand", "Some description", funcHelper::funcDynamic<Command::Data*>(Command::print, "Something to print"));
    Command::command("SomeCommand", "Some description", funcHelper::funcDynamic<Command::Data*>(Command::print, to_string(rand())));

    //* Commands
    // Examples of creating commands
    Command::command("CommandName", "A description for the command", /*The function*/{Command::print, "This is a command"}, 
                    /*Sub commands*/ {Command::command("A sub Command", "It's description", {Command::print, "A sub command"})});
    auto roundCommand = Command::command("round", "Rounds the given float", {[](Command::Data* data)
    {
        float value;
        if (!Command::isValidInput<float>("Invalid float entered", *data, data->getToken(0), value, 0))
            return;
        data->setReturnStr(std::to_string(std::round(value)));
    }});

    //* CommandHandler
    // Adding commands to the CommandHandler
    Command::Handler::addCommand(roundCommand);

    // Calling a command
    cout << "Calling the round command" << endl;
    cout << Command::Handler::callCommand("round 7.5").getReturnStr() << endl;

    // finding all commands that start with the given string
    cout << "All commands that start with \"rou\":" << endl;
    for (string s: Command::Handler::autoFillSearch("rou"))
    {
        cout << s << endl;
    }
}
