#ifndef TFUNCDISPLAYTEST_H
#define TFUNCDISPLAYTEST_H

#pragma once

//! Depends on TerminatingFunction, SFML Graphics, TGUI, and can be used in combination with CommandHandler and CommandPrompt
#include "include/Utils/Debug/TFuncDisplaySFML.hpp"

class TFuncDisplayTest
{
public:
    static void test();

private:
    inline TFuncDisplayTest() = default;
};

#endif