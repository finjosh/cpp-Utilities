#ifndef UpdateLimiterTest_H
#define UpdateLimiterTest_H

#pragma once

#include <iostream>

#include "include/Utils/UpdateLimiter.h"

// used for showing how updateLimiter works
#include "include/Utils/Stopwatch.h"

class UpdateLimiterTest
{
public:
    static void test();

private:
    inline UpdateLimiterTest() = default;
};

#endif
