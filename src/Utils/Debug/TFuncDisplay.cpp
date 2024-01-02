#include "include/Utils/Debug/TFuncDisplay.hpp"

tgui::ChildWindow::Ptr TFuncDisplay::_parent = nullptr;
tgui::ListView::Ptr TFuncDisplay::_list = nullptr;
float TFuncDisplay::_parentHeight = 0.f;

void TFuncDisplay::init(tgui::Gui& gui)
{
    _parent = tgui::ChildWindow::create("Terminating Functions", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    gui.add(_parent);
    
    _parent->onSizeChange([]()
    { 
        if (int(_parent->getSize().y) > int(_parent->getRenderer()->getTitleBarHeight()))
        {    
            _parentHeight = _parent->getRenderer()->getTitleBarHeight();
            _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
        }
    });
    _parent->setSize({"20%","20%"});
    _parent->setPosition({"70%","0%"});
    _parent->setResizable(true);
    _parent->onClosing(_close);
    _parent->onMinimize(minimize);
    _parent->onMaximize(maximize);
    setVisible(false);

    _list = tgui::ListView::create();
    _parent->add(_list);
    _list->setSize({"100%","100%"});
    _list->addColumn("Function Name");
    _list->setColumnExpanded(0, true);
    _list->addColumn("Total Time    ");
    _list->setColumnAutoResize(1, true);
    _list->setResizableColumns(true);
    _list->setAutoScroll(false);
    _list->setFocusable(false);
    _list->setMultiSelect(false);
    auto listRenderer = _list->getRenderer();
    listRenderer->setSelectedBackgroundColorHover(listRenderer->getBackgroundColor());

    // if command handler is also in use then we add some commands for using the var display
    #ifdef COMMANDHANDLER_H
    Command::Handler::addCommand({"TFunc", "Placeholder", {}, {
        {"open", "Opens the Terminating functions display", {TFuncDisplay::setVisible, true}},
        {"close", "Closes the Terminating Functions display", {TFuncDisplay::setVisible, false}}}});
    #endif
}

void TFuncDisplay::close()
{
    _parent = nullptr;
    _list = nullptr;
}

void TFuncDisplay::update()
{
    if (!_parent->isVisible() || isMinimized())
        return;

    float scrollPositionV = _list->getVerticalScrollbarValue();
    float scrollPositionH = _list->getHorizontalScrollbarValue();

    // TODO only update values instead of removing and adding back
    _list->removeAllItems();
    for (auto funcData: TerminatingFunction::getStringData())
    {
        _list->addItem({funcData.first, funcData.second});
    }

    _list->setVerticalScrollbarValue(scrollPositionV);
    _list->setHorizontalScrollbarValue(scrollPositionH);
}

void TFuncDisplay::setVisible(bool visible)
{
    if (visible)
    {
        _parent->setVisible(true);
        _parent->setEnabled(true);
    }
    else
    {
        _parent->setVisible(false);
        _parent->setEnabled(false);
    }
}

bool TFuncDisplay::isVisible()
{
    return _parent->isVisible();
}

void TFuncDisplay::minimize()
{
    setVisible(true);

    // adding the ability to maximize the TFuncDisplay without using shortcut
    _parentHeight = _parent->getSize().y;
    _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);

    // reducing the height of the window
    _parent->setHeight(_parent->getRenderer()->getTitleBarHeight());
}

void TFuncDisplay::maximize()
{
    _parent->setHeight(_parentHeight);
    _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
}

bool TFuncDisplay::isMinimized()
{
    return int(_parent->getSize().y) == int(_parent->getRenderer()->getTitleBarHeight());
}

void TFuncDisplay::_close(bool* abortTguiClose)
{
    (*abortTguiClose) = true;

    _parent->setEnabled(false);
    _parent->setVisible(false);
}
