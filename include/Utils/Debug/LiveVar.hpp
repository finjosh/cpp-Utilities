#ifndef LIVE_VAR_H
#define LIVE_VAR_H

#pragma once

#include <unordered_map>
#include <string>
#include <list>
#include <numeric>

#include "SFML/Window/Event.hpp"

#include "Utils/funcHelper.hpp"
#include "Utils/EventHelper.hpp"

#if __has_include("Utils/CommandHandler.hpp")
#include "Utils/CommandHandler.hpp" 
#endif

#define VALUE_SET std::pair<sf::Keyboard::Key, float>

class LiveVar
{
public:

    /**
     * @brief if there is not already a var of that name returns INFINITY
    */
    static float getValue(const std::string& name);

    /**
     * @brief the given event is called whenever the value connected to the given name is changed
     * @note optional param is the value of the given var
     * @exception if there is no value with the given name returns nullptr
     * @returns a event that is called when the value is changed
    */
    static EventHelper::EventDynamic<float>* getVarEvent(const std::string& name); 

    /**
     * @brief sets the value of the var
     * @note if the given name does not exist nothing happens
     * @returns true if set false if not
    */
    static bool setValue(const std::string& name, float value);

    /**
     * @param func a function that will return the new value depending on the given event data
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, float value);

    /**
     * @param func a function that will return the new value depending on the given event data
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, float value, funcHelper::funcDynamic2<float*, sf::Event> func);

    /**
     * @param increaseKey when the given key is pressed the value will be increase by the increment amount
     * @param decreaseKey when the given key is pressed the value will be decrease by the increment amount
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, float value, float increment, sf::Keyboard::Key increaseKey, sf::Keyboard::Key decreaseKey,
                        float min = std::numeric_limits<float>::lowest(), float max = std::numeric_limits<float>::max());

    /**
     * @param value is a pair with the keyboard key and the value that will be selected when said key is pressed
     * @param numberOfValues the number of pairs that will be given
     * @param v the default value if no key is clicked
     * @note if there is already a var with the same name the new one will not be added
     * @warning ellipsis must be the pair of keyboard::key, float
     * @returns false if the value was not added
    */
    inline static bool initVar(const std::string& name, float v, std::convertible_to<VALUE_SET> auto&&... value)
    {
        std::list<std::pair<sf::Keyboard::Key, float>> values = {value...};

        return initVar(name, v, {&presetKeys, values});
    }

    /// @returns true if removed false if not
    static bool removeVar(const std::string& name);

    static void UpdateLiveVars(const sf::Event& event);

    /// @brief called when ever a var is initalized and returns the name of the added var
    static EventHelper::EventDynamic<std::string> onVarAdded;
    /// @brief called when ever a var is removed
    static EventHelper::EventDynamic<std::string> onVarRemoved;

    /// @brief initializes commands for live vars if the command handler is included
    /// @note if varDisplay is used, this is already called
    static inline void initCommand()
    {
        // if command handler is also in use then we add some commands for using the var display
        #ifdef COMMAND_HANDLER_H
        //* adding commands for live vars to the command handler
        Command::Handler::get().addCommand("lVars", "Contains commands for live variables", {Command::helpPrint, "Trying using 'help lVars'"});
        Command::Handler::get().findCommand("lVars")
        ->addCommand("get", "[Name] | Gets the value for the given variable", {[](Command::Data* data){
                float temp = LiveVar::getValue(data->getTokensStr());
                if (temp == std::numeric_limits<float>::infinity())
                {
                    data->setReturnStr("Value does not exist");
                    // data->setReturnColor(Command::WARNING_COLOR); // TODO set the color
                    return;
                }
                data->setReturnStr(std::to_string(temp));
            }})
        .addCommand("set", "[Name] [value : Float] | Sets the value for the given variable", {[](Command::Data* data){
                float value;
                if (!Command::isValidInput<float>(data->getToken(1), value, std::numeric_limits<float>::infinity()))
                {
                    data->setReturnStr("Invalid amount entered");
                    return;
                }
                if (LiveVar::setValue(data->getTokensStr(), value))
                {
                    data->setReturnStr("Variable successfully set");
                    // data->setReturnColor({0,255,0}); // TODO set the color
                    return;
                }
                // data->setReturnColor(Command::INVALID_INPUT_COLOR); // TODO set the color
                data->setReturnStr("Invalid name entered");
            }})
        .addCommand("create", "[Name] [initValue = 0] | Creates a new live variable", {[](Command::Data* data){
                float value = StringHelper::toFloat(data->getToken(data->getNumTokens()-1), 0);
                
                if (data->getToken(0) != "" && !LiveVar::initVar(data->getTokensStr(0, data->getNumTokens()-2), value))
                {
                    data->setReturnStr("Variable already exists");
                    // data->setReturnColor(Command::INVALID_INPUT_COLOR); // TODO set the color
                    return;
                }
                data->setReturnStr("Variable successfully created");
                // data->setReturnColor({0,255,0}); // TODO set the color
            }})
        .addCommand("remove", "[Name] | Removes the live variable if it exists", {[](Command::Data* data){
                if (LiveVar::removeVar(data->getTokensStr()))
                {
                    data->setReturnStr("Variable successfully removed");
                    // data->setReturnColor({0,255,0}); // TODO set the color
                    return;
                }
                data->setReturnStr("Invalid name entered");
                // data->setReturnColor(Command::INVALID_INPUT_COLOR); // TODO set the color
            }});
        #endif
    }

protected:
    // updates the value based on the keys and increment amount
    static void incrementKeys(sf::Keyboard::Key increaseKey, sf::Keyboard::Key decreaseKey, float increment, 
                            float min, float max, float* value, const sf::Event& event);
    // updates the value based on the preset keys and values
    static void presetKeys(const std::list<std::pair<sf::Keyboard::Key, float>>& values, float* value, const sf::Event& event);

private:
    inline LiveVar() = default;

    class var
    {
    public:
        var(float value, funcHelper::funcDynamic2<float*, sf::Event> keyEvent);

        float getValue() const;
        void setValue(float v);
        void invokeKeyEvent(const sf::Event& event);

        // called when ever the value is changed
        EventHelper::EventDynamic<float> onChanged;
    private:
        float value;
        // used to change the value based on the preset key events
        funcHelper::funcDynamic2<float*, sf::Event> keyEvent;
    };

    static std::unordered_map<std::string, var> m_vars;
};

#endif
