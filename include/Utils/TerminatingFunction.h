#ifndef TERMINATINGFUNCTION_H
#define TERMINATINGFUNCTION_H

#pragma once

#include <list>
#include <string>

#include "funcHelper.h"

struct TerminatingFunction 
{
public:
    enum State
    {
        Finished = 0,
        Running = 1
    };

    struct data
    {
        inline data(const float& deltaTime, const float& totalTime) : deltaTime(deltaTime), totalTime(totalTime) {};
        const float deltaTime = 0.f;
        const float totalTime = 0.f;
        State state = State::Finished;
    };

    /// @brief Calling this function will call all Terminating Functions and erase ther from the list if they are finished running
    /// @param deltaTime the deltaTime in seconds
    static void UpdateFunctions(float deltaTime);
    /// @note if the function is already added it will be added again
    /// @warning The function will only be added if it is valid
    /// @returns the function typeid is "" if function is not valid
    static std::string Add(funcHelper::funcDynamic<data*> function);
    /// @brief clears all terminating functions from the list 
    static void clear();
    /// @brief erases the given function from the terminating functions list IF there is one in the list
    /// @warning removes all functions with the same functionTypeid
    static void remove(const std::string& functionTypeid);

protected:
    struct _tFunc
    {
        inline _tFunc(funcHelper::funcDynamic<data*> func) : func(func) {};

        funcHelper::funcDynamic<data*> func;
        float totalTime = 0.f;

        bool operator== (const _tFunc& tFunc)
        {
            return func.getTypeid() == tFunc.func.getTypeid();
        }
    };

    // second value is the current time the function has been running for
    static std::list<_tFunc> terminatingFunctions;
private:
    inline TerminatingFunction() = default;
};

// use this for typedefs that shorten names
namespace TerminatingFunctions
{
    typedef TerminatingFunction TermFunc;
    typedef funcHelper::funcDynamic<TermFunc::data*> TFunction;
    typedef TerminatingFunction::data TData;
}

#endif // TERMINATINGFUNCTION_H
