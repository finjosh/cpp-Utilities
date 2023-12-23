#ifndef UpdateLimiterTest_H
#define UpdateLimiterTest_H

#pragma once

#include <iostream>

#include "include/Utils/UpdateLimiter.hpp"

// used for showing how updateLimiter works
#include "include/Utils/Stopwatch.hpp"

class UpdateLimiterTest
{
public:
    static void test();

private:
    inline UpdateLimiterTest() = default;
};

#endif
