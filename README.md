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

# Utilities breakdown
| File | Dependencies | Brief Description |
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
Note that the theme can be set to any TGUI theme, and the utilities UI will update accordingly. 

## Command Prompt

Firstly, to open and close the command prompt, you can use the tilde key (~)

![image](https://github.com/finjosh/cpp-Utilities/assets/109707607/990e7045-e504-4d80-b135-ae33b1eaf529)

To get a list of commands, use the command "help"

![image](https://github.com/finjosh/cpp-Utilities/assets/109707607/199cc998-220e-43eb-b51b-314a8216f380)

To find out more about a specific command, type "help 'command'"

![image](https://github.com/finjosh/cpp-Utilities/assets/109707607/2ab067a8-ab5e-428e-bdf8-98fbda065fe0)

![Untitledvideo-MadewithClipchamp-ezgif com-video-to-gif-converter](https://github.com/finjosh/cpp-Utilities/assets/109707607/0b10f62a-a905-44c3-ab3f-91f41092c9d2)

## Var Display and Live Var

Live vars can have key binds set to them for specific values or to increment them.

Example:

![2024-01-1419-44-59-ezgif com-video-to-gif-converter](https://github.com/finjosh/cpp-Utilities/assets/109707607/de21c7f9-2e3f-4d26-a9f4-829ef0962d87)

Example usage with command prompt:

https://github.com/finjosh/cpp-Utilities/assets/109707607/cb718067-b088-4c1c-b04a-90767adf1ef5

## Terminating Functions Display

Display for seeing the name and total time of each terminating function

![image](https://github.com/finjosh/cpp-Utilities/assets/109707607/6ccbe7e5-0fef-4375-9220-54400852c50b)

![Untitledvideo-MadewithClipchamp1-ezgif com-video-to-gif-converter](https://github.com/finjosh/cpp-Utilities/assets/109707607/976373e7-f805-4e30-959e-7b8c9aef7a17)
