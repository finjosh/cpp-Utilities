#include "Utils/CommandHandler.hpp"
#include <sstream>
#include <iostream>

using namespace Command;

void Command::print(const std::string& str, Data* input)
{
    input->setReturnStr(str);
}

void Command::helpCommand(const std::string& name, Data* data)
{
    data->setReturnStr("Use \"help " + name + "\" for more infomation");
}

std::vector<std::string> Command::parseTokens(const std::string& command)
{
    std::vector<std::string> tokens;
    std::stringstream sstr(command);
    std::string str;

    while (sstr >> str)
    {
        tokens.push_back(str);
    }

    return tokens;
}

// * Command Data

// * Command::Data::Info

void Command::Data::Info::addInfo(const std::string& info)
{
    this->m_info.emplace_back(info);
}

void Command::Data::Info::addInfo(const std::list<std::string>& info)
{
    this->m_info.insert(this->m_info.end(), info.begin(), info.end());
}

const std::list<std::string>& Command::Data::Info::getInfo() const
{
    return this->m_info;
}

bool Command::Data::Info::hasInfo() const
{
    return !m_info.empty();
}

// * -------------------

Data::Data(const std::string& str)
{
    this->m_tokens = parseTokens(str);
}

void Data::setTokens(const std::string& str)
{
    this->m_tokens = parseTokens(str);
}

void Data::setTokens(const std::vector<std::string>& tokens)
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

const std::vector<std::string>& Data::getTokens() const
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

std::string Data::getFirstToken() const
{
    if (this->m_tokens.size() > 0)
        return this->m_tokens[0];
    return "";
}

std::string Data::getLastToken() const
{
    if (this->m_tokens.size() > 0)
        return this->m_tokens[this->m_tokens.size()-1];
    return "";
}

