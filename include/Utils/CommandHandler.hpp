#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#pragma once

#include "Utils/funcHelper.hpp"
#include "Utils/StringHelper.hpp"
#include "Utils/EventHelper.hpp"

#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>

namespace Command
{
/// @brief 4 space string
#define TAB_STR "    "
#define HELP_STRING "Try using \"help\" for a list of commands"
#define HELP_CMD_STRING(commandScope) "Try using \"help " + commandScope + "\" for more information"
/// @brief if commands should be converted to lower case while comparing then this should be defined
// #define CASE_SENSITIVE_COMMANDS

/// @brief Custom comparator for strings that ignores case if CASE_SENSITIVE_COMMANDS is not defined
/// @note used for map compare
struct Command_Compare
{
    bool operator()(const std::string& a, const std::string& b) const
    {
    #ifdef CASE_SENSITIVE_COMMANDS
        return a < b;
    #else
        for (size_t i = 0; i < std::min(a.size(), b.size()); i++)
        {
            char aChar = std::tolower(a[i]);
            char bChar = std::tolower(b[i]);
            if (aChar != bChar)
                return aChar < bChar;
        }
        return a.size() < b.size();
    #endif
    }
};

/// @note these are case sensitive if CASE_SENSITIVE_COMMANDS is defined
inline bool equalStr(const std::string& a, const std::string& b)
{
#ifdef CASE_SENSITIVE_COMMANDS
    return a == b;
#else
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < std::min(a.size(), b.size()); i++)
    {
        char aChar = std::tolower(a[i]);
        char bChar = std::tolower(b[i]);
        if (aChar != bChar)
            return false;
    }
    return true;
#endif
}

/// @note these are case sensitive if CASE_SENSITIVE_COMMANDS is defined
inline bool startsWith(const std::string& str, const std::string& prefix)
{
#ifdef CASE_SENSITIVE_COMMANDS
    return str.starts_with(prefix);
#else
    return StringHelper::toLower_copy(str).starts_with(StringHelper::toLower_copy(prefix));
#endif
}

// /// @note if you want a constructor from sf::Color make sure to include the CommandHandler include AFTER sfml color include
// struct color
// {
//     /// @brief default is the default color
//     color(uint8_t r = _default_text_color.r, uint8_t g = _default_text_color.g, uint8_t b = _default_text_color.b, uint8_t a = _default_text_color.a);
//     #ifdef SFML_COLOR_HPP
//     inline color(const sf::Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {};
//     #endif 

//     static void setDefaultColor(color color);

//     static color getDefaultColor();

//     uint8_t r = _default_text_color.g;
//     uint8_t g = _default_text_color.g;
//     uint8_t b = _default_text_color.g;
//     uint8_t a = _default_text_color.g;

// protected:
//     static color _default_text_color;
// };

// const color WARNING_COLOR = color(255,164,0,255);
// const color ERROR_COLOR = color(255,0,0,255);
// const color INVALID_INPUT_COLOR = color(255,164,0,255);

const std::string WARNING_COLOR = "";
const std::string ERROR_COLOR = "";
const std::string INVALID_INPUT_COLOR= "";

/// @brief parses the given command
/// @warning if not able to be parsed returns a empty list
std::vector<std::string> parseTokens(const std::string& command);

/// @brief tries to convert the given string to the given type
/// @param value the output of the value covered from the string
/// @param strValue the input string
/// @param defaultValue the value will be assigned this value is the string is invalid
/// @returns false if the input is not valid for the given type 
template<typename valueType, typename std::enable_if_t<std::is_same_v<valueType, int> || std::is_same_v<valueType, float> || 
                            std::is_same_v<valueType, unsigned long> || std::is_same_v<valueType, unsigned int> || std::is_same_v<valueType, bool>>* = nullptr>
