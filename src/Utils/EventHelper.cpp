#include "Utils/EventHelper.hpp"

using namespace EventHelper;

std::list<std::pair<Event*, std::function<void()>>> Event::ThreadSafe::m_events;
std::mutex Event::ThreadSafe::m_lock;

void Event::ThreadSafe::update()
{
    m_lock.lock();
    for (auto event: m_events)
    {
        event.second.operator()();
    }
    m_events.clear(); // removing all the events in the queue
    m_lock.unlock();
}

void Event::ThreadSafe::removeEvent(Event* event)
{
    m_lock.lock();
    // removing from list
    m_events.remove_if(
        [event](const std::pair<EventHelper::Event *, std::function<void()>>& v){
            return v.first == event;
        });
    m_lock.unlock();
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
    m_lastId = 0;
}

bool Event::invoke(bool threadSafe, bool removeOtherInstances)
{
    if (m_functions.empty() || !m_enabled)
        return false;

    if (threadSafe)
    {
        Event::ThreadSafe::addEvent(this, [this]{ Event::invokeFunc(&Event::_invoke, this); }, removeOtherInstances);
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

size_t Event::getNumCallbacks() const
{
    return m_functions.size();
}

// void Event::invokeThreadSafe()
// {
//     Event::ThreadSafe::addEvent(this);
// }