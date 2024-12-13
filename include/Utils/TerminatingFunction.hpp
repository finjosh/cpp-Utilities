#ifndef TERMINATING_FUNCTION_H
#define TERMINATING_FUNCTION_H

#pragma once

#include <list>
#include <string>

#include "funcHelper.hpp"
#include "EventHelper.hpp"

struct TerminatingFunction 
{
public:
    enum State
    {
        Finished = 0,
        Running = 1
    };

    class Data
    {
    public:
        inline Data() = default;
        // inline Data(float deltaTime, float totalTime, bool isForceStop) : deltaTime(deltaTime), totalTime(totalTime), isForceStop(isForceStop) {};

        void setRunning();
        /// @brief NOT required as state is by default finished
        void setFinished();
        State getState() const;
        float getDeltaTime() const;
        float getTotalTime() const;
        /// @returns true when this function will not be called anymore no matter its state (running or finished)
        bool isForceStop() const;
        /// @returns true if this function has been requested to stop
        bool isStopRequested() const;

    protected:
        float m_deltaTime = 0.f;
        float m_totalTime = 0.f;
        bool m_isForceStop = false;
        bool m_requestStop = false;
        State m_state = State::Finished;

        friend TerminatingFunction;
    };

    /// @brief Calling this function will call all Terminating Functions and erase ther from the list if they are finished running
    /// @param deltaTime the deltaTime in seconds
    static void UpdateFunctions(float deltaTime);
    /// @param maxTime is a hard limit on how long a function can run
    /// @note if the function is already added it will be added again
    /// @note when a function is removed or over its limit data will set "isForceStop" to true
    /// @warning The function will only be added if it is valid
    /// @returns the function typeid is "" if function is not valid
    static std::string Add(funcHelper::funcDynamic<Data*> function, float maxTime = std::numeric_limits<float>::infinity());
    /// @brief clears all terminating functions from the list 
    /// @note this gives warning to all functions before clearing (i.e. runs once more before clearing)
    static void clear();
    /// @brief erases the given function from the terminating functions list IF there is one in the list
    /// @note this will still call the function one more time so it has warning of ending
    /// @note removes all functions with the same functionTypeid
    static void remove(const std::string& functionTypeid);
    /// @brief erases the given function from the terminating functions list IF there is one in the list
    /// @note removes all functions with the same functionTypeid
    /// @warning bypasses the final call which gives warning of ending
    static void forceRemove(const std::string& functionTypeid);
    /// @brief requests to stop the given function type id
    /// @returns true if it was requested and false if function not found
    static bool requestStop(const std::string& functionTypeid);

    /// @brief gets the string data from ever terminating function at the moment
    /// @return a list where each item is a list of: function name, total time running, max run time
    static std::list<std::list<std::string>> getStringData();

protected:
    struct _tFunc
    {
        _tFunc(funcHelper::funcDynamic<Data*> func, float maxTime) : m_func(func), m_maxTime(maxTime) {}

        funcHelper::funcDynamic<Data*> m_func;
        float m_totalTime = 0.f;
        float m_maxTime = std::numeric_limits<float>().max();
        bool m_requestStop = false;

        bool operator== (const _tFunc& tFunc) const;
        bool operator== (const std::string& id) const;
        bool operator< (const _tFunc& tFunc) const;
        bool operator< (const std::string& id) const;
        /// @brief removes everything before "setFunction"
        /// @param typeID the typeID of the function
        /// @returns shortened id
        std::string getShortenedID(const std::string& typeID);
        /// @return list with 3 strings (id, total time, max time)
        std::list<std::string> toString();
    };

private:
    inline TerminatingFunction() = default;

    /// @warning you must free the pointer yourself
    static std::list<_tFunc> m_terminatingFunctions;
    static std::mutex m_lock;
};

typedef TerminatingFunction TFunc;
typedef funcHelper::funcDynamic<TFunc::Data*> TFunction;
typedef TerminatingFunction::Data TData;
typedef TerminatingFunction::State TState;

#endif