inline bool isValidInput(const std::string& strValue, valueType& value, valueType defaultValue)
{
    bool valid = false;

    if constexpr (std::is_same_v<valueType, int>)
    {
        valid = StringHelper::attemptToInt(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, float>)
    {
        valid = StringHelper::attemptToFloat(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, unsigned int>)
    {
        valid = StringHelper::attemptToUInt(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, bool>)
    {
        valid = StringHelper::attemptToBool(strValue, value);
    }
    else // if the type is unsigned long 
    {
        valid = StringHelper::attemptToULong(strValue, value);
    }

    return valid;
}

class Handler;

class Data
{
public:
    class Info
    {
    public:
        void addInfo(const std::string& info);
        void addInfo(const std::list<std::string>& info);

        const std::list<std::string>& getInfo() const;
        bool hasInfo() const;
    private:
        std::list<std::string> m_info;
    };

    /// @brief sets the tokens with the given string
    /// @note if empty then starts with no tokens
    Data(const std::string& str = "");

    /// @brief sets the tokens given a string
    /// @note parses the string for the tokens 
    /// @note tokens will contain strings that are separated by whitespace
    void setTokens(const std::string& str);
    /// @brief overrides the current tokens to the given tokens
    void setTokens(const std::vector<std::string>& tokens);
    void setToken(size_t index, const std::string& tokenStr);
    void addToken(const std::string& tokenStr);
    void removeToken(size_t index = 0);

    /// @returns a const reference to the current tokens
    const std::vector<std::string>& getTokens() const;
    /// @param begin the first index
    /// @param end the last index
    /// @returns the tokens in the given range combined into one string separated by whitespace
    std::string getTokensStr(size_t begin = 0, size_t end = std::numeric_limits<size_t>().max()) const;
    /// @param index the index of the wanted token
    /// @returns if there is a token at the index returns token else returns an empty string
    std::string getToken(size_t index = 0) const;
    /// @note if no token exists then returns an empty string
    std::string getFirstToken() const;
    /// @note if no token exists then returns an empty string
    std::string getLastToken() const;
    /// @returns the current number of tokens stored
    size_t getNumTokens() const;

    /// @returns the current return string
    const std::string& getReturnStr() const;
    /// @brief sets the string that will be returned after the command if finished
    void setReturnStr(const std::string& returnStr);
    /// @brief adds a line to the return string
    /// @param line the line to be added (adds a \\n to the start of the string)
    void addReturnStrLine(const std::string& line);
    /// @brief adds a string to the return string
    /// @note does not add the stirng in a new line
    /// @param str the string to be added
    void addToReturnStr(const std::string& str);
    /// @brief parses the command recursively and calls any sub commands for input values
    /// @param context the command handler that will be used to call the sub commands
    /// @note Errors and warnings during the parsing will be added to this data object
    /// @note commands are called for input if they are in brackets i.e. "$(command)"
    /// @note to just input "$(command)" as a string without trying to call <command> use a "$" in front of the "$(" i.e."$$("
    void parseCommandInput(Command::Handler& context);
    
    /// @brief if true then commands that depended on this commands output are NOT called (used in "parseCommand()")
    bool hasErrors() const;
    /// @brief if true then commands that depended on this commands output are STILL called (used in "parseCommand()")
    bool hasWarnings() const;
    const std::list<std::string>& getWarnings() const;
    const std::list<std::string>& getErrors() const;
    /// @note you must add any coloring to the string yourself
    void addWarning(const std::string& warning);
    /// @note you must add any coloring to the string yourself
    void addWarnings(const std::list<std::string>& warnings);
    /// @note you must add any coloring to the string yourself
    void addError(const std::string& error);
    /// @note you must add any coloring to the string yourself
    void addErrors(const std::list<std::string>& errors);

protected:

private:
    std::vector<std::string> m_tokens;
    std::string m_return;
    Data::Info m_errors;
    Data::Info m_warnings;
};

/// @brief template for printing something (can be used as the func for a command)
/// @note this is not affected by the 'canPrint()' state
/// @note only give a value for str
void print(const std::string& str, Data* input);

/// @brief prints 'Use "help <name>" for more infomation'
/// @note this is not affected by the 'canPrint()' state
void helpCommand(const std::string& name, Data* data);
#define helpPrint helpCommand

class Definition
{
public:
    /// @param description Description of the command
    /// @param func The function to be called can take in Command Data if wanted
    /// @param subCommands Commands that are only accessible in this commands scope
    /// @param possibleInputs SOME/all possible inputs to this command/subCommand (used for autoFill)
    Definition(const std::string& description, 
                const funcHelper::funcDynamic<Data*>& func = [](){}, 
                const std::set<std::string>& possibleInputs = {},
                const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands = {});
    Definition(const Command::Definition& command);

    /// @brief Description of the command
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& setDescription(const std::string& description);
    /// @brief The function to be called can take in Command Data if wanted
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& setFunction(const funcHelper::funcDynamic<Data*>& func);
    /// @brief Commands that are only accessible in this commands scope
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& setCommands(const std::map<std::string, Command::Definition, Command::Command_Compare>& subCommands);
    /// @param replace if true replaces the command if it already exists
    /// @param name can NOT be a scoped command
    /// @note if any scoped/sub commands dont exist but there main command does then they will be added under it no matter what replace is
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& addCommand(const std::string& name, const Command::Definition& command, bool replace = false);
    /// @param replace if true replaces the command if it already exists
    /// @param name can NOT be a scoped command
    /// @note if any scoped/sub commands dont exist but there main command does then they will be added under it no matter what replace is
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& addCommand(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func = [](){}, 
                                    const std::set<std::string>& possibleInputs = {},
                                    const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands = {}, bool replace = false);
    /// @brief SOME/all possible inputs to this command/subCommand (used for autoFill)
    /// @note Scoped/Sub commands are already included in the autoFill please don't add them here
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& setPossibleInputs(const std::set<std::string>& possibleInputs);
    /// @brief SOME/all possible inputs to this command/subCommand (used for autoFill)
    /// @note Scoped/Sub commands are already included in the autoFill please don't add them here
    /// @returns The reference to this command for chaining function calls when setting values
    Command::Definition& addPossibleInput(const std::string& input);
    Command::Definition& removePossibleInput(const std::string& input);
    Command::Definition& removeAllPossibleInputs(const std::set<std::string>& inputs);

    std::string getDescription() const;
    /// @param name the name for this Command::Definition
    /// @param maxLength the max length of the string to be returned
    /// @param tabs the number of tabs to be added after any "\n"
    /// @returns the name separated by " - " and the description
    std::string getNameDescription(const std::string& name, size_t maxLength = SIZE_MAX, int tabs = 0) const;
    /// @note this does not include the definition of this command
    /// @param CommandIndex is only for the recursive usage of this function
    /// @param maxLineLength the max length of any commands description
    /// @returns the name separated by " - " and the description of each scoped/sub command (with the max length given)
    std::string getScopedNameDescriptions(size_t maxLineLength = SIZE_MAX, size_t commandIndex = 1) const;
    /// @param name the name for this Command::Definition
    /// @param maxLineLength the max length of any commands description
    /// @param CommandIndex is only for the recursive usage of this function
    /// @returns the name separated by " - " and the description of each scoped/sub command (with the max length given)
    std::string getFullNameDescription(const std::string& name, size_t maxLineLength = SIZE_MAX, size_t commandIndex = 1) const;
    /// @returns a list of possible inputs
    const std::set<std::string>& getPossibleInputs() const;
    /// @returns the currently set function for this Command::Definition
    const funcHelper::funcDynamic<Data*>& getFunction() const;
    /// @returns all the scoped/sub commands
    const std::map<std::string, Command::Definition, Command::Command_Compare>& getCommands() const;
    
    /// @param name the name of the command that is wanted
    /// @note name can include a space separated scoped command "c1 c2 c3" to remove only "c3" that is scoped under "c1 c2"
    /// @returns nullptr if not found else a pointer to the command
    const Command::Definition* findCommand(const std::string& command) const;
    Command::Definition* findCommand(const std::string& command);
    
    /// @param path the path to the command that is wanted, if empty then always returns nullptr
    /// @example For path, searching for "c2" where it is scoped under "c1" which is scoped under this command, i.e. "c1 c2" would have path={"c1", "c2"}
    /// @returns nullptr if not found else a pointer to the command
    const Command::Definition* findCommand(const std::vector<std::string>& path = {}) const;
    Command::Definition* findCommand(const std::vector<std::string>& path = {});
    
    const Command::Definition* findCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath) const;
    Command::Definition* findCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath);

    /// @brief tries to find the given command and returns the closest match
    /// @returns a pair of a string of the command found and the command definition
    std::pair<std::string, const Command::Definition*> findClosestCommand(const std::string& command) const;
    /// @brief tries to find the given command and returns the closest match
    /// @returns a pair of a string of the command found and the command definition
    std::pair<std::string, Command::Definition*> findClosestCommand(const std::string& command);

    /// @brief tries to find the given command and returns the closest match
    /// @returns a pair of a string of the command found and the command definition
    std::pair<std::string, const Command::Definition*> findClosestCommand(const std::vector<std::string>& tokens) const;
    /// @brief tries to find the given command and returns the closest match
    /// @returns a pair of a string of the command found and the command definition
    std::pair<std::string, Command::Definition*> findClosestCommand(const std::vector<std::string>& tokens);

    std::pair<std::string, const Command::Definition*> findClosestCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath) const;
    std::pair<std::string, Command::Definition*> findClosestCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath);

    Command::Definition* removeAllCommands();
    /// @brief removes the given command and all of its sub commands if found
    /// @note this can include a space separated scoped command "c1 c2 c3" to remove only "c3" that is scoped under "c1 c2"
    Command::Definition* removeCommand(const std::string& command);
    /// @brief removes the given command and all of its sub commands if found
    /// @param commandPath the path to the command that is wanted to be removed
    void removeCommand(const std::vector<std::string>& commandPath);
    void removeCommand(std::vector<std::string>::const_iterator beginPath, std::vector<std::string>::const_iterator endPath);
    /// @brief used for auto fill / command predictions
    /// @param search will take in any str and try to parse it (tokens being separated by spaces)
    /// @note if there is a space after the commend the auto fill will search for the next commands if possible
    /// @returns an unsorted list of possible commands
    std::list<std::string> autoFillSearch(const std::string& search);

    /// @param data should be the entire command string
    /// @note this calls parseCommand() on the data before calling the function
    /// @note data will contain the return string if the function is called
    /// @note if there where any errors then the command will not be called (errors are stored in the data)
    /// @note if there are warning the function will still be called if possible (warnings are stored in the data)
    void invoke(Command::Handler& context, Data& data) const;

