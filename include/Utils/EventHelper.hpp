#ifndef EVENT_HELPER_H
#define EVENT_HELPER_H

#pragma once

// #include <unordered_map>
#include <map>
#include <list>
#include <deque>
#include <memory>
#include <functional>
#include <mutex>
#include <cassert>

namespace EventHelper
{
static constexpr size_t INVALID_ID = SIZE_MAX;
static constexpr size_t STARTING_PUBLIC_ID = INVALID_ID-1;
static constexpr size_t STARTING_PRIVATE_ID = 0;

/// @note when using dynamic events with synchronized calls make sure the synchronized var is set true when invoking
/// @note connectPrivate is similar to connect but the callbacks can only be removed via there id "disconnectAll()" will not remove private callbacks
class Event
{
public:
    inline Event() = default;
    inline ~Event()
    {
        // removing pointer to this event in synchronized in case it is still in queue
        Event::Synchronized::removeEvent(this); // TODO do this better?
    }

    /// @brief Default move constructor
    Event(Event&& other) noexcept = default;
    /// @brief Default move assignment operator
    Event& operator=(Event&& other) noexcept = default;

    /// @brief Connects a function that will be called when this event is invoked
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    /// @return Unique id of the connection (specific to this event)
    template <typename Func, typename... BoundArgs>
    size_t operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    /// @return Unique id of the connection (specific to this event)
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    size_t connect(const Func& func, const BoundArgs&... args)
    {
        const size_t id = m_nextPublicID--;

        assert(id > m_nextPrivateID && "EventHelper Callback id overflow" && __PRETTY_FUNCTION__);
        #ifndef EVENT_HELPER_DONT_THROW
        if (id <= m_nextPrivateID)
            throw std::runtime_error("EventHelper callback id overflow" + std::string(__PRETTY_FUNCTION__));
        #endif

        if constexpr(sizeof...(BoundArgs) == 0)
            m_callbacks[id] = func;
        else
        {
            m_callbacks[id] = [=]{ 
                invokeFunc(func, args...); 
            };
        }
        return id;
    }

    /// @brief Connects a function that will be called when this event is invoked
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    /// @note private callbacks can only be removed via there id "disconnectAll()" will not remove them
    /// @return Unique id of the connection (specific to this event)
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    size_t connectPrivate(const Func& func, const BoundArgs&... args)
    {
        const size_t id = m_nextPrivateID++;
        
        assert(id < m_nextPublicID && "EventHelper Callback id overflow" && __PRETTY_FUNCTION__);
        #ifndef EVENT_HELPER_DONT_THROW
        if (id >= m_nextPublicID)
            throw std::runtime_error("EventHelper callback id overflow" + std::string(__PRETTY_FUNCTION__));
        #endif

        if constexpr(sizeof...(BoundArgs) == 0)
            m_callbacks[id] = func;
        else
        {
            m_callbacks[id] = [=]{ 
                invokeFunc(func, args...); 
            };
        }
        return id;
    }

    /// @brief Disconnect a function from this event
    /// @param id  Unique id of the callback returned by the connect function
    /// @return True when a connection with this id existed and was removed
    bool disconnect(size_t id);

    /// @brief changes the callback with the given id
    /// @param id Unique id of the callback returned by the connect function
    /// @return True if callback was found and changed
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args)
    {
        auto callback = m_callbacks.find(id);
        if (callback != m_callbacks.end())
        {
            if constexpr(sizeof...(BoundArgs) == 0)
                callback->second = func;
            else
            {
                callback->second = [=]{ 
                    invokeFunc(func, args...); 
                };
            } 

            return true;
        }

        return false;
    }

    /// @brief Disconnect all public function from this event
    /// @note previous ids are now invalid
    void disconnectAll();

    /// @brief Call all connected functions
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @param removeOtherInstances if true will remove any other synchronized calls of this event before adding
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @note private callbacks are invoked before public callbacks
    void invoke(bool synchronized = false, bool removeOtherInstances = false);

    /// @brief Changes whether this event calls the connected functions when triggered
    /// @param enabled  Is the event enabled?
    /// events are enabled by default. Temporarily disabling the event is the better alternative to disconnecting the
    /// handler and connecting it again a few lines later.
    void setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    /// @brief Returns whether this event calls the connected functions when triggered
    /// @return Is the event enabled?
    /// events are enabled by default. Temporarily disabling the event is the better alternative to disconnecting the
    /// handler and connecting it again a few lines later.
    bool isEnabled() const
    {
        return m_enabled;
    }

