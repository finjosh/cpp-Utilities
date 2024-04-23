#include "Utils/Debug/CommandHandler.hpp"

using namespace Command;

color Command::color::_default_text_color = color(0,0,0,255);

Command::color::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
    r(r), b(b), g(g), a(a)
{}

void Command::color::setDefaultColor(const color& color)
{
    _default_text_color = color;
}

color Command::color::getDefaultColor()
{
    return _default_text_color;
}

void Command::print(const std::string& str, Data* input)
{
    input->setReturnStr(str);
}

// * Command Data

Data::Data(const std::string& command)
{
    this->_tokens = parseCommand(command);
}

void Data::setCommand(const std::string& command)
{
    this->_tokens = parseCommand(command);
}

void Data::setTokens(const std::string& command)
{
    this->_tokens = parseCommand(command);
}

void Data::setTokens(const Tokens& tokens)
{
    this->_tokens = tokens;    
}

void Data::setToken(const size_t& index, const std::string& tokenStr)
{
    this->_tokens[index] = tokenStr;
}

void Data::addToken(const std::string& tokenStr)
{
    this->_tokens.push_back(tokenStr);
}

void Data::removeToken(const size_t& index)
{
    this->_tokens.erase(this->_tokens.begin() + index);
}

const Tokens& Data::getTokens()
{
    return this->_tokens;
}

const std::string Data::getToken(const size_t& index)
{
    if (index < this->_tokens.size())
        return this->_tokens[index];
    return "";
}

size_t Data::getNumOfTokens()
{
    return this->_tokens.size();
}

const color& Data::getReturnColor()
{
    return this->_color;
}

void Data::setReturnColor(const color& color)
{
    this->_color = color;
}

const std::string& Data::getReturnStr()
{
    return this->_return;
}

void Data::setReturnStr(const std::string& returnStr)
{
    this->_return = returnStr;
}

void Data::addReturnStrLine(const std::string& line)
{
    this->_return += "\n" + line;
}

void Data::addToReturnStr(const std::string& str)
{
    this->_return += str;
}

Tokens Data::parseCommand(const std::string& command)
{
    Tokens tokens;
    std::stringstream sstr(command);
    std::string str;

    while (sstr >> str)
    {
        tokens.push_back(str);
    }

    return tokens;
}

void Data::deepParseCommand()
{
    size_t i = 0;
    while (i < this->getNumOfTokens())
    {
        std::string token = this->getToken(i);
        // checking if we are calling commands for the token data
        if (token.starts_with('('))
        {
            size_t startOfSub = i;
            std::string command = token.substr(1);
            this->removeToken(i);
            token = this->getToken(i);
            while (!token.ends_with(')') && token != "")
            {
                command += " " + token;
                this->removeToken(i);
                token = this->getToken(i);
            }
            // once token ends with ')' add it to the end of the command
            if (token != "")
            {
                this->removeToken(i);
                token.erase(token.size()-1);
                command += " " + token;
            }
            Tokens temp = this->parseCommand(Command::Handler::callCommand(command).getReturnStr());
            // this->_tokens.erase(this->_tokens.begin()+startOfSub,this->_tokens.begin()+i); // removing all the tokens we have used
            this->_tokens.insert(this->_tokens.begin()+startOfSub, temp.begin(), temp.end()); // adding the data from the sub command
            i = startOfSub + temp.size()-1; // setting i to the end of the sub commands return tokens
        }
        i++;
    }
}

// * Command

command::command(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func, const std::list<command>& subCommands) :
    _name(name), _description(description), _function(func), _subCommands(subCommands)
{
    _subCommands.sort();
}

command::command(const command& command) :
    _name(command.getName()), _description(command.getDescription()), _function(command._function), _subCommands(command._subCommands)
{}

bool command::operator< (const command& command) const
{
    return this->operator<(command.getName());
}

bool command::operator< (const std::string& str) const
{
    return this->getName() < str;
}

bool command::operator> (const command& command) const
{
    return this->operator>(command.getName());
}

bool command::operator> (const std::string& str) const
{
    return this->getName() > str;
}

bool command::operator== (const command& command) const
{
    return this->operator==(command.getName());
}

bool command::operator== (const std::string& str) const
{
    return this->getName() == str;
}

std::string command::getName() const
{
    return this->_name;
}

std::string command::getDescription() const
{
    return this->_description;
}

std::string command::getNameDescription(const size_t& maxLength) const
{
    return this->_name + " - " + this->_description.substr(0, maxLength) + (this->_description.size() > maxLength ? "..." : "");
}

std::string command::getSubCommandsNameDescription(const size_t& maxLength, size_t subCommandIndex) const
{
    std::string rtn;
    std::for_each(_subCommands.begin(), _subCommands.end(), [&rtn, &maxLength, &subCommandIndex](const command& cmd)
    {
        for (size_t i = 0; i < subCommandIndex; i++)
        {
            rtn += TAB_STR;
        }
        rtn += "~ " + cmd.getNameDescription() + "\n";
        rtn += cmd.getSubCommandsNameDescription(maxLength, ++subCommandIndex);
        --subCommandIndex;
    });
    return rtn;
}

void command::invoke(Data& data)
{
    this->_function.invoke(&data);
}

// * -------

// * Command Handler

void Handler::addCommand(const command& command)
{
    _addCommand(command, _commands);
}

