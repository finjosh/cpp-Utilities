#ifndef TERMINATINGFUNCTION_H
#define TERMINATINGFUNCTION_H

#pragma once

#include <unordered_set>
#include <string>

#include "funcHelper.h"

// TODO update this to work more like how commands work in the command handler
class TerminatingFunction
{
public:
    enum StateType
    {
        Finished = 0,
        Running = 1
    };

    /// @brief Calling this function will call all Terminating Functions and erase ther from the list if they are finished running
    /// @param deltaTime the deltaTime in seconds
    static void UpdateFunctions(float deltaTime);
    /// @param replace if there is already the same function currently running does it get replaced
    /// @warning The function will only be added if it is valid
    /// @returns the function typeid or "" if function is not valid
    static std::string Add(funcHelper::func<StateType> function, bool replace = false);
    /// @brief clears all terminating functions from the list 
    static void clear();
    /// @brief erases the given function from the terminating functions list IF there is one in the list
    /// @warning removes all functions with the same functionTypeid
    static void erase(const std::string& functionTypeid);
    /// @brief returns the deltaTime from the last time it was updated
    /// @warning does NOT update unless the UpdateFunctions function is called
    static float getDeltaTime();
private:
    static float deltaTime;
    static std::unordered_multiset<funcHelper::func<StateType>> terminatingFunctions;
};

typedef funcHelper::func<TerminatingFunction::StateType> TFunc;
typedef TerminatingFunction TF;

#endif // TERMINATINGFUNCTION_H
