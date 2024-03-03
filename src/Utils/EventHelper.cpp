#include "Utils\EventHelper.hpp"

using namespace EventHelper;

std::list<std::pair<Event*, std::function<void()>>> Event::ThreadSafe::_events;

void Event::ThreadSafe::update()
{
    for (auto event: _events)
    {
        event.second();
    }
    _events.clear(); // removing all the events in the queue
}

void Event::ThreadSafe::removeEvent(Event* event)
{
    // removing from list
    _events.remove_if(
        [event](const std::pair<EventHelper::Event *, std::function<void()>>& v){
            return v.first == event;
        });
}

std::deque<const void*> Event::m_parameters(5, nullptr);

Event::Event(const Event& other) :
    m_enabled {other.m_enabled},
    m_functions{} // Event handlers are not copied
{
}

Event& Event::operator=(const Event& other)
{
    if (this != &other)
    {
        m_enabled = other.m_enabled;
        m_functions.clear(); // Event handlers are not copied
    }

    return *this;
}

bool Event::disconnect(size_t id)
{
    return (m_functions.erase(id) != 0);
}

void Event::disconnectAll()
{
    m_functions.clear();
}

bool Event::invoke(bool threadSafe, bool removeOtherInstances)
{
    if (m_functions.empty() || !m_enabled)
        return false;

    if (threadSafe)
    {
        if (removeOtherInstances) Event::ThreadSafe::removeEvent(this);
        Event::ThreadSafe::addEvent(this, [this]{ Event::invokeFunc(&Event::_invoke, this); });
        return true;
    }

    _invoke();
    return true;
}

void Event::_invoke()
{
    for (const auto& function : m_functions)
        function.second();
}

// void Event::invokeThreadSafe()
// {
//     Event::ThreadSafe::addEvent(this);
// }