void Handler::_addCommand(const command& cmd, std::list<command>& _commands)
{
    auto lastCommand = std::find(_commands.begin(), _commands.end(), cmd);
    // if the command is not already in the list add it
    if (lastCommand == _commands.end())
    {
        _commands.push_back(cmd);
        _commands.sort();
        return;
    }

    // if the command is in the list try adding its sub commands to the current ones sub commands
    for (auto subCommand: cmd._subCommands)
    {
        _addCommand(subCommand, lastCommand->_subCommands);
    }
}

std::list<std::string> Handler::autoFillSearch(const std::string& search)
{
    bool foundCommand = false; // set to true if a command that partially matches was found
    std::list<std::string> rtn;
    Data input("");
    
    // if we want to auto fill sub command instead of the whole command
    size_t subCommandPos = search.find_last_of('(');
    if (subCommandPos != std::string::npos)
    {
        input.setCommand(StringHelper::toLower_copy(search.substr(subCommandPos+1)));
        if (search.ends_with(" ") || search.ends_with("("))
            input.addToken(""); // adding an empty token so the auto fill will work if there is a space 
    }
    else // if there is no sub command to auto fill
    {
        input.setCommand(StringHelper::toLower_copy(search));
    }

    if (search.ends_with(" "))
        input.addToken(""); // adding an empty token so the auto fill will work if there is a space 
    if (input.getNumOfTokens() == 0) return rtn;
    else if (input.getToken(0) == "help") 
    {
        input.removeToken(0); // auto filling for searching by removing the help token
        if (input.getNumOfTokens() == 0) return rtn; // checking if the only thing imputed was help
    }

    std::list<command>::iterator commandsIter = _commands.begin();
    std::list<command>* commandsList = &_commands;

    // finding which list to find auto fill for
    {
        std::list<command>* tempList = &_commands;
        std::list<command>::iterator i = commandsIter;
        while (true)
        {
            i = std::find_if(tempList->begin(), tempList->end(), [&input](const Command::command& v){return StringHelper::toLower_copy(v.getName()) == input.getToken(0);});
            if (i != tempList->end() && input.getNumOfTokens() > 1)
            {
                tempList = &i->_subCommands;
                input.removeToken(0);
            }
            else
            {
                break;
            }
        }
        commandsIter = tempList->begin();
        commandsList = tempList;
    }

    for (std::list<command>::iterator i = commandsList->begin(); i != commandsList->end(); i++)
    {
        if (StringHelper::toLower_copy(i->getName()).starts_with(input.getToken(0)))
        {
            foundCommand = true;
            rtn.push_back(i->getName() + " ");
        }
        else if (foundCommand)
        {
            break;
        }
    }

    return rtn;
}

Data Handler::callCommand(const std::string& commandStr)
{
    Data input(commandStr);

    // doing early checks for either help command or no command being entered
    if (input.getNumOfTokens() == 0) 
    {
        input.setReturnStr("Try using \"help\" for a list of commands");
        return input;
    }
    // returns the list of commands and there descriptions if help is entered
    else if (input.getToken(0) == "help") 
    {
        // checking if command specific help
        if (input.getNumOfTokens() == 1)
        {
            // no command specific help
            std::string rtn = "help [Command] - can be used on every command to show it's full description and it's sub commands if there are any\n\n";
            std::for_each(_commands.begin(), _commands.end(), [&rtn](const command& cmd){ rtn += "~ " + cmd.getNameDescription(50) + "\n"; });
            input.setReturnStr(rtn);
            return input;
        }
        else
        {
            input.removeToken(0); // remove help from the tokens
            
            std::string rtn;
            std::list<command>::iterator curCommand = std::find(_commands.begin(), _commands.end(), input.getToken(0));
            
            if (curCommand == _commands.end()) 
            {
                input.setReturnStr("The given command could not be found\nTry using \"help\" for a list of commands");
                return input;
            }

            input.removeToken(0); // remove the current command from the tokens to find check if there is another to look for
            while (input.getNumOfTokens() > 0 || curCommand->_subCommands.size() == 0)
            {
                auto temp = std::find(curCommand->_subCommands.begin(), curCommand->_subCommands.end(), input.getToken(0));

                if (temp == curCommand->_subCommands.end()) break;

                curCommand = temp;
                input.removeToken(0);
            }

            if (input.getNumOfTokens() == 0) // only if every token was found display the info for the command and its sub commands
            {
                rtn += "~ " + curCommand->getNameDescription() + "\n";
                rtn += curCommand->getSubCommandsNameDescription(50);
            }

            input.setReturnStr(rtn);           
            return input; // returning the string of commands in the color white
        }
    }

    std::list<command>::iterator curCommand = std::find(_commands.begin(), _commands.end(), input.getToken(0));
    if (curCommand == _commands.end()) 
    {
        input.setReturnStr("Try using \"help\" for a list of commands");
        return input;
    }

    input.removeToken(0); // remove the current command from the tokens to find check if there is another to look for
    while (input.getNumOfTokens() > 0 || curCommand->_subCommands.size() == 0)
    {
        auto temp = std::find(curCommand->_subCommands.begin(), curCommand->_subCommands.end(), input.getToken(0));

        if (temp == curCommand->_subCommands.end()) break;

        curCommand = temp;
        input.removeToken(0);
    }

    input.deepParseCommand(); // parsing the command to check if there are any subCommands that need to be called for input first
    color printColor;
    curCommand->invoke(input);

    return input;
}

std::list<command> Handler::_commands;

// * ---------------
