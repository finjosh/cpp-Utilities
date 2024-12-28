#include "Utils/Debug/LiveVar.hpp"

LiveVar::var::var(float value, funcHelper::funcDynamic2<float*, sf::Event> keyEvent) : value(value), keyEvent(keyEvent) {}

float LiveVar::var::getValue() const
{
    return value;
}

void LiveVar::var::setValue(float v)
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

    if (iter == m_vars.end()) return std::numeric_limits<float>::infinity();

    return iter->second.getValue();
}

EventHelper::EventDynamic<float>* LiveVar::getVarEvent(const std::string& name)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return nullptr;

    return &(iter->second.onChanged);
}

bool LiveVar::setValue(const std::string& name, float value)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return false;

    iter->second.setValue(value);
    return true;
}

bool LiveVar::initVar(const std::string& name, float value)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, [](){}}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, float value, funcHelper::funcDynamic2<float*, sf::Event> func)
{
    if (m_vars.find(name) == m_vars.end()) 
    {
        m_vars.insert({name, {value, func}});
        onVarAdded.invoke(name);
        return true;
    }
    
    return false;
}

bool LiveVar::initVar(const std::string& name, float value, float increment, sf::Keyboard::Key increaseKey, sf::Keyboard::Key decreaseKey,
                        float min, float max)
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

void LiveVar::incrementKeys(sf::Keyboard::Key increaseKey, sf::Keyboard::Key decreaseKey, float increment,
                            float min, float max, float* value, const sf::Event& event)
{
    if (const sf::Event::KeyPressed* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == increaseKey)
        {
            (*value) = std::min(max, (*value) + increment);
        }
        else if (keyPressed->code == decreaseKey)
        {
            (*value) = std::max(min, (*value) - increment);
        }
    }
}

void LiveVar::presetKeys(const std::list<std::pair<sf::Keyboard::Key, float>>& values, float* value, const sf::Event& event)
{
    if (const sf::Event::KeyPressed* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        for (std::list<std::pair<sf::Keyboard::Key, float>>::const_iterator i = values.begin(); i != values.end(); i++)
        {
            if (keyPressed->code == i->first)
            {
                (*value) = i->second;
                return;
            }
        }
    }
}
