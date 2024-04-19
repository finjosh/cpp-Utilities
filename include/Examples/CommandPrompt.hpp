#ifndef COMMANDPROMPTTEST_H
#define COMMANDPROMPTTEST_H

#pragma once

// Used for testing
#include "include/Utils/TerminatingFunction.hpp"

//! Depends on SFML Graphics, StringHelper, and CommandHandler
#include "include/Utils/Debug/CommandPrompt.hpp"

class CommandPromptTest
{
public:
    static void test();

private:
    inline CommandPromptTest() = default;
};

#endif
