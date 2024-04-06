#include "Utils/Debug/VarDisplaySFML.hpp"

std::map<std::string, float> VarDisplay::_vars;
bool VarDisplay::_varChanged = false;

tgui::ChildWindow::Ptr VarDisplay::_parent = nullptr;
float VarDisplay::_parentHeight = 0;
tgui::RichTextLabel::Ptr VarDisplay::_varLabel = nullptr;
tgui::ScrollablePanel::Ptr VarDisplay::_scrollPanel = nullptr;

void VarDisplay::init(tgui::Gui& gui)
{
    // adding any event updates for the new live var
    LiveVar::onVarAdded([](const std::string& name)
    { 
        VarDisplay::addVar(name, LiveVar::getValue(name)); 
        // adding auto updating values for the live vars
        LiveVar::getVarEvent(name)->connect([name]()
        {
            VarDisplay::setVar(name, LiveVar::getValue(name));
        });
    });
    // adding updates for the removed live var
    LiveVar::onVarRemoved([](const std::string& name){
        VarDisplay::removeVar(name);
    });

    _parent = tgui::ChildWindow::create("Debugging Vars", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
    gui.add(_parent);

    _parent->onClosing(&_closeWindow);
    _parent->setSize({"10%","15%"});
    _parent->setPosition({"90%", "0%"});
    _parent->setResizable(true);
    _parent->onMinimize(&VarDisplay::minimizeWindow);
    _parent->onMaximize(&VarDisplay::maximizeWindow);
    _parent->add(tgui::ScrollablePanel::create({_parent->getSize() - tgui::Vector2f(0, _parent->getSharedRenderer()->getTitleBarHeight())}));
    _scrollPanel = _parent->getWidgets().back()->cast<tgui::ScrollablePanel>();
    _parent->onSizeChange([]()
    { 
        _scrollPanel->setSize(_parent->getSize() - tgui::Vector2f(0, _parent->getSharedRenderer()->getTitleBarHeight())); 
        if (int(_parent->getSize().y) > int(_parent->getSharedRenderer()->getTitleBarHeight()))
        {    
            _parentHeight = _parent->getSharedRenderer()->getTitleBarHeight();
            _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
        }
    });

    _varLabel = tgui::RichTextLabel::create();
    _scrollPanel->add(_varLabel);
    _varLabel->setAutoSize(true);
    _varLabel->onSizeChange([](){ VarDisplay::_scrollPanel->setContentSize(VarDisplay::_varLabel->getSize()); });

    bool temp;
    _closeWindow(&temp);

    // if command handler is also in use then we add some commands for using the var display
    #ifdef COMMANDHANDLER_H
    //* adding commands for live vars to the command prompt
    Command::Handler::addCommand(Command::command("lVars", "Contains commands for live variables", {Command::print, "Invalid command\nTrying using \"help lVars\""}, 
        //* sub commands for the lVars
        {
        Command::command("open", "Displays the live variables", {VarDisplay::setVisible}),
        Command::command("close", "Hides the display for live variables", {VarDisplay::setVisible, false}),
        Command::command("get", "[Name] | Gets the value for the given variable", {[](Command::Data* data){
            float temp = LiveVar::getValue(data->getToken());
            if (temp == std::numeric_limits<float>::min())
            {
                data->setReturnStr("Value does not exist");
                data->setReturnColor(Command::WARNING_COLOR);
                return;
            }
            data->setReturnStr(std::to_string(temp));
        }}),
        Command::command("set", "[Name] [value : Float] | Sets the value for the given variable", {[](Command::Data* data){
            float value;
            if (!Command::isValidInput<float>("Invalid amount entered", *data, data->getToken(1), value, std::numeric_limits<float>::min()))
                return;
            if (LiveVar::setValue(data->getToken(), value))
            {
                data->setReturnStr("Variable successfully set");
                data->setReturnColor({0,255,0});
                return;
            }
            data->setReturnColor(Command::INVALID_INPUT_COLOR);
            data->setReturnStr("Invalid name entered");
        }}),
        Command::command("create", "[Name] [initValue = 0] | Creates a new live variable", {[](Command::Data* data){
            float value = StringHelper::toFloat(data->getToken(1), 0);
            
            if (data->getToken(0) != "" && !LiveVar::initVar(data->getToken(0), value))
            {
                data->setReturnStr("Variable already exists");
                data->setReturnColor(Command::INVALID_INPUT_COLOR);
                return;
            }
            data->setReturnStr("Variable successfully created");
            data->setReturnColor({0,255,0});
        }}),
        Command::command("remove", "[Name] | Removes the live variable if it exists", {[](Command::Data* data){
            if (LiveVar::removeVar(data->getToken()))
            {
                data->setReturnStr("Variable successfully removed");
                data->setReturnColor({0,255,0});
                return;
            }
            data->setReturnStr("Invalid name entered");
            data->setReturnColor(Command::INVALID_INPUT_COLOR);
        }})
        }
    ));
    #endif
}

void VarDisplay::close()
{
    _parent = nullptr;
    _varLabel = nullptr;
    _scrollPanel = nullptr;
}

void VarDisplay::Update()
{
    if (_varChanged)
    {
        std::string str = "";
        for (auto i: _vars)
        {
            str += ("<b><i>" + i.first + "</i></b> = " + std::to_string(i.second) + "\n"); 
        }
        if (str.length() > 0)
            str.erase(--str.end());
        
        _varLabel->setText(str);

        _varChanged = false;
    }
}

void VarDisplay::setVisible(bool visible)
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

void VarDisplay::addVar(const std::string& name, const float& value)
{
    _varChanged = true;

    _vars.insert({name, value});
}

void VarDisplay::setVar(const std::string& name, const float& value)
{
    auto iter = _vars.find(name);

    if (iter == _vars.end()) return;

    _varChanged = true;
    iter->second = value;
}

void VarDisplay::removeVar(const std::string& name)
{
    if (_vars.erase(name))
    {
        _varChanged = true;
    }
}

void VarDisplay::_closeWindow(bool* abortTguiClose)
{
    if (abortTguiClose != nullptr)
        (*abortTguiClose) = true;

    _parent->setEnabled(false);
    _parent->setVisible(false);
}

void VarDisplay::minimizeWindow()
{
    setVisible();

    // adding the ability to maximize the VarDisplay without using shortcut
    _parentHeight = _parent->getSize().y;
    _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);

    // reducing the height of the window
    _parent->setHeight(_parent->getSharedRenderer()->getTitleBarHeight());
}

void VarDisplay::maximizeWindow()
{
    _parent->setHeight(_parentHeight);
    _parent->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
}
