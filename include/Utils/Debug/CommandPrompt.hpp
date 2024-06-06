#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#pragma once

#include <list>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ChatBox.hpp"
#include "TGUI/Widgets/ListBox.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "Utils/Debug/CommandHandler.hpp"
#include "Utils/StringHelper.hpp"
#include "Utils/TGUICommon.hpp"

namespace Command
{

class Prompt
{
public:
    /// @brief initializes the command prompt UI with the given tgui::Gui
    static void init(tgui::Gui& sfmlGui);
    /// @brief used to erase all pointers to UI in tgui so the program can close without a crash
    static void close();

    /// @brief used to update the command prompts keyboard events
    static void UpdateEvent(const sf::Event& event);

    /// @brief sets the Command::color default to the current tgui theme default
    static void UpdateDefaultColor();

    /// @brief sets the command prompt as visible or not
    /// @param visible the state to set the command prompt to
    static void setVisible(bool visible = true);

    /// @brief only prints to the command prompt IF the command prompt is set to true
    /// @note setting command prompt to allow prints is only done through the command prompt its self during runtime
    static void print(const tgui::String& str, Command::color color = Command::color());

    static bool isPrintAllowed();

    /// @param print true if printing is allowed
    static void allowPrint(bool print = true);
    static void setMaxHistory(size_t size = 64);
    static size_t getMaxHistory();
    static void clearHistory();

protected:
    // static void MaximizePrompt();
    // /// @brief handles the resizing of the prompt
    // static void ResizePrompt();

    static void UpdateAutoFill();

    /// @brief commits the currently selected auto fill
    /// @param updateAutoFill if true updates the auto fill list
    static void AutoFill(bool updateAutoFill = true);

    static void addHistory(const std::string& command);

    // /// @brief closes the command prompts window without removing it from tgui
    // /// @param abortTguiClose set to nullptr if not used with tgui
    // static void _close(bool* abortTguiClose);

private:
    inline Prompt() = default;

    static tguiCommon::ChildWindow m_windowHandler;

    static tgui::ChildWindow::Ptr m_parent;
    static tgui::EditBox::Ptr m_textBox;
    static tgui::ListBox::Ptr m_autoFillList;
    static tgui::ChatBox::Ptr m_chatBox;

    static std::list<std::string> m_commandHistory;
    static size_t m_maxHistory;

    static bool m_allowPrint;
};

}

#endif
