#include "Utils/CommandHandler.hpp"

using namespace Command;

color Command::color::_default_text_color = color(0,0,0,255);

Command::color::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
    r(r), b(b), g(g), a(a)
{}

void Command::color::setDefaultColor(color color)
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

void Command::helpCommand(const std::string& name, Data* data)
{
    data->setReturnStr("Use \"help " + name + "\" for more infomation");
}

// * Command Data

Data::Data(const std::string& command)
{
    this->m_tokens = parseCommand(command);
}

void Data::setCommand(const std::string& command)
{
    this->m_tokens = parseCommand(command);
}

void Data::setTokens(const std::string& command)
{
    this->m_tokens = parseCommand(command);
}

void Data::setTokens(const Tokens& tokens)
{
    this->m_tokens = tokens;    
}

void Data::setToken(size_t index, const std::string& tokenStr)
{
    this->m_tokens[index] = tokenStr;
}

void Data::addToken(const std::string& tokenStr)
{
    this->m_tokens.push_back(tokenStr);
}

void Data::removeToken(size_t index)
{
    this->m_tokens.erase(this->m_tokens.begin() + index);
}

const Tokens& Data::getTokens() const
{
    return this->m_tokens;
}

std::string Data::getTokensStr(size_t begin, size_t end) const
{
    std::string rtn;
    size_t last = end <= this->getNumTokens()-1 ? end : this->getNumTokens()-1;
    for (size_t i = begin; i <= last; i++)
    {
        rtn += this->getToken(i) + " ";
    }
    rtn.erase(rtn.size()-1);
    return rtn;
}

std::string Data::getToken(size_t index) const
{
    if (index < this->m_tokens.size())
        return this->m_tokens[index];
    return "";
}

size_t Data::getNumTokens() const
{
    return this->m_tokens.size();
}

color Data::getReturnColor() const
{
    return this->m_color;
}

void Data::setReturnColor(color color)
{
    this->m_color = color;
}

const std::string& Data::getReturnStr() const
{
    return this->m_return;
}

void Data::setReturnStr(const std::string& returnStr)
{
    this->m_return = returnStr;
}

void Data::addReturnStrLine(const std::string& line)
{
    this->m_return += "\n" + line;
}

void Data::addToReturnStr(const std::string& str)
{
    this->m_return += str;
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
    while (i < this->getNumTokens())
    {
        std::string token = this->getToken(i);
        // checking if we are calling commands for the token data
        if (token.starts_with('('))
        {
            size_t startOfSub = i;
            std::string command = token.substr(1);
            i++;
            token = this->getToken(i);
            while (!token.ends_with(')') && token != "")
            {
                command += " " + token;
                i++;
                token = this->getToken(i);
            }
            i--; // removing the extra index added
            // once token ends with ')' add it to the end of the command
            if (token != "")
            {
                i++;
                token.erase(token.size()-1);
                command += " " + token;
            }
            if (Command::Handler::isCommand(command))
            {
                Tokens temp = Command::Data::parseCommand(Command::Handler::callCommand(command).getReturnStr());
                this->m_tokens.erase(this->m_tokens.begin()+startOfSub,this->m_tokens.begin()+i); // removing all the tokens we have used
                this->m_tokens.insert(this->m_tokens.begin()+startOfSub, temp.begin(), temp.end()); // adding the data from the sub command
                i = startOfSub + temp.size()-1; // setting i to the end of the sub commands return tokens
            }
            else
            {
                i = startOfSub + Command::Data::parseCommand(command).size();
            }
        }
        i++;
    }
}

// * Command

command::command(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func, const std::list<std::string>& possibleInputs, const std::list<command>& subCommands) :
    m_name(name), m_description(description), m_function(func), m_subCommands(subCommands), m_possibleInputs(possibleInputs)
{
    m_subCommands.sort();
    m_possibleInputs.sort();
}

command::command(const command& command) :
    m_name(command.getName()), m_description(command.getDescription()), m_function(command.m_function), m_subCommands(command.m_subCommands), m_possibleInputs(command.m_possibleInputs)
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
    return this->m_name;
}

std::string command::getDescription() const
{
    return this->m_description;
}

std::string command::getNameDescription(size_t maxLength) const
{
    return this->m_name + " - " + this->m_description.substr(0, maxLength) + (this->m_description.size() > maxLength ? "..." : "");
}