size_t Data::getNumTokens() const
{
    return this->m_tokens.size();
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

void Data::parseCommandInput(Command::Handler& context)
{
    size_t tIndex = this->getNumTokens();
    while (tIndex > 0 && !this->hasErrors())
    {
        tIndex--;
        std::string token = this->getToken(tIndex);
        // checking if we are calling commands for the token data
        if (token.starts_with("$("))
        {
            size_t startOfCommand = tIndex;
            std::string command = token.substr(2);
            token = this->getToken(++tIndex);
            while (!token.ends_with(')') && token != "")
            {
                command += " " + token;
                token = this->getToken(++tIndex);
            }
            // once token ends with ')' add it to the end of the command
            if (token != "")
            {
                // remove the bracket and add to the command
                command += " " + token.substr(0, token.size()-1);
            }
            else
            {
                // this is not an error as we can still call the comands
                this->addWarning(WARNING_COLOR + "Warning" + END_COLOR + " - Missing closing bracket for \"" + command + "\" starting at input token " + std::to_string(startOfCommand+1));
            }

            Data returnData = context.invokeCommand(command);
            // adding the errors and warnings from the return data
            this->addErrors(returnData.getErrors());
            this->addWarnings(returnData.getWarnings());

            // adding the return string to the tokens if there where no errors
            if (!returnData.hasErrors())
            {
                std::vector<std::string> returnTokenized = Command::parseTokens(returnData.getReturnStr());
                this->m_tokens.erase(this->m_tokens.begin()+startOfCommand, this->m_tokens.begin()+tIndex + (tIndex < m_tokens.size() ? 1 : 0)); // removing all the tokens we have used
                this->m_tokens.insert(this->m_tokens.begin()+startOfCommand, returnTokenized.begin(), returnTokenized.end()); // adding the data from the input command
                tIndex = startOfCommand; // setting tIndex to the end of the input commands return tokens
            }
        }
    }
}

bool Command::Data::hasErrors() const
{
    return m_errors.hasInfo();
}

bool Command::Data::hasWarnings() const
{
    return m_warnings.hasInfo();
}

const std::list<std::string>& Command::Data::getWarnings() const
{
    return m_warnings.getInfo();
}

const std::list<std::string>& Command::Data::getErrors() const
{
    return m_errors.getInfo();
}

void Command::Data::addWarning(const std::string& warning)
{
    m_warnings.addInfo(warning);
}

void Command::Data::addWarnings(const std::list<std::string>& warnings)
{
    m_warnings.addInfo(warnings);
}

void Command::Data::addError(const std::string& error)
{
    m_errors.addInfo(error);
}

void Command::Data::addErrors(const std::list<std::string>& errors)
{
    m_errors.addInfo(errors);
}

// * Definition

Command::Definition::Definition(const std::string& description, 
                                const funcHelper::funcDynamic<Data*>& func, 
                                const std::set<std::string>& possibleInputs,
                                const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands) :
    m_description(description), m_function(func), m_commands(scopedCommands), m_possibleInputs(possibleInputs) {}

Command::Definition::Definition(const Command::Definition& command) :
    m_description(command.m_description), m_function(command.m_function), m_commands(command.m_commands), m_possibleInputs(command.m_possibleInputs)
{}

Command::Definition& Command::Definition::setDescription(const std::string& description)
{
    this->m_description = description;
    return *this;
}

Command::Definition& Command::Definition::setFunction(const funcHelper::funcDynamic<Data*>& func)
{
    this->m_function = func;
    return *this;
}

Command::Definition& Command::Definition::setCommands(const std::map<std::string, Command::Definition, Command::Command_Compare>& subCommands)
{
    this->m_commands = subCommands;
    return *this;
}

Command::Definition& Command::Definition::addCommand(const std::string& name, const Command::Definition& command, bool replace)
{
    this->addCommand(name, command.m_description, command.m_function, command.m_possibleInputs, command.m_commands, replace);
    return *this;
}

Command::Definition& Command::Definition::addCommand(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func, 
                                                   const std::set<std::string>& possibleInputs,
                                                   const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands, bool replace)
{
    auto iter = m_commands.find(name);
    if (iter != m_commands.end())
    {
        if (replace)
        {
            m_commands.erase(iter);
        }
        else // we need to add only the sub commands that do not exist 
        {
            // adding the sub commands that do not exist by seting replace to false
            for (auto commandPair: scopedCommands)
            {
                iter->second.addCommand(commandPair.first, commandPair.second, false);
            }
            return *this;
        }
    }

    // emplace does not replace values if the key already exists
    m_commands.emplace(name, Command::Definition{description, func, possibleInputs, scopedCommands});

    return *this;
}

Command::Definition& Command::Definition::setPossibleInputs(const std::set<std::string>& possibleInputs)
{
    this->m_possibleInputs = possibleInputs;
    return *this;
}

Command::Definition& Command::Definition::addPossibleInput(const std::string& input)
{
    m_possibleInputs.insert(input);
    return *this;
}

std::string Command::Definition::getDescription() const
{
    return this->m_description;
}

std::string Command::Definition::getNameDescription(const std::string& name, size_t maxLength, int tabs) const
{
    //! this is not going to work too good unless the font is monospaced
    std::string tabsStr = TAB_STR;
    for (size_t i = 0; i < 2 + name.length(); i++)
        tabsStr += " ";
    for (int i = 0; i < tabs; i++) 
        tabsStr += TAB_STR;

    std::string rtn = Command::BOLD_FONT + name + Command::END_BOLD_FONT + " - " + this->m_description.substr(0, maxLength) + (this->m_description.size() > maxLength ? "..." : "");

    size_t pos = 0;
    while ((pos = rtn.find('\n', pos)) != std::string::npos) {
        rtn.replace(pos, 1, "\n" + tabsStr);
        pos += tabsStr.length();
    }
    return rtn;
}

std::string Command::Definition::getScopedNameDescriptions(size_t maxLineLength, size_t commandIndex) const
{
    std::string rtn;
    // iter.first is the name of the command
    // iter.second is the command
    for (auto iter: this->m_commands)
    {
        for (size_t i = 0; i < commandIndex; i++)
        {
            rtn += TAB_STR;
        }
        rtn += "~ " + iter.second.getNameDescription(iter.first, maxLineLength) + "\n";
        rtn += iter.second.getScopedNameDescriptions(maxLineLength, ++commandIndex);
        --commandIndex;
    }
    return rtn;
}

std::string Command::Definition::getFullNameDescription(const std::string& name, size_t maxLineLength, size_t commandIndex) const
{
    std::string rtn = "~ ";
    rtn += this->getNameDescription(name, maxLineLength, commandIndex);
    rtn += "\n" + this->getScopedNameDescriptions(maxLineLength, commandIndex);
    return rtn;
}

const std::set<std::string>& Command::Definition::getPossibleInputs() const
{
    return m_possibleInputs;
}

const std::map<std::string, Command::Definition, Command::Command_Compare>& Command::Definition::getCommands() const
{
    return m_commands;
}

const Command::Definition* Command::Definition::findCommand(const std::string& command) const
{
    // Delegate to the non-const version using const_cast
    // This is safe because we're not modifying the object, just reusing the logic
    return const_cast<Command::Definition*>(this)->findCommand(command);
}

Command::Definition* Command::Definition::findCommand(const std::string& command)
{
    auto tokens = Command::parseTokens(command);
    if (tokens.size() == 0)
        return nullptr;
    return this->findCommand(tokens);
}

const Command::Definition* Command::Definition::findCommand(const std::vector<std::string>& path) const
{
    return this->findCommand(path.begin(), path.end());
}

Command::Definition* Command::Definition::findCommand(const std::vector<std::string>& path)
{
    return this->findCommand(path.begin(), path.end());
}

const Command::Definition* Command::Definition::findCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath) const
{
    // Delegate to the non-const version using const_cast
    // This is safe because we're not modifying the object, just reusing the logic
    return const_cast<Command::Definition*>(this)->findCommand(beginPath, endPath);
}

