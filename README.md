# cpp-Utilities
C++ utility classes that I made while working on a personal project

NOTE: Some of the classes are built for the "Simple and Fast Multimedia Library" ([SFML](https://www.sfml-dev.org/index.php)) and the "Texus' Graphical User Interface" ([TGUI](https://tgui.eu/))

Tested with: 
  - Compiler: MSYS2
  - Version: g++.exe (Rev3, Built by MSYS2 project) 12.1.0

SFML:
  - Version: 2.6.1

TGUI:
  - Version: 1.1

# Ultilies breakdown
| File | Dependencies | Breif Description |
| --- | --- | --- |
| funcHelper.hpp | Makes taking, storing, and using functions as variables easier. Functions set can have dynamic data given to them when the function is invoked. | None |
| EventHelper.hpp | Event class with thread-safety. Functions set to an event can have dynamic data given when the event is invoked. | None |
| StringHelper.hpp | Simple class for common string operations like trimming and converting char to lowercase. Includes functions for converting a string to float, int, etc. | None |
| UpdateLimiter.hpp | Used to limit the speed of a thread | None |
| Stopwatch.hpp | Used to check how long operations take | None |
| Log.hpp | Simple class that prints logs into a file | None |
| iniParser.hpp | Easy to use ini parser | StringHelper.hpp |
| CommandHandler.hpp | Used to create and use commands. The command handler also has the ability to search commands. | funcHelper.hpp, StringHelper.hpp |
| CommandPrompt.hpp | Simple command prompt that uses the command handler as its backend | CommandHandler.hpp, StringHelper.hpp, TGUI |
| TerminatingFunction.hpp | Functions that will be called every frame until it says it is done running | funcHelper.hpp, CommandHandler.hpp (optional), CommandPrompt.hpp (optional) |
| TFuncDisplay.hpp | Simple display for terminating functions for debugging | TerminatingFunction.hpp, TGUI (dev build from 12/30/2023 or later), CommandHandler.hpp (optional) |
| LiveVar.hpp | Live variables are used for debugging and testing | EventHelper.hpp, funcHelper.hpp, SFML Events, CommandHandler.hpp (optional) |
| VarDisplay.hpp | Simple display for live vars, which shows all the current live vars and their values | LiveVar.hpp, TGUI, CommandHandler.hpp (optional) |

# A preview of a few utilities

## Command Prompt

Firstly, to open and close the command prompt, you can use the tilde key (~)

![Screenshot 2023-12-22 105606](https://github.com/finjosh/cpp-Utilities/assets/109707607/b2e0ddac-76d3-40de-bfeb-456b96416b92)

To get a list of commands, use the command "help"

![Screenshot 2023-12-22 105639](https://github.com/finjosh/cpp-Utilities/assets/109707607/74438a7b-c2be-46dc-8fc5-285086b4b321)

To find out more about a specific command, type "help 'command'"

![Screenshot 2023-12-22 120330](https://github.com/finjosh/cpp-Utilities/assets/109707607/4bb83900-17a1-48e3-807f-225d1c03b468)

![ezgif com-optimize](https://github.com/finjosh/cpp-Utilities/assets/109707607/9786e255-aeed-453c-b2d4-42be619531f5)

## Var Display and Live Var

Live vars can have key binds set to them for specific values or to incremenet them.

Example:

![ezgif com-video-to-gif-converted](https://github.com/finjosh/cpp-Utilities/assets/109707607/cdb428d4-5b0c-455b-911b-3e4023e45b8c)

Example usage with command prompt:

https://github.com/finjosh/cpp-Utilities/assets/109707607/58003d68-5a51-4cf6-a753-c7578461bf1c

## Terminating Functions Display

Display for seeing the name and total time of each terminating function

![Screenshot 2023-12-23 165722](https://github.com/finjosh/cpp-Utilities/assets/109707607/e9649332-6aa2-4803-aa5f-f84c1e2fb868)

![TFuncDisplay](https://github.com/finjosh/cpp-Utilities/assets/109707607/02feac9a-3e17-4246-978f-5aca69cb8229)

