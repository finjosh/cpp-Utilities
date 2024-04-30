#include "Utils/TGUICommon.hpp"

using namespace tguiCommon;

void ChildWindow::maximizeWindow(tgui::ChildWindow::Ptr window)
{
    if (window->getSizeLayout().x.toString() != "100%" && window->getSizeLayout().y.toString() != "100%")
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

void ChildWindow::closeWindow(tgui::ChildWindow::Ptr window, bool* abortTguiClose)
{
    window->setEnabled(false);
    window->setVisible(false);
    if (abortTguiClose != nullptr)
        *abortTguiClose = true;
}

void ChildWindow::setVisible(tgui::ChildWindow::Ptr window, const bool& visible)
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
