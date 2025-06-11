#include "Utils/EventHelper.hpp"

using namespace EventHelper;

std::list<std::pair<Event*, std::function<void()>>> Event::Synchronized::m_events;
std::mutex Event::Synchronized::m_lock;

void Event::Synchronized::update()
{
    m_lock.lock();
    for (auto event: m_events)
    {
        event.second.operator()();
    }
    m_events.clear(); // removing all the events in the queue
    m_lock.unlock();
}

void Event::Synchronized::removeEvent(Event* event)
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

bool Event::disconnect(size_t id)
{
    return (m_callbacks.erase(id) != 0);
}

void Event::disconnectAll()
{
    // all public ids will be greater than m_nextPublicID
    auto iter = m_callbacks.lower_bound(m_nextPublicID);
    m_callbacks.erase(iter, m_callbacks.end());
    
    // Reset the public ID counter
    m_nextPublicID = STARTING_PUBLIC_ID;
}

void Event::invoke(bool threadSafe, bool removeOtherInstances)
{
    if (m_callbacks.empty() || !m_enabled)
        return;

    if (threadSafe)
    {
        Event::Synchronized::addEvent(this, [this]{ Event::invokeFunc(&Event::_invoke, this); }, removeOtherInstances);
    }

    _invoke();
}

void Event::_invoke()
{
    auto callback = m_callbacks.begin();
    while (callback != m_callbacks.end())
    {
        auto temp = callback++;
        (*temp).second();
    }
}

size_t Event::getNumCallbacks() const
{
    return m_callbacks.size();
}
