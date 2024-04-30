#ifndef TGUI_COMMON_H
#define TGUI_COMMON_H

#pragma once

#include "TGUI/Widgets/ChildWindow.hpp"

namespace tguiCommon
{

/// @brief common functions that can be used with events for tgui child windows
class ChildWindow
{
public:
    /// @brief closes the window without removing it from the gui
    /// @note you must give the child window ptr while adding to the "onClosing" event
    /// @param abortTGUIClose used by the tgui event
    /// @param window used by this function to disable and make the window invisible
    static void closeWindow(tgui::ChildWindow::Ptr window, bool* abortTguiClose);
    /// @brief sets the given window visible or not
    /// @note enables, sets visible, and moves to front
    /// @param window the window to set state
    /// @param visible visible or not
    static void setVisible(tgui::ChildWindow::Ptr window, const bool& visible);
    /// @brief this will handle maximizing button for the given window
    /// @note locks the window postion when maximized
    /// @warning if x and y layout is "100%" then assumes that the window is maximized
    void maximizeWindow(tgui::ChildWindow::Ptr window);

private:
    tgui::Layout2d m_position; // the position when the window was minimized
    tgui::Layout2d m_size; // the size when the window was minimized
};

}

#endif