    /// @note this includes both public and private callbacks
    size_t getNumCallbacks() const;

    inline size_t getNextPublicID() const { return m_nextPublicID; }
    inline size_t getNextPrivateID() const { return m_nextPrivateID; }

    class Synchronized
    {
    public:
        /// @brief Call this first thing every frame
        /// @note invokes all events that where called with the synchronized flag
        static void update();
        /// @brief used when an event is destroyed in case its still in the queue
        /// @note removes all instances of the given event from the queue
        static void removeEvent(Event* event);
        /// @brief adds the given event to the synchronized queue and copys any data needed to call it
        /// @note dont use this unless you know what you are doing
        /// @note set synchronized = true when invoking from the event
        static void addEvent(Event* event, std::function<void()>&& func, bool removeSameEvents = false)
        {
            m_lock.lock();
            if (removeSameEvents)
                m_events.remove_if(
                [event](const std::pair<EventHelper::Event *, std::function<void()>>& v){
                    return v.first == event;
                });
            m_events.emplace_back(event, func);
            m_lock.unlock();
        }

    private:
        inline Synchronized() = default;

        static std::mutex m_lock;
        static std::list<std::pair<Event*, std::function<void()>>> m_events;
    };

protected:
    /// @brief Copy constructor which will not copy the callbacks
    Event(const Event& other) = delete;
    /// @brief Copy assignment operator which will not copy the callbacks
    Event& operator=(const Event& other) = delete;

    void _invoke();

    /// @brief Turns the void* parameters back into its original type right before calling the callback function
    template <typename Type>
    static const std::decay_t<Type>& dereferenceParam(std::size_t paramIndex)
    {
        return *static_cast<const std::decay_t<Type>*>(m_parameters[paramIndex]);
    }

    template <typename Func, typename... Args>
    static void invokeFunc(Func&& func, Args&&... args)
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    bool m_enabled = true;
    //!!!! dont change this unless order is no longer required for callbacks
    // private are called first since there ids start a 0 and increase
    std::map<size_t, std::function<void()>> m_callbacks; 

    // Possible issue if event are constantly removed and added
    size_t m_nextPublicID = STARTING_PUBLIC_ID;
    size_t m_nextPrivateID = STARTING_PRIVATE_ID;
    static std::deque<const void*> m_parameters;
};

#define CONNECT_NO_PARAMS() \
    template <typename Func, typename... BoundArgs> \
    size_t operator()(const Func& func, const BoundArgs&... args) \
    { \
        return connect(func, args...); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect(func, args...); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate(func, args...); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            if constexpr(sizeof...(BoundArgs) == 0) \
                callback->second = func; \
            else \
            { \
                callback->second = [=]{  \
                    invokeFunc(func, args...);  \
                }; \
            }  \
            return true; \
        } \
        return false; \
    }

#define CONNECT_ONE_PARAM() \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            callback->second = [=]{  \
                invokeFunc(func, args..., dereferenceParam<T>(0));  \
            }; \
            return true; \
        } \
        return false; \
    }

#define CONNECT_TWO_PARAMS() \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            callback->second = [=]{  \
                invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1));  \
            }; \
            return true; \
        } \
        return false; \
    }

#define CONNECT_THREE_PARAMS() \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            callback->second = [=]{  \
                invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2));  \
            }; \
            return true; \
        } \
        return false; \
    }

#define CONNECT_FOUR_PARAMS() \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            callback->second = [=]{  \
                invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3));  \
            }; \
            return true; \
        } \
        return false; \
    }

#define CONNECT_FIVE_PARAMS() \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr> \
    size_t connect(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3), dereferenceParam<T5>(4)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr> \
    size_t connectPrivate(const Func& func, const BoundArgs&... args) \
    { \
        return Event::connectPrivate([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3), dereferenceParam<T5>(4)); }); \
    } \
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr> \
    bool setCallback(size_t id, const Func& func, const BoundArgs&... args) \
    { \
        auto callback = m_callbacks.find(id); \
        if (callback != m_callbacks.end()) \
        { \
            callback->second = [=]{  \
                invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3), dereferenceParam<T5>(4));  \
            }; \
            return true; \
        } \
        return false; \
    }

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T>
class EventDynamic : public Event
{
public:

