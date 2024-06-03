#ifndef VARDISPLAY_H
#define VARDISPLAY_H

#pragma once

#include <map>
#include <algorithm>

#include "LiveVar.hpp" 
#include "Utils/TGUICommon.hpp"

#if __has_include("CommandHandler.hpp")
#include "CommandHandler.hpp" 
#endif

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/RichTextLabel.hpp"
#include "TGUI/Widgets/ScrollablePanel.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

/**
 * @brief auto adds any var that is made in live var and auto updates when that var is changed
*/
class VarDisplay
{
public:
    /// @brief creates all of the UI needed for the display to work
    /// @param gui used to initalized the UI for the display
    static void init(tgui::Gui& gui);
    /// @brief removes all of the pointers from storage so the program can close without a crash
    static void close();

    /// @brief updates the live vars
    /// @note call this every frame
    static void Update();

    static void setVisible(bool visible = true);

protected:
    /// @brief adds a var to the list
    /// @note used with the events from live vars
    static void addVar(const std::string& name, float value);
    /// @brief sets the value of a var 
    /// @note used with the events from live vars
    static void setVar(const std::string& name, float value);
    /// @brief removes a var from the list
    /// @note used with the events from live vars
    static void removeVar(const std::string& name);
    
private:
    VarDisplay() = default;

    static inline void initCommands()
    {
        // if command handler is also in use then we add some commands for using the var display
        #ifdef COMMANDHANDLER_H
        tguiCommon::ChildWindow::createOpenCloseCommand("lVars", m_parent);
        LiveVar::initCommand();
        #endif
    }

    static std::map<std::string, float> m_vars;
    static bool m_varChanged;

    static tguiCommon::ChildWindow m_windowHandler;
    static tgui::ChildWindow::Ptr m_parent;
    static tgui::RichTextLabel::Ptr m_varLabel;
    static tgui::ScrollablePanel::Ptr m_scrollPanel;
};

#endif
