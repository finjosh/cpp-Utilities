#ifndef LIVEVARTEST_H
#define LIVEVARTEST_H

#pragma once

#include <iostream>

//! Depends on SFML Events, FuncHelper, and EventHelper
#include "include/Utils/Debug/LiveVar.h"

class LiveVarTest
{
public:
    static void test();

private:
    inline LiveVarTest() = default;
};

#endif
