#include "Utils/TGUICommon.hpp"

using namespace tguiCommon;

void ChildWindow::setMaximize(tgui::ChildWindow::Ptr window)
{
    if (!window) return;
    window->setTitleButtons(tgui::ChildWindow::TitleButton::Maximize | window->getTitleButtons());
    window->onMaximize(_maximizeWindow, this);
}

bool ChildWindow::isMinimized(tgui::ChildWindow::Ptr window)
{
    return (window->getTitleButtons() & tgui::ChildWindow::TitleButton::Maximize) == tgui::ChildWindow::TitleButton::Maximize;
}

void ChildWindow::_maximizeWindow(tgui::ChildWindow::Ptr window)
{
    if (window->getSizeLayout().x.toString() != "100%" || window->getSizeLayout().y.toString() != "100%" || m_size.x.getValue() == 0 || m_size.y.getValue() == 0)
    {
        m_size = window->getSizeLayout();
        m_position = window->getPositionLayout();
        window->setPosition({0,0});
        window->setPositionLocked(true);
        window->setSize({"100%", "100%"});
    }
    else
    {
        window->setSize(m_size);
        window->setPosition(m_position);
        window->setPositionLocked(false);
    }
}

void ChildWindow::setMinimize_Maximize(tgui::ChildWindow::Ptr window)
{
    if (!window) return;
    window->setTitleButtons(tgui::ChildWindow::TitleButton::Minimize | window->getTitleButtons() & ~tgui::ChildWindow::Maximize);
    window->onMinimize(_minimizeWindow, this);
    window->onMaximize(_minimizeMaximize, this);
}

void ChildWindow::_minimizeWindow(tgui::ChildWindow::Ptr window)
{
    window->setTitleButtons(tgui::ChildWindow::TitleButton::Maximize | window->getTitleButtons() & ~tgui::ChildWindow::Minimize);
    window->setResizable(false);
    m_size = window->getSize();
    window->setHeight(window->getSharedRenderer()->getTitleBarHeight() + 1);
}

void ChildWindow::_minimizeMaximize(tgui::ChildWindow::Ptr window)
{
    window->setResizable(true);
    window->moveToFront();
    window->setTitleButtons(tgui::ChildWindow::TitleButton::Minimize | window->getTitleButtons() & ~tgui::ChildWindow::Maximize);
    window->setSize(m_size);
}

void ChildWindow::setSoftClose(tgui::ChildWindow::Ptr window)
{
    if (!window) return;
    window->onClosing(_softClose, window);
}

void ChildWindow::_softClose(tgui::ChildWindow::Ptr window, bool* abortTguiClose)
{
    window->setEnabled(false);
    window->setVisible(false);
    if (abortTguiClose != nullptr)
        *abortTguiClose = true;
}

void ChildWindow::setVisible(tgui::ChildWindow::Ptr window, bool visible)
{
    if (!window) return;
    if (visible)
    {
        window->setVisible(true);
        window->setEnabled(true);
        window->moveToFront();
    }
    else
    {
        window->setVisible(false);
        window->setEnabled(false);
    }
}
