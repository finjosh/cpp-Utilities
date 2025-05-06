#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#pragma once

#include <list>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ChatBox.hpp"
#include "TGUI/Widgets/ListBox.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "Utils/CommandHandler.hpp"
#include "Utils/StringHelper.hpp"
#include "Utils/TGUICommon.hpp"

namespace Command
{

/// @note all UI functions only work if the intt function was called
class Prompt
{
public:
    /// @brief initializes the command prompt UI with the given tgui::Gui
    static void init(tgui::Gui& sfmlGui);
    /// @brief initializes the command prompt UI with the given widget as the parent
    static void init(tgui::Container::Ptr parent);
    /// @brief completely removes the command prompt from the gui
    static void close();

    /// @brief used to update the command prompts keyboard events
    /// @returns true if the event was used by the command prompt
    static bool UpdateEvent(const sf::Event& event);

    /// @brief sets the command prompt as visible or not
    /// @param visible the state to set the command prompt to
    static void setVisible(bool visible = true);
    /// @brief focuses the input box for the command prompt
    static void setInputBoxFocused(bool focused = true);

    /// @brief only prints to the command prompt IF the command prompt is set to true
    /// @note setting command prompt to allow prints is only done through the command prompt its self during runtime
    /// @param forcePrint if this should print no matter if printing is allowed
    static void print(const tgui::String& str, bool forcePrint = false);

    static bool isPrintAllowed();

    /// @param print true if printing is allowed
    static void allowPrint(bool print = true);

protected:
    static void UpdateAutoFill();

    /// @brief commits the currently selected auto fill
    /// @param updateAutoFill if true updates the auto fill list
    static void AutoFill(bool updateAutoFill = true);

private:
    inline Prompt() = default;

    static tguiCommon::ChildWindow m_windowHandler;

    static tgui::ChildWindow::Ptr m_parent;
    static tgui::EditBox::Ptr m_textBox;
    static tgui::ListBox::Ptr m_autoFillList;
    static tgui::ChatBox::Ptr m_chatBox; // TODO dont use a chatbox use a list of richtext labels

    static bool m_allowPrint;

    static bool m_ignoreInputText;
};

}

#endif
