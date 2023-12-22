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

# Command Prompt

Firstly, to open and close the command prompt you can use the tilda key (~)

![Screenshot 2023-12-22 105606](https://github.com/finjosh/cpp-Utilities/assets/109707607/b2e0ddac-76d3-40de-bfeb-456b96416b92)

To get a list of commands use the command "help"

![Screenshot 2023-12-22 105639](https://github.com/finjosh/cpp-Utilities/assets/109707607/74438a7b-c2be-46dc-8fc5-285086b4b321)

To find out more about a specific command type "help 'command'"

![Screenshot 2023-12-22 120330](https://github.com/finjosh/cpp-Utilities/assets/109707607/4bb83900-17a1-48e3-807f-225d1c03b468)

![ezgif com-optimize](https://github.com/finjosh/cpp-Utilities/assets/109707607/9786e255-aeed-453c-b2d4-42be619531f5)