protected:
    /// @brief key is the name, value is the definition
    std::map<std::string, Command::Definition, Command::Command_Compare> m_commands = {};
    std::set<std::string> m_possibleInputs = {};

private:
    std::string m_description = "";
    
    funcHelper::funcDynamic<Data*> m_function;
};

class Handler
{
public:
    inline Handler() = default;
    static Handler& get();

    /// @param replace if true replaces the command if it already exists
    /// @param name can NOT be a scoped command
    /// @note if any scoped/sub commands dont exist but there main command does then they will be added under it no matter what replace is
    void addCommand(const std::string& name, const Command::Definition& command, bool replace = false);
    /// @param replace if true replaces the command if it already exists
    /// @param name can NOT be a scoped command
    /// @note if any scoped/sub commands dont exist but there main command does then they will be added under it no matter what replace is
    void addCommand(const std::string& name, const std::string& description, const funcHelper::funcDynamic<Data*>& func = [](){}, 
                                    const std::set<std::string>& possibleInputs = {},
                                    const std::map<std::string, Command::Definition, Command::Command_Compare>& scopedCommands = {}, bool replace = false);

    void removeAllCommands();
    /// @brief removes the given command and all of its scoped commands if
    /// @param commandPath the path to the command that is wanted to be removed
    void removeCommand(const std::vector<std::string>& commandPath);
    /// @brief removes the given command and all of its scoped commands if
    /// @param strCommand the command that is wanted to be removed WITHOUT any inputs
    void removeCommand(const std::string& command);

