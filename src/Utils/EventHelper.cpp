#include "include\Utils\EventHelper.hpp"

using namespace EventHelper;

std::list<std::pair<Event*, std::deque<void*>>> Event::ThreadSafe::_events;

void Event::ThreadSafe::update()
{
    for (auto event: _events)
    {
        // copying the stored vars to the events vars
        event.first->m_parameters.insert(event.first->m_parameters.begin(), event.second.begin(), event.second.end());
        event.first->invoke();
        for (void* var: event.second)
        {
            delete(var);
        }
    }
    _events.clear();
}

void Event::ThreadSafe::removeEvent(Event* event)
{
    // trying to find the event with the same pointer
    auto temp = std::find_if(_events.begin(), _events.end(), 
        [event](const std::pair<EventHelper::Event *, std::deque<void*>>& v){
            return v.first == event;
        });
    // making sure the event is in the list
    if (temp == _events.end()) return;

    // freeing all memory from the temp params
    for (void* var: temp->second)
    {
        delete(var);
    }
    // removing from list
    _events.erase(temp);
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
        Event::ThreadSafe::addEvent(this);
    }

    if (m_functions.empty() || !m_enabled)
        return false;

    for (const auto& function : m_functions)
        function.second();

    return true;
}

void Event::invokeThreadSafe()
{
    Event::ThreadSafe::addEvent(this);
}