Command::Definition* Command::Definition::findCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath)
{
    Command::Definition* lastValidCommand = this;

    auto iter = lastValidCommand->m_commands.begin();
    while (iter != lastValidCommand->m_commands.end() && beginPath != endPath)
    {
        iter = lastValidCommand->m_commands.find(*beginPath);
        if (iter != lastValidCommand->m_commands.end())
        {
            lastValidCommand = &iter->second;
            beginPath++;
        }
        else
        {
            break;
        }
    }

    return beginPath != endPath ? nullptr : lastValidCommand;
}

std::pair<std::string, const Command::Definition*> Command::Definition::findClosestCommand(const std::string& command) const
{
    // Delegate to the non-const version using const_cast
    // This is safe because we're not modifying the object, just reusing the logic
    return const_cast<Command::Definition*>(this)->findClosestCommand(command);
}

std::pair<std::string, Command::Definition*> Command::Definition::findClosestCommand(const std::string& command)
{
    auto tokens = Command::parseTokens(command);
    if (tokens.size() == 0)
        return {"", nullptr};
    return this->findClosestCommand(tokens);
}

std::pair<std::string, const Command::Definition*> Command::Definition::findClosestCommand(const std::vector<std::string>& tokens) const
{
    return this->findClosestCommand(tokens.begin(), tokens.end());
}

std::pair<std::string, Command::Definition*> Command::Definition::findClosestCommand(const std::vector<std::string>& tokens)
{
    return this->findClosestCommand(tokens.begin(), tokens.end());
}

std::pair<std::string, const Command::Definition*> Command::Definition::findClosestCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath) const
{
    // Delegate to the non-const version using const_cast
    // This is safe because we're not modifying the object, just reusing the logic
    return const_cast<Command::Definition*>(this)->findClosestCommand(beginPath, endPath);
}

std::pair<std::string, Command::Definition*> Command::Definition::findClosestCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath)
{
    Command::Definition* lastValidCommand = this;
    std::string command;

    auto iter = lastValidCommand->m_commands.begin();
    while (iter != lastValidCommand->m_commands.end() && beginPath != endPath)
    {
        iter = lastValidCommand->m_commands.find(*beginPath);
        if (iter != lastValidCommand->m_commands.end())
        {
            lastValidCommand = &iter->second;
            command += *beginPath + " ";
            beginPath++;
        }
        else
        {
            break;
        }
    }

    if (command.size() > 0)
        command.erase(command.size()-1); // removing the last space
    return {command, lastValidCommand};
}


