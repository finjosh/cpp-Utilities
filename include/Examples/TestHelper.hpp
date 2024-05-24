#ifndef TESTHELPERTEST_H
#define TESTHELPERTEST_H

#pragma once

#include <iostream>

//! Depends on SFML/Window, SFML/Graphics, TGUI, iniParser, Stopwatch, funcHelper, Graph
#include "include/Utils/TestHelper.hpp"

class TestHelperTest
{
public:
    static void test();

private:
    inline TestHelperTest() = default;
};

#endif
