#include "Utils/Debug/TFuncDisplay.hpp"

tguiCommon::ChildWindow TFuncDisplay::m_windowHandler;
tgui::ChildWindow::Ptr TFuncDisplay::m_parent = nullptr;
tgui::ListView::Ptr TFuncDisplay::m_list = nullptr;

void TFuncDisplay::init(tgui::Gui& gui)
{
    m_parent = tgui::ChildWindow::create("Terminating Functions", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    gui.add(m_parent);
    
    m_parent->setSize({"20%","20%"});
    m_parent->setPosition({"70%","0%"});
    m_parent->setResizable(true);
    m_windowHandler.setMinimize_Maximize(m_parent);
    m_windowHandler.setSoftClose(m_parent);
    setVisible(false);

    m_list = tgui::ListView::create();
    m_parent->add(m_list);
    m_list->setSize({"100%","100%"});
    m_list->addColumn("Function Name");
    m_list->setColumnExpanded(0, true);
    m_list->addColumn("    Total Time    ");
    m_list->setColumnAutoResize(1, true);
    m_list->addColumn("    Max Time    ");
    m_list->setColumnAutoResize(2, true);
    m_list->setResizableColumns(true);
    m_list->setAutoScroll(false);

    initCommands();
}

void TFuncDisplay::close()
{
    m_parent = nullptr;
    m_list = nullptr;
}

void TFuncDisplay::update()
{
    if (!m_parent->isVisible() || m_windowHandler.isMinimized(m_parent))
        return;

    float scrollPositionV = m_list->getVerticalScrollbarValue();
    float scrollPositionH = m_list->getHorizontalScrollbarValue();

    m_list->removeAllItems();
    for (auto funcData: TerminatingFunction::getStringData())
    {
        m_list->addItem({tgui::String(funcData.front()), tgui::String(*++funcData.begin()), tgui::String(funcData.back())});
    }

    m_list->setVerticalScrollbarValue(scrollPositionV);
    m_list->setHorizontalScrollbarValue(scrollPositionH);
}

void TFuncDisplay::setVisible(bool visible)
{
    if (visible)
    {
        m_parent->setVisible(true);
        m_parent->setEnabled(true);
        m_parent->moveToFront();
    }
    else
    {
        m_parent->setVisible(false);
        m_parent->setEnabled(false);
    }
}

bool TFuncDisplay::isVisible()
{
    return m_parent->isVisible();
}

void TFuncDisplay::resetColumnSizes()
{
    m_list->setColumnExpanded(0, true);
    m_list->setColumnAutoResize(1, true);
    m_list->setColumnAutoResize(2, true);
}