std::string command::getSubCommandsNameDescription(size_t maxLength, size_t subCommandIndex) const
{
    std::string rtn;
    std::for_each(m_subCommands.begin(), m_subCommands.end(), [&rtn, &maxLength, &subCommandIndex](const command& cmd)
    {
        for (size_t i = 0; i < subCommandIndex; i++)
        {
            rtn += TAB_STR;
        }
        rtn += "~ " + cmd.getNameDescription(maxLength) + "\n";
        rtn += cmd.getSubCommandsNameDescription(maxLength, ++subCommandIndex);
        --subCommandIndex;
    });
    return rtn;
}

const std::list<std::string>& Command::command::getPossibleInputs() const
{
    return m_possibleInputs;
}

void command::invoke(Data& data)
{
    this->m_function.invoke(&data);
}

// * -------

// * Command Handler

void Command::Handler::addCommand(const command& command, bool replace)
{
    m_addCommand(command, m_commands, replace);
}

void Command::Handler::m_addCommand(const command& cmd, std::list<command>& m_commands, bool replace)
{
    auto lastCommand = std::find(m_commands.begin(), m_commands.end(), cmd);
    // if the command is not already in the list add it
    if (lastCommand == m_commands.end())
    {
        m_commands.push_back(cmd);
        m_commands.sort();
        return;
    }

    if (replace)
    {
        m_commands.erase(lastCommand);
        m_commands.push_back(cmd);
        m_commands.sort();
        return;
    }
    // if the command is in the list try adding its sub commands to the current ones sub commands
    for (auto subCommand: cmd.m_subCommands)
    {
        m_addCommand(subCommand, lastCommand->m_subCommands, replace);
    }
}

bool Command::Handler::addSubCommand(const std::vector<std::string>& commandPath, const command& command, bool replace)
{
    Command::command* cmd = m_getCommand(commandPath);
    if (cmd == nullptr)
        return false;

    m_addCommand(command, cmd->m_subCommands, replace);

    return true;
}

bool Command::Handler::addSubCommand(const std::string& strCommand, const command& command, bool replace)
{
    return addSubCommand(Command::Data(strCommand).getTokens(), command, replace);
}

void Command::Handler::removeAllCommands()
{
    m_commands.clear();
    onAllCommandsRemoved.invoke(m_threadSafeEvents);
}

bool Command::Handler::removeCommand(const std::vector<std::string>& commandPath)
{
    std::list<command>* lastList = &m_commands;
    std::list<command>* curList = &m_commands;
    std::list<command>::iterator curCommand = m_commands.end();
    size_t i = 0;
    while (commandPath.size() > i)
    {
        curCommand = std::find_if(curList->begin(), curList->end(), [&commandPath, &i](const command& cmd){ return cmd.getName() == commandPath[i]; });
        if (curCommand == curList->end())
            return false;
        else
        {
            i++;
            lastList = curList;
            curList = &(curCommand->m_subCommands);
        }
    }

    if (i != 0)
        lastList->erase(curCommand);

    return true;
}

bool Command::Handler::removeCommand(const std::string& strCommandPath)
{
    return Command::Handler::removeCommand(Command::Data(strCommandPath).getTokens());
}

command* Command::Handler::m_getCommand(const std::vector<std::string>& commandPath, std::list<command>* list)
{
    std::list<command>::iterator curCommand = m_commands.end();
    size_t i = 0;
    while (commandPath.size() > i)
    {
        curCommand = std::find_if(list->begin(), list->end(), [&commandPath, &i](const command& cmd){ return cmd.getName() == commandPath[i]; });
        if (curCommand == list->end())
            return nullptr;

        i++;
        list = &(curCommand->m_subCommands);
    }
    return &(*curCommand);
}

void Command::Handler::setThreadSafeEvents(bool threadSafe)
{
    m_threadSafeEvents = threadSafe;
}

