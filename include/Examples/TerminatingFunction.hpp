#ifndef TERMINATINGFUNCTIONTEST_H
#define TERMINATINGFUNCTIONTEST_H

#pragma once

#include <iostream>

//! Depends on funcHelper
// can be used with command prompt and command handler
//      - adds a command to print all current terminating functions
#include "include/Utils/TerminatingFunction.hpp"

class TerminatingFunctionTest
{
public:
    static void test();

private:
    inline TerminatingFunctionTest() = default;
};

#endif // TERMINATINGFUNCTION_H
