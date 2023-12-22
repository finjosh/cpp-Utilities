# cpp-Utilities
C++ utility classes that I made while working on a personal project

NOTE: Some of the classes are built for the "Simple and Fast Multimedia Library" ([SFML](https://www.sfml-dev.org/index.php)) and the "Texus' Graphical User Interface" ([TGUI](https://tgui.eu/))

Tested with: 
  - Compiler: MSYS2
  - Version: g++.exe (Rev3, Built by MSYS2 project) 12.1.0

SFML:
  - Version: 2.6.0

TGUI:
  - Version: 1.0

# Ultilies breakdown
| Class | Dependencies |
| --- | --- |
| funcHelper.h | None |
| EventHelper.h | None |
| StringHelper.h | None |
| UpdateLimiter.h | None |
| Stopwatch.h | None |
| Log.h | None |
| iniPareser.h | StringHelper.h |
| CommandHandler.h | funcHelper.h, StringHelper.h |
| CommandPrompt.h | CommandHandler.h, StringHelper.h, TGUI |
| TerminatingFunction.h | funcHelper.h, (optinal) CommandHandler.h, (optinal) CommandPrompt.h |
| LiveVar.h | EventHelper.h, funcHelper.h, SFML Events |
| VarDisplay.h | LiveVar.h, TGUI, (optinal) CommandHandler.h |

