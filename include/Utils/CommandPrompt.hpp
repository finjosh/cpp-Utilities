#ifndef COMMANDPROMPT_H
#define COMMANDPROMPT_H

#pragma once

#include <list>

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/EditBox.hpp"
#include "TGUI/Widgets/ListBox.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/GrowVerticalLayout.hpp"
#include "TGUI/Widgets/RichTextLabel.hpp"

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

    // TODO implement tabs, one for commands, warnings, errors, and printing info from the program
    static void print(const std::string& str); // TODO remove this when the tabs are implemented

protected:
    static void UpdateAutoFill();

    /// @brief commits the currently selected auto fill
    /// @param updateAutoFill if true updates the auto fill list
    static void AutoFill(bool updateAutoFill = true);

    static void addLine(const tgui::String& line);

private:
    inline Prompt() = default;

    static tguiCommon::ChildWindow m_windowHandler;

    static tgui::ChildWindow::Ptr m_parent;
    static tgui::EditBox::Ptr m_textBox;
    static tgui::ListBox::Ptr m_autoFillList;
    static tgui::GrowVerticalLayout::Ptr m_lineContainer;

    static bool m_ignoreInputText;
};

}

#endif
