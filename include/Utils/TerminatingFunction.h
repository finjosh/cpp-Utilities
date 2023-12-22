#ifndef TERMINATINGFUNCTION_H
#define TERMINATINGFUNCTION_H

#pragma once

#include <list>
#include <string>

#include "funcHelper.h"

#if __has_include("include/Utils/Debug/CommandHandler.h")
#include "include/Utils/Debug/CommandHandler.h" 
#endif
#if __has_include("include/Utils/Debug/CommandPrompt.h")
#include "include/Utils/Debug/CommandPrompt.h" 
#endif

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

    /// @brief gets the string data from ever terminating function at the moment
    /// @return a list where each item is the data for a function
    static std::list<std::string> getStringData();

    #ifdef COMMANDPROMPT_H
        /// @brief prints the string data to the command prompt every frame
        /// @param print if turn prints every frame
        static void printToPrompt(const bool& print = false);
        /// @brief prints the string data to the command prompt once
        static void printToPromptOnce();
    #endif

    #ifdef COMMANDHANDLER_H
        // must be called separately ass static classes are not initalized in any specific order
        static void initCommands();
    #endif 

protected:
    struct _tFunc
    {
        inline _tFunc(funcHelper::funcDynamic<data*> func) : func(func) {};

        funcHelper::funcDynamic<data*> func;
        float totalTime = 0.f;

        inline bool operator== (const _tFunc& tFunc)
        {
            return func.getTypeid() == tFunc.func.getTypeid();
        }

        inline std::string toString()
        {
            return std::to_string(totalTime) + " " + func.getTypeid();
        }
    };

    // second value is the current time the function has been running for
    static std::list<_tFunc> terminatingFunctions;
private:
    inline TerminatingFunction() = default;
    #ifdef COMMANDPROMPT_H
    static bool _printToPrompt;
    #endif
};

// use this for typedefs that shorten names
namespace TerminatingFunctions
{
    typedef TerminatingFunction TermFunc;
    typedef funcHelper::funcDynamic<TermFunc::data*> TFunction;
    typedef TerminatingFunction::data TData;
    typedef TerminatingFunction::State TState;
}

#endif // TERMINATINGFUNCTION_H
