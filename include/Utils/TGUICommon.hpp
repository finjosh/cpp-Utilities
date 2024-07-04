#ifndef TGUI_COMMON_H
#define TGUI_COMMON_H

#pragma once

#include "TGUI/Widgets/ChildWindow.hpp"
#if __has_include("Utils/CommandHandler.hpp")
#include "Utils/CommandHandler.hpp" 
#endif

namespace tguiCommon
{

/// @brief common functions that can be used with events for tgui child windows
class ChildWindow
{
public:
    /// @brief Makes the window not visible and does NOT remove it from the UI
    /// @param window the window to update
    static void setSoftClose(tgui::ChildWindow::Ptr window);
    /// @brief sets the given window visible or not
    /// @note enables, sets visible, and moves to front
    /// @param window the window to set state
    /// @param visible visible or not
    static void setVisible(tgui::ChildWindow::Ptr window, bool visible);
    /// @note maximizing will make the window fullscreen
    /// @note locks the window postion when maximized
    /// @note if x and y layout is "100%" then assumes that the window is maximized
    /// @note does NOT work with setMinimizeWindow
    /// @warning this must be accessible until the events are reset on window OR the window is removed from the GUI
    void setMaximize(tgui::ChildWindow::Ptr window);
    /// @note minimize will show only the window bar
    /// @note maximize will show the previous size of the window
    /// @note does NOT work with setMaximizeWindow
    /// @warning this must be accessible until the events are reset on window OR the window is removed from the GUI
    void setMinimize_Maximize(tgui::ChildWindow::Ptr window);
    /// @brief checks if the window is minimized
    /// @note the window is minimized if the maximize button is showing
    /// @param window the window to check
    static bool isMinimized(tgui::ChildWindow::Ptr window);
    #ifdef COMMANDHANDLER_H
    /// @brief creates the command for opening and closing this window
    /// @note uses the set visible command (requires the window to always be in the GUI)
    /// @param windowName the name of the window in the command prompt
    static inline void createOpenCloseCommand(const std::string& windowName, tgui::ChildWindow::Ptr window)
    {
        Command::Handler::addCommand(Command::command(windowName, "Commands for the " + windowName + " window", 
                                    [windowName](Command::Data* data){ data->setReturnStr("Try using 'help " + windowName + "'"); }, {},
                                    {Command::command("open", "Opens the window", [window](){ ChildWindow::setVisible(window, true); }),
                                     Command::command("close", "Opens the window", [window](){ ChildWindow::setVisible(window, false); })}));
    }
    #endif

protected:
    void _maximizeWindow(tgui::ChildWindow::Ptr window);
    void _minimizeWindow(tgui::ChildWindow::Ptr window);
    void _minimizeMaximize(tgui::ChildWindow::Ptr window);
    static void _softClose(tgui::ChildWindow::Ptr window, bool* abortTguiClose);

private:
    tgui::Layout2d m_position; // the position when the window was minimized
    tgui::Layout2d m_size; // the size when the window was minimized
};

}

#endif
