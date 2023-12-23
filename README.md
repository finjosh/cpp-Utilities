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
| Class | Dependencies |
| --- | --- |
| funcHelper.hpp | None |
| EventHelper.hpp | None |
| StringHelper.hpp | None |
| UpdateLimiter.hpp | None |
| Stopwatch.hpp | None |
| Log.hpp | None |
| iniParser.hpp | StringHelper.hpp |
| CommandHandler.hpp | funcHelper.hpp, StringHelper.hpp |
| CommandPrompt.hpp | CommandHandler.hpp, StringHelper.hpp, TGUI |
| TerminatingFunction.hpp | funcHelper.hpp, (optional) CommandHandler.hpp, (optional) CommandPrompt.hpp |
| TFuncDisplay.hpp | TerminatingFunction.hpp, TGUI, (optional) CommandHandler.hpp |
| LiveVar.hpp | EventHelper.hpp, funcHelper.hpp, SFML Events |
| VarDisplay.hpp | LiveVar.hpp, TGUI, (optional) CommandHandler.hpp |

# A preview of a few utilities

## Command Prompt

Firstly, to open and close the command prompt you can use the tilda key (~)

![Screenshot 2023-12-22 105606](https://github.com/finjosh/cpp-Utilities/assets/109707607/b2e0ddac-76d3-40de-bfeb-456b96416b92)

To get a list of commands use the command "help"

![Screenshot 2023-12-22 105639](https://github.com/finjosh/cpp-Utilities/assets/109707607/74438a7b-c2be-46dc-8fc5-285086b4b321)

To find out more about a specific command type "help 'command'"

![Screenshot 2023-12-22 120330](https://github.com/finjosh/cpp-Utilities/assets/109707607/4bb83900-17a1-48e3-807f-225d1c03b468)

![ezgif com-optimize](https://github.com/finjosh/cpp-Utilities/assets/109707607/9786e255-aeed-453c-b2d4-42be619531f5)

## Var Display

Live vars can have keybinds set to them for specific values or to incremenet them

Example:

![ezgif com-video-to-gif-converted](https://github.com/finjosh/cpp-Utilities/assets/109707607/cdb428d4-5b0c-455b-911b-3e4023e45b8c)

Example usage with command prompt:

https://github.com/finjosh/cpp-Utilities/assets/109707607/58003d68-5a51-4cf6-a753-c7578461bf1c

## Terminating Functions Display