    /// @brief Constructor
    inline EventDynamic() = default;

    CONNECT_NO_PARAMS();
    CONNECT_ONE_PARAM();

    /// @brief Call all connected callbacks
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, bool synchronized = false, bool removeOtherInstances = false)
    {
        if (m_callbacks.empty() || !m_enabled)
            return false;

        if (synchronized)
        {
            // if (removeOtherInstances) Event::Synchronized::removeEvent(this);
            Event::Synchronized::addEvent(this, [this, param]{ Event::invokeFunc(&EventDynamic::invoke, this, param, false, false); }, removeOtherInstances);
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2>
class EventDynamic2 : public Event
{
public:

    /// @brief Constructor
    EventDynamic2()
    {}

    CONNECT_NO_PARAMS();
    CONNECT_ONE_PARAM();
    CONNECT_TWO_PARAMS();

    /// @brief Call all connected callbacks
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, bool synchronized = false, bool removeOtherInstances = false)
    {
        if (m_callbacks.empty() || !m_enabled)
            return false;

        if (synchronized)
        {
            // if (removeOtherInstances) Event::Synchronized::removeEvent(this);
            Event::Synchronized::addEvent(this, [this, param, param2]{ Event::invokeFunc(&EventDynamic2::invoke, this, param, param2, false, false); }, removeOtherInstances);
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3>
class EventDynamic3 : public Event
{
public:

    /// @brief Constructor
    EventDynamic3()
    {}

    CONNECT_NO_PARAMS();
    CONNECT_ONE_PARAM();
    CONNECT_TWO_PARAMS();
    CONNECT_THREE_PARAMS();

    /// @brief Call all connected callbacks
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, bool synchronized = false, bool removeOtherInstances = false)
    {
        if (m_callbacks.empty() || !m_enabled)
            return false;

        if (synchronized)
        {
            // if (removeOtherInstances) Event::Synchronized::removeEvent(this);
            Event::Synchronized::addEvent(this, [this, param, param2, param3]{ Event::invokeFunc(&EventDynamic3::invoke, this, param, param2, param3, false, false); }, removeOtherInstances);
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3, typename T4>
class EventDynamic4 : public Event
{
public:

    /// @brief Constructor
    EventDynamic4()
    {}

    CONNECT_NO_PARAMS();
    CONNECT_ONE_PARAM();
    CONNECT_TWO_PARAMS();
    CONNECT_THREE_PARAMS();
    CONNECT_FOUR_PARAMS();

    /// @brief Call all connected callbacks
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, T4 param4, bool synchronized = false, bool removeOtherInstances = false)
    {
        if (m_callbacks.empty() || !m_enabled)
            return false;

        if (synchronized)
        {
            // if (removeOtherInstances) Event::Synchronized::removeEvent(this);
            Event::Synchronized::addEvent(this, [this, param, param2, param3, param4]{ Event::invokeFunc(&EventDynamic4::invoke, this, param, param2, param3, param4, false, false); }, removeOtherInstances);
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3, typename T4, typename T5>
class EventDynamic5 : public Event
{
public:

    /// @brief Constructor
    EventDynamic5()
    {}

    CONNECT_NO_PARAMS();
    CONNECT_ONE_PARAM();
    CONNECT_TWO_PARAMS();
    CONNECT_THREE_PARAMS();
    CONNECT_FOUR_PARAMS();
    CONNECT_FIVE_PARAMS();

    /// @brief Call all connected callbacks
    /// @param synchronized if true this event will be called on EventHelper::Event::Synchronized::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @note if called as synchronized then returns true if there is at least one callback connected
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, T4 param4, T5 param5, bool synchronized = false, bool removeOtherInstances = false)
    {
        if (m_callbacks.empty() || !m_enabled)
            return false;

        if (synchronized)
        {
            // if (removeOtherInstances) Event::Synchronized::removeEvent(this);
            Event::Synchronized::addEvent(this, [this, param, param2, param3, param4, param5]{ Event::invokeFunc(&EventDynamic5::invoke, this, param, param2, param3, param4, param5, false, false); }, removeOtherInstances);
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        m_parameters[4] = static_cast<const void*>(&param5);
        Event::_invoke();
        return true;
    }
};

}

#endif