    Command::Definition* findCommand(const std::string& command);
    Command::Definition* findCommand(const std::vector<std::string>& commandPath);

    /// @brief used for auto fill / command predictions
    /// @param search will take in any str and try to parse it (tokens being separated by spaces)
    /// @note if there is a space after the commend the auto fill will search for the next commands if possible
    /// @returns an unsorted list of possible commands
    std::list<std::string> autoFillSearch(const std::string& search) 
    {
        return m_commands.autoFillSearch(search);
    }

    /// @brief invokes the given command
    /// @param command will take in any str and try to parse it (tokens being separated by spaces)
    /// @note the command could have failed which will be documented in the return string
    /// @returns the data collected from the command
    Data invokeCommand(const std::string& commandStr);

    /// @brief the history of commands that have been called
    /// @note immediate repeats are not added
    size_t getMaxCommandHistory();
    /// @brief the history as a command prompt would show it
    /// @note storead by "lines" each line is a command or the result of a command
    size_t getMaxLineHistory();
    /// @brief the history of commands that have been called
    /// @note immediate repeats are not added
    void setMaxCommandHistory(size_t size = 64);
    /// @brief the history as a command prompt would show it
    /// @note storead by "lines" each line is a command or the result of a command
    void setMaxLineHistory(size_t size = 128);
    /// @brief the history of commands that have been called
    /// @note immediate repeats are not added
    const std::list<std::string>& getCommandHistory();
    /// @brief the history as a command prompt would show it
    /// @note storead by "lines" each line is a command or the result of a command
    const std::list<std::string>& getCommandHistoryLines();

    /// @brief called when the max command history is changed
    EventHelper::Event onMaxCommandHistoryChanged;
    /// @brief called when the line history is changed
    /// @note a line is a command or the result of a command
    EventHelper::Event onMaxLineHistoryChanged;
    void setThreadSafeEvents(bool threadSafe = true);
    bool isThreadSafeEvents();

protected:

private:
    /// @brief key is the name, value is the definition
    Command::Definition m_commands = Command::Definition("Base Scope", {&Command::print, HELP_STRING});
    bool m_threadSafeEvents = false;

    //* History
    size_t m_maxCommandHistory = 64; 
    /// @brief the history of commands that have been called
    /// @note immediate repeats are not added
    std::list<std::string> m_commandHistory = {};
    size_t m_maxLineHistory = 128;
    /// @brief the history as a command prompt would show it
    /// @note storead by "lines" each line is a command or the result of a command
    std::list<std::string> m_lineHistory = {};
};

}

#endif
