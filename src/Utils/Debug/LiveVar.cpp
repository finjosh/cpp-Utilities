#include "include/Utils/Debug/LiveVar.hpp"

LiveVar::var::var(const float& value, funcHelper::funcDynamic2<float*, sf::Event> keyEvent) : value(value), keyEvent(keyEvent) {}

float LiveVar::var::getValue() const
{
    return value;
}

void LiveVar::var::setValue(const float& v)
{
    if (v != value)
    {
        value = v;
        onChanged.invoke(value);
    }
}

void LiveVar::var::invokeKeyEvent(const sf::Event& event)
{
    keyEvent.invoke(&value, event);
}

EventHelper::EventDynamic<std::string> LiveVar::onVarAdded;
EventHelper::EventDynamic<std::string> LiveVar::onVarRemoved;
std::unordered_map<std::string, LiveVar::var> LiveVar::m_vars;

float LiveVar::getValue(const std::string& name)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return std::numeric_limits<float>::min();

    return iter->second.getValue();
}

EventHelper::EventDynamic<float>* LiveVar::getVarEvent(const std::string& name)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return nullptr;

    return &(iter->second.onChanged);
}

bool LiveVar::setValue(const std::string& name, const float& value)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return false;

    iter->second.setValue(value);
    return true;
}

bool LiveVar::initVar(const std::string& name, const float& value)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, [](){}}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, const float& value, funcHelper::funcDynamic2<float*, sf::Event> func)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, func}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, const float& value, const float& increment, const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey,
                        const float& min, const float& max)
{
    return initVar(name, value, {&incrementKeys, increaseKey, decreaseKey, increment, min, max});
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
        float temp = iter->second.getValue();
        iter->second.invokeKeyEvent(event);
        if (temp != iter->second.getValue())
        {
            iter->second.onChanged.invoke(iter->second.getValue());
        }
    }
}

void LiveVar::incrementKeys(const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey, const float& increment,
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

void LiveVar::presetKeys(const std::list<std::pair<sf::Keyboard::Key, float>>& values, float* value, const sf::Event& event)
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
