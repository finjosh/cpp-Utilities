#ifndef FUNCHELPERTEST_H
#define FUNCHELPERTEST_H

#pragma once

#include <iostream>

// Note: funcHelper is not dependent on any other class contained in this project
#include "include/Utils/funcHelper.hpp"

class testingClass
{
public:
    testingClass(std::string s) : str(s) {};

    void function()
    {
        std::cout << str << std::endl;
    }

    void function2(int i)
    {
        std::cout << std::to_string(i) << std::endl;
    }

private:
    std::string str;
};

class funcHelperTest
{
public:
    static void test();

private:
    inline funcHelperTest() = default;
};

#endif