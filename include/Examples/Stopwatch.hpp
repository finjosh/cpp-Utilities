#ifndef STOPWATCHTEST_H
#define STOPWATCHTEST_H

#pragma once

#include <iostream>
#include <thread>

#include "include/Utils/Stopwatch.hpp"

class StopwatchTest
{
public:
   static void test();

private:
   inline StopwatchTest() = default;
};

#endif