Command::Definition* Command::Definition::removeAllCommands()
{
    m_commands.clear();
    return this;
}

Command::Definition* Command::Definition::removeCommand(const std::string& command)
{
    auto tokens = Command::parseTokens(command);
    this->removeCommand(tokens);
    return this;
}

void Command::Definition::removeCommand(const std::vector<std::string>& commandPath)
{
    this->removeCommand(commandPath.begin(), commandPath.end());
}

void Command::Definition::removeCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath)
{
    // search the path first
    auto second = beginPath;
    second++;
    if (second != endPath)
    {
        auto iter = m_commands.find(beginPath.operator*());
        if (iter != m_commands.end())
            iter->second.removeCommand(second, endPath);
    }
    else
    {
        auto iter = m_commands.find(beginPath.operator*());
        if (iter != m_commands.end())
            m_commands.erase(iter);
    }
}

void Command::Definition::invoke(Command::Handler& context, Command::Data& data) const
{
    data.parseCommandInput(context);
    this->m_function.invoke(&data);
}

std::list<std::string> Command::Definition::autoFillSearch(const std::string& search)
{
    std::list<std::string> rtn;
    std::vector<std::string> tokens;
    size_t curToken = 0;
    
    // checking if there are nested commands to autofill for instead of the entire command
    // finding the last $ in the string where its not "$$("
    size_t nestedCommandPos = search.find_last_of('$');
    while (nestedCommandPos >= 0 && nestedCommandPos < search.size() && nestedCommandPos != std::string::npos 
            && (search[nestedCommandPos-1] == '$' || search[nestedCommandPos+1] != '('))
    {
        nestedCommandPos = search.find_last_of('$', nestedCommandPos-1);
    }

    if (nestedCommandPos != std::string::npos)
    {
        tokens = Command::parseTokens(search.substr(nestedCommandPos+2));
        if (search.ends_with('('))
            tokens.push_back(""); // adding an empty token so the auto fill will work if there is a open bracket
    }
    else // if there is no sub command to auto fill
    {
        tokens = Command::parseTokens(search);
    }

    if (search.ends_with(' '))
        tokens.push_back(""); // adding an empty token so the auto fill will work if there is a space
    
    // skipping the help token so we autofill the command after it
    if (tokens.size() == 0)
        return rtn;
    else if (Command::equalStr(tokens[0], "help"))
    {
        curToken++;
        if (curToken >= tokens.size()) 
            return rtn; // checking if the only thing imputed was help
    }

    auto closestCommand = this->findClosestCommand(tokens.begin() + curToken, tokens.end() - 1);
    if (closestCommand.first.size() > 0)
        curToken++;
    for (char c: closestCommand.first)
    {
        if (c == ' ')
            curToken++;
    }
    
    if (tokens.back() == "")
    {
        for (auto i: closestCommand.second->getPossibleInputs())
        {
            rtn.emplace_back(i);
        }
    }

    for (auto& iter: closestCommand.second->getCommands())
    {
        if (Command::startsWith(iter.first, tokens[curToken]))
        {
            rtn.emplace_back(iter.first + " ");
        }
    }

    return rtn;
}

// * ----------

// * Command Handler

Command::Handler& Command::Handler::get()
{
    static Handler* threadPool = new Handler();
    return *threadPool;
}

void Command::Handler::addCommand(const std::string& name, const Command::Definition& command, bool replace)
{
    m_commands.addCommand(name, command, replace);
}

void Command::Handler::addCommand(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func, 
                                  const std::set<std::string>& possibleInputs,
                                  const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands, bool replace)
{
    m_commands.addCommand(name, description, func, possibleInputs, scopedCommands, replace);
}

