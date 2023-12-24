#include "include\Utils\EventHelper.hpp"

using namespace EventHelper;

std::set<Event*> ThreadSafeEvent::_events;

void ThreadSafeEvent::addEvent(Event* event)
{
    _events.insert({event});
}

void ThreadSafeEvent::update()
{
    for (auto event: _events)
    {
        event->invoke();
    }
    _events.clear();
}

void ThreadSafeEvent::removeEvent(Event* event)
{
    _events.erase(event);
}

std::deque<const void*> Event::m_parameters(5, nullptr);

Event::Event(const Event& other) :
    m_enabled {other.m_enabled},
    m_functions{} // Event handlers are not copied with the widget
{
}

Event& Event::operator=(const Event& other)
{
    if (this != &other)
    {
        m_enabled = other.m_enabled;
        m_functions.clear(); // Event handlers are not copied with the widget
    }

    return *this;
}

bool Event::disconnect(unsigned int id)
{
    return (m_functions.erase(id) != 0);
}

void Event::disconnectAll()
{
    m_functions.clear();
}

bool Event::invoke(bool threadSafe)
{
    if (threadSafe)
    {
        ThreadSafeEvent::addEvent(this);
        return true;
    }

    if (m_functions.empty() || !m_enabled)
        return false;

    for (const auto& function : m_functions)
        function.second();

    return true;
}
