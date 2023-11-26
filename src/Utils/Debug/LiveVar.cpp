#include "include/Utils/Debug/LiveVar.h"

EventHelper::EventDynamic<std::string> LiveVar::onVarAdded;
EventHelper::EventDynamic<std::string> LiveVar::onVarRemoved;
std::unordered_map<std::string, std::pair<float, std::pair<EventHelper::EventDynamic<float>, funcHelper::funcDynamic2<float*, sf::Event>>>> LiveVar::m_vars;

float LiveVar::getVar(const std::string& name)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return std::numeric_limits<float>::min();

    return iter->second.first;
}

EventHelper::EventDynamic<float>* LiveVar::getVarEvent(const std::string& name)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return nullptr;

    return &(iter->second.second.first);
}

bool LiveVar::setVar(const std::string& name, const float& value)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return false;

    iter->second.first = value;
    iter->second.second.first.invoke(value);
    return true;
}

bool LiveVar::initVar(const std::string& name, const float& value)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, {{}, {[](){}}}}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, const float& value, funcHelper::funcDynamic2<float*, sf::Event> func)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, {{}, func}}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, const float& value, const float& increment, const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey,
                        const float& min, const float& max)
{
    return initVar(name, value, {&updateValue, increaseKey, decreaseKey, increment, min, max});
}

bool LiveVar::removeVar(const std::string& name)
{
    if (m_vars.erase(name))
    {
        onVarRemoved.invoke(name);
        return true;
    }
    return false;
}

void LiveVar::UpdateLiveVars(const sf::Event& event)
{
    for (auto iter = m_vars.begin(); iter != m_vars.end(); iter++)
    {
        float temp = iter->second.first;
        iter->second.second.second.invoke(&iter->second.first, event);
        if (temp != iter->second.first)
        {
            iter->second.second.first.invoke(iter->second.first);
        }
    }
}

void LiveVar::updateValue(const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey, const float& increment,
                            const float& min, const float& max, float* value, const sf::Event& event)
{
    if (event.type != sf::Event::KeyPressed) return;
    if (event.key.code == increaseKey)
    {
        (*value) = std::min(max, (*value) + increment);
    }
    else if (event.key.code == decreaseKey)
    {
        (*value) = std::max(min, (*value) - increment);
    }
}

void LiveVar::updateValue2(const std::list<std::pair<sf::Keyboard::Key, float>>& values, float* value, const sf::Event& event)
{
    if (event.type != sf::Event::KeyPressed) return;

    for (std::list<std::pair<sf::Keyboard::Key, float>>::const_iterator i = values.begin(); i != values.end(); i++)
    {
        if (event.key.code == i->first)
        {
            (*value) = i->second;
            return;
        }
    }
}
