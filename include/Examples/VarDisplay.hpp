#ifndef VARDISPLAYTEST_H
#define VARDISPLAYTEST_H

#pragma once

// used for testing
#include "include/Utils/TerminatingFunction.hpp"

//! Depends on LiveVar, SFML Graphics, TGUI, and can be used in combination with CommandHandler and CommandPrompt
#include "include/Utils/Debug/VarDisplay.hpp"

class VarDisplayTest
{
public:
    static void test();

private:
    inline VarDisplayTest() = default;
};

#endif
