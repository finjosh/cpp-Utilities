#ifndef TFUNCDISPLAY_H
#define TFUNCDISPLAY_H

#pragma once

#include "TGUI/Backend/SFML-Graphics.hpp"
#include "TGUI/Widgets/ChildWindow.hpp"
#include "TGUI/Widgets/ListView.hpp"

#include "SFML/Graphics/RenderWindow.hpp"

#include "Utils/TerminatingFunction.hpp"
#include "Utils/TGUICommon.hpp"

#if __has_include("Utils/CommandHandler.hpp")
#include "Utils/CommandHandler.hpp" 
#endif

class TFuncDisplay
{
public:
    /// @brief creates all of the UI needed for the display to work
    /// @param gui used to initalized the UI for the display
    static void init(tgui::Gui& gui);
    /// @brief initializes the TFuncDisplay UI with the given widget as the parent
    static void init(tgui::Container::Ptr parent);
    /// @brief completely removes the TFuncDisplay from the gui
    static void close();

    /// @brief Call this every frame to update the terminating functions being shown
    static void update();

    /// @brief closes and opens the window
    static void setVisible(bool visible = true);
    static bool isVisible();
    static void resetColumnSizes();

protected:
    /// @note only creates commands if the command handler is included
    static inline void initCommands()
    {
        // if command handler is also in use then we add some commands for using the var display
        #ifdef COMMANDHANDLER_H
        tguiCommon::ChildWindow::createOpenCloseCommand("TFunc", m_parent);
        Command::Handler::addSubCommand("TFunc", Command::command{"resetColumnSizes", "Resets the columns to be auto sized", [](){ TFuncDisplay::resetColumnSizes(); }});
        #endif
    }

private:
    inline TFuncDisplay() = default;

    static tguiCommon::ChildWindow m_windowHandler;
    static tgui::ChildWindow::Ptr m_parent;
    static tgui::ListView::Ptr m_list;
};

#endif