void Command::Handler::removeAllCommands()
{
    m_commands.removeAllCommands();
}

void Command::Handler::removeCommand(const std::vector<std::string>& commandPath)
{
    m_commands.removeCommand(commandPath);
}

void Command::Handler::removeCommand(const std::string& command)
{
    m_commands.removeCommand(command);
}

Command::Definition* Command::Handler::findCommand(const std::string& command)
{
    return m_commands.findCommand(command);
}

Command::Definition* Command::Handler::findCommand(const std::vector<std::string>& commandPath)
{
    return m_commands.findCommand(commandPath);
}

void Command::Handler::setThreadSafeEvents(bool threadSafe)
{
    m_threadSafeEvents = threadSafe;
}

bool Command::Handler::isThreadSafeEvents()
{
    return m_threadSafeEvents;
}

Command::Data Command::Handler::invokeCommand(const std::string& commandStr)
{
    Command::Data input(commandStr);

    // doing early checks for either help command or no command being entered
    if (input.getNumTokens() == 0) 
    {
        input.setReturnStr(HELP_STRING);
        return input;
    }
    // returns the list of commands and there descriptions if help is entered
    else if (Command::equalStr(input.getToken(0), "help"))
    {
        // checking if command specific help
        if (input.getNumTokens() == 1)
        {
            // no command specific help
            std::string rtn = "help [Command] - can be used on every command to show it's full description and it's scoped commands if there are any\n\n";
            for (auto iter: m_commands.getCommands())
            {
                rtn += "~ " + iter.second.getNameDescription(iter.first, 64) + "\n";
            }
            input.setReturnStr(rtn);
            return input;
        }
        else
        {
            input.removeToken(0); // remove help from the tokens
            
            std::string rtn;
            const Command::Definition* cmd = m_commands.findCommand(input.getTokens());
            if (cmd == nullptr) 
            {
                input.setReturnStr("The given command could not be found"); // TODO make this a different color?
                input.addReturnStrLine(HELP_STRING);
                return input;
            }

            if (cmd != nullptr) // only if the entire command was found
            {
                rtn += "~ " + cmd->getNameDescription(input.getLastToken()) + "\n";
                rtn += cmd->getScopedNameDescriptions(64);
            }

            input.setReturnStr(rtn);    
        }
    }
    else
    {
        auto closestCommand = m_commands.findClosestCommand(input.getTokens());
        
        if (closestCommand.second != nullptr)
        {
            input.setTokens(input.getTokensStr().substr(closestCommand.first.size()));
            closestCommand.second->invoke(*this, input);
        }
    }
    
    if (m_commandHistory.front() != commandStr)
    {
        if (m_commandHistory.size() >= m_maxCommandHistory)
        {
            m_commandHistory.pop_back();
        }
        m_commandHistory.emplace_front(commandStr);
    }

    for (auto error: input.getErrors())
    {
        m_lineHistory.emplace_front(error);
    }
    for (auto warning: input.getWarnings())
    {
        m_lineHistory.emplace_front(warning);
    }
    m_lineHistory.emplace_front(StringHelper::trim_copy(commandStr));
    m_lineHistory.emplace_front(input.getReturnStr());
    if (m_lineHistory.size() > m_maxLineHistory)
        m_lineHistory.resize(m_maxLineHistory);

    return input;
}

size_t Command::Handler::getMaxCommandHistory()
{
    return m_maxCommandHistory;
}

size_t Command::Handler::getMaxLineHistory()
{
    return m_maxLineHistory;
}

void Command::Handler::setMaxCommandHistory(size_t size)
{
    m_maxCommandHistory = size;
    onMaxCommandHistoryChanged.invoke();
}

void Command::Handler::setMaxLineHistory(size_t size)
{
    m_maxLineHistory = size;
    onMaxLineHistoryChanged.invoke();
}

const std::list<std::string>& Command::Handler::getCommandHistory()
{
    return m_commandHistory;
}

const std::list<std::string>& Command::Handler::getCommandHistoryLines()
{
    return m_lineHistory;
}

// * ---------------