bool Command::Handler::isThreadSafeEvents()
{
    return m_threadSafeEvents;
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
            input.addToken(""); // adding an empty token so the auto fill will work if there is a space or a open bracket
    }
    else // if there is no sub command to auto fill
    {
        input.setCommand(StringHelper::toLower_copy(search));
    }

    if (search.ends_with(" "))
        input.addToken(""); // adding an empty token so the auto fill will work if there is a space 
    if (input.getNumTokens() == 0) return rtn;
    else if (input.getToken(0) == "help") 
    {
        input.removeToken(0); // auto filling for searching by removing the help token
        if (input.getNumTokens() == 0) return rtn; // checking if the only thing imputed was help
    }

    std::list<command>* commandsList = &m_commands;
    command* lastValidCommand = nullptr;

    // finding which list to find auto fill for
    {
        std::list<command>::iterator i = m_commands.begin();;
        while (true)
        {
            i = std::find_if(commandsList->begin(), commandsList->end(), [&input](const Command::command& v){return StringHelper::toLower_copy(v.getName()) == input.getToken(0);});
            if (i != commandsList->end() && input.getNumTokens() > 1)
            {
                commandsList = &i->m_subCommands;
                lastValidCommand = &(*i);
                input.removeToken(0);
            }
            else
            {
                break;
            }
        }    
    }
    
    if (lastValidCommand != nullptr && input.getToken(input.getNumTokens()-1) == "")
    {
        for (auto i: lastValidCommand->getPossibleInputs())
        {
            rtn.emplace_back(i);
        }
    }

    for (std::list<command>::iterator i = commandsList->begin(); i != commandsList->end(); i++)
    {
        if (StringHelper::toLower_copy(i->getName()).starts_with(input.getToken(0)))
        {
            rtn.push_back(i->getName() + " ");
        }
    }

    rtn.sort();

    return rtn;
}

Data Handler::callCommand(const std::string& commandStr)
{
    Data input(commandStr);

    // doing early checks for either help command or no command being entered
    if (input.getNumTokens() == 0) 
    {
        input.setReturnStr("Try using \"help\" for a list of commands");
        return input;
    }
    // returns the list of commands and there descriptions if help is entered
    else if (input.getToken(0) == "help") 
    {
        // checking if command specific help
        if (input.getNumTokens() == 1)
        {
            // no command specific help
            std::string rtn = "help [Command] - can be used on every command to show it's full description and it's sub commands if there are any\n\n";
            std::for_each(m_commands.begin(), m_commands.end(), [&rtn](const command& cmd){ rtn += "~ " + cmd.getNameDescription(64) + "\n"; });
            input.setReturnStr(rtn);
            return input;
        }
        else
        {
            input.removeToken(0); // remove help from the tokens
            
            std::string rtn;
            command* cmd = m_getCommand(input.getTokens());
            if (cmd == nullptr) 
            {
                input.setReturnStr("The given command could not be found\nTry using \"help\" for a list of commands");
                return input;
            }

            if (cmd != nullptr) // only if the entire command was found
            {
                rtn += "~ " + cmd->getNameDescription() + "\n";
                rtn += cmd->getSubCommandsNameDescription(64);
            }

            input.setReturnStr(rtn);           
            return input; // returning the string of commands in the color white
        }
    }

    std::list<command>::iterator curCommand = std::find(m_commands.begin(), m_commands.end(), input.getToken(0));
    if (curCommand == m_commands.end()) 
    {
        input.setReturnStr("Try using \"help\" for a list of commands");
        return input;
    }

    input.removeToken(0); // remove the current command from the tokens to check if there is another to look for
    while (input.getNumTokens() > 0 || curCommand->m_subCommands.size() == 0)
    {
        auto temp = std::find(curCommand->m_subCommands.begin(), curCommand->m_subCommands.end(), input.getToken(0));

        if (temp == curCommand->m_subCommands.end()) break;

        curCommand = temp;
        input.removeToken(0);
    }

    input.deepParseCommand(); // parsing the command to check if there are any subCommands that need to be called for input first
    color printColor;
    curCommand->invoke(input);

    return input;
}

bool Command::Handler::isCommand(const std::string& commandStr)
{
    Data input(commandStr);
    std::list<command>::iterator curCommand = std::find(m_commands.begin(), m_commands.end(), input.getToken(0));
    if (curCommand == m_commands.end()) 
    {
        return false;
    }

    return true;
}

std::list<command> Command::Handler::m_commands;
EventHelper::Event Command::Handler::onAllCommandsRemoved;
bool Command::Handler::m_threadSafeEvents = false;

// * ---------------
