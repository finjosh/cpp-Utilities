#include "include/Utils/Debug/CommandPrompt.h"

tgui::ChildWindow::Ptr CommandPrompt::_parent{nullptr};
tgui::EditBox::Ptr CommandPrompt::_textBox{nullptr};
tgui::ListBox::Ptr CommandPrompt::_autoFillList{nullptr};
tgui::ChatBox::Ptr CommandPrompt::_chatBox{nullptr};

std::list<std::string> CommandPrompt::_commandHistory;
size_t CommandPrompt::_maxHistory = 64;

bool CommandPrompt::_allowPrint = true;

tgui::Layout2d CommandPrompt::_parentSize{"25%", "25%"};
tgui::Layout2d CommandPrompt::_parentPos{0,0};

void CommandPrompt::init(tgui::Gui& sfmlGui)
{
    _parent = tgui::ChildWindow::create("Command Prompt", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    sfmlGui.add(_parent);

    // * setup of child window
    _parent->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Left);
    _parent->setResizable();
    _parent->setSize({"25%","25%"});
    _parent->setPosition({"100% - 25%","100% - 25%"});
    {
        auto renderer = _parent->getRenderer();
        renderer->setTitleColor(tgui::Color::White);
        renderer->setBorderColor({50,50,50,255});
        renderer->setBorderColorFocused({75,75,75,255});
        renderer->setTitleBarColor(tgui::Color::Black);
        renderer->setBackgroundColor(tgui::Color::Black);
    }

    // * setup for edit box
    {
        _textBox = tgui::EditBox::create();
        _parent->add(_textBox);
        _textBox->setSize({"100%", "25"});
        _textBox->setPosition({0, "100% - 25"});
        auto _textBoxRender = _textBox->getRenderer();
        _textBoxRender->setTextColor(tgui::Color::White);
        _textBoxRender->setCaretColor({100,100,100,255});
        _textBoxRender->setCaretColorFocused(tgui::Color::White);
        _textBoxRender->setBorderColor({50,50,50,255});
        _textBoxRender->setBorderColorFocused({75,75,75,255});
        _textBoxRender->setBackgroundColor(tgui::Color::Black);
        _textBoxRender->setBackgroundColorHover({50,50,50,255});

        // * setup for auto closing _autoFillList when focus is lost
        _textBox->onUnfocus([]()
        {
            if (_autoFillList)
            {
                _autoFillList->setVisible(false);
            }
        });

        // * setup for chat _textBox
        _chatBox = tgui::ChatBox::create();
        _parent->add(_chatBox);
        _chatBox->setSize({"100%", "100%-25"});
        _chatBox->setLinesStartFromTop();
        _chatBox->setLineLimit(1028);
        auto chatRender = _chatBox->getRenderer();
        chatRender->setBorderColor(tgui::Color::Transparent);
        chatRender->setBackgroundColor({15,15,15,255});
        chatRender->setPadding(5);
        _chatBox->setTextColor(tgui::Color::White);
        _chatBox->setFocusable(false);

        // * setup for the command prompt custom commands
        {
            auto temp = _chatBox.get();
            Command::Handler::addCommand({"cp", "Prefix for any Command Prompt specific commands", {[](Command::Data* input)
            { 
                input->setReturnStr("Not a valid cp command. Try using 'help cp' for more info"); 
            }},
            // list of sub commands
            {
                Command::command("clear", "clears the command prompt", {[temp](){ temp->removeAllLines(); }}),
                Command::command("setMaxLines", "sets the max number of lines the command prompt will hold (min value of 50)", {[temp](Command::Data* input)
                { 
                    unsigned long t;
                    if (Command::isValidInput<unsigned long>("Please enter a valid number (min - 50)", *input, input->getToken(0), t, 0, 
                            [](const unsigned long& i){ return i < 50; }))
                    {
                        temp->setLineLimit(t);
                        input->setReturnStr("Max lines was successfully set to: " + input->getToken(0));
                        input->setReturnColor({0,255,0,255});
                    } 
                }}),
                Command::command("getMaxLines", "retuns the current max lines", {[temp](Command::Data* input){
                    input->setReturnStr(std::to_string(temp->getLineLimit()));
                }}),
                Command::command("allowPrinting", "controls if printing to the command prompt is allowed (true/false or 1/0)", {[](Command::Data* input)
                {
                    _allowPrint = StringHelper::toBool(input->getToken(0), _allowPrint);
                    input->setReturnStr("Printing is now: ");
                    input->addToReturnStr(_allowPrint ? "allowed" : "not allowed");
                }}),
                Command::command("getPrintingAllowed", "prints if printing is allowed", {[](Command::Data* input)
                {
                    input->setReturnStr(_allowPrint ? "True" : "False");
                }}),
                Command::command("clearHistory", "clears the command history", {[](Command::Data* data){ CommandPrompt::clearHistory(); data->setReturnStr("History Cleared"); data->setReturnColor({0,255,0}); }}),
                Command::command("getMaxHistory", "prints the max number of commands in history", {[](Command::Data* data){ Command::print(std::to_string(CommandPrompt::getMaxHistory()), data); }}),
                Command::command("setMaxHistory", "sets the max number of commands in history", {[](Command::Data* data){
                    unsigned long temp = 0;
                    if (!Command::isValidInput<unsigned long>("Invalid max entered", *data, data->getToken(), temp, 64))
                        return;
                    CommandPrompt::setMaxHistory(temp);
                    data->setReturnStr("Max history set successfully");
                    data->setReturnColor({0,255,0});
                }}),
                { "getRandom", "[min = 0] [max = " + std::to_string(RAND_MAX) + " ] [amount = 1]" +
                            " | prints n random numbers in the following syntax: \"a(1) a(2) a(3) a(4) ... a(n)\" (hard max of " + std::to_string(RAND_MAX) + " and hard min of 0)",
                    {[](Command::Data* input)
                    {
                        int min = 0;
                        int max = RAND_MAX;

                        if (!Command::isValidInput<int>("Invalid min entered", *input, input->getToken(0), min, -1, [](int& v){ return v >= 0;}) 
                            || !Command::isValidInput<int>("Invalid max entered", *input, input->getToken(1), max, -1, [&max, &min](int& v){ return !(v < 0 || v > max || min > max);}))
                            return;

                        unsigned long amount = 1;
                        if (input->getNumOfTokens() == 3)
                            Command::isValidInput<unsigned long>("Invalid amount entered - Defaulted to 1", *input, input->getToken(2), amount, 1, [](unsigned long& v){ return v > 0; });

                        while (amount != 0)
                        {
                            input->addToReturnStr(std::to_string((min + (rand())%(max+1 - min))) + " ");
                            amount--;
                        } 
                    }}
                }
                // ,Command::command("print", "", {[](Command::Data* data){ for (auto s: data->getTokens()) { data->addToReturnStr(s); } }})
            }});
        }
        // * ---------------------------

        // * setup for auto fill
        _autoFillList = tgui::ListBox::create();
        _parent->add(_autoFillList);
        _autoFillList->setSize({"100%", "0"});
        _autoFillList->setAutoScroll(false);
        auto listRender = _autoFillList->getRenderer();
        listRender->setBackgroundColor(tgui::Color::Black);
        listRender->setSelectedBackgroundColor({50,50,50,255});
        listRender->setBackgroundColorHover({50,50,50,255});
        listRender->setBorderColor({50,50,50,255});
        listRender->setTextColor(tgui::Color::White);
        listRender->setTextColorHover(tgui::Color::White);
        _autoFillList->setOrigin(0,1);
        _autoFillList->setPosition({"0"}, {"100%-" + std::to_string(_textBox->getSize().y)});
        _autoFillList->setFocusable(false);

        _textBox->onTextChange(CommandPrompt::UpdateAutoFill);
        _textBox->onFocus([]()
        {
            CommandPrompt::UpdateAutoFill(); 
            _textBox->onFocus.setEnabled(false);
        });
        _textBox->onUnfocus([]()
        {
            if (_textBox)
                _textBox->onFocus.setEnabled(true);
        });

        _textBox->onReturnKeyPress([]()
        {
            if (_textBox->getText().size() == 0 && _autoFillList->isVisible())
                _textBox->setText(_autoFillList->getSelectedItem());

            _chatBox->addLine("> " + _textBox->getText(), tgui::Color::White, tgui::TextStyle::Bold);

            auto commandData = Command::Handler::callCommand(_textBox->getText().toStdString());
            CommandPrompt::addHistory(_textBox->getText().toStdString());
            
            if (commandData.getReturnStr() != "")
            {
                auto& color = commandData.getReturnColor();
                _chatBox->addLine(commandData.getReturnStr(), tgui::Color(color.r, color.g, color.b, color.a));
            }

            _textBox->setText("");
        });
    }

    // * events
    _parent->onSizeChange(&CommandPrompt::ResizePrompt);
    _parent->onClosing(&CommandPrompt::closePrompt);
    _parent->setMaximumSize(_parent->getParentGui()->getView().getSize());
    _parent->onMaximize(&CommandPrompt::MaximizePrompt);
}

void CommandPrompt::close()
{
    _parent = nullptr;
    _textBox = nullptr;
    _autoFillList = nullptr;
    _chatBox = nullptr;
}

void CommandPrompt::UpdateEvent(const sf::Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Key::Tilde)
        {
            if (_parent->isEnabled())
            {
                bool temp;
                closePrompt(&temp);
            }
            else
                openPrompt();
        }

        if (event.key.code == sf::Keyboard::Key::Escape)
        {
            if (_autoFillList->isVisible() && (_textBox->isFocused() || _autoFillList->isFocused()))
            {
                _autoFillList->setVisible(false);
            }
            else
            {
                _textBox->setFocused(false);
            }
        }

        if (event.key.code == sf::Keyboard::Key::Up && (_textBox->isFocused() || _autoFillList->isFocused()))
        {
            if (!_autoFillList->isVisible() && _commandHistory.size() != 0)
            {
                _autoFillList->removeAllItems();
                for (auto i = _commandHistory.begin(); i != _commandHistory.end(); i++)
                    _autoFillList->addItem(*i);
                _autoFillList->setVisible(true);
                _autoFillList->setSize({"100%", std::min(float(_autoFillList->getItemHeight() * _autoFillList->getItemCount() + 5), _parent->getSize().y / 3)});
            }
            int temp = _autoFillList->getSelectedItemIndex() - 1;
            temp = temp < 0 ? _autoFillList->getItemCount()-1 : temp; // checking if we want to wrap to the last item in the auto fill list

            _autoFillList->setSelectedItemByIndex(temp);
            _textBox->setCaretPosition(_textBox->getText().size());
        }

        if (event.key.code == sf::Keyboard::Key::Down)
        {
            if (_autoFillList->isVisible())
            {
                _autoFillList->setSelectedItemByIndex((_autoFillList->getSelectedItemIndex() + 1)%_autoFillList->getItemCount());
                _textBox->setCaretPosition(_textBox->getText().size());
            }
        }

        if (event.key.code == sf::Keyboard::Key::Tab)
        {
            AutoFill();
        }
    }
}

void CommandPrompt::openPrompt()
{
    _parent->setEnabled(true);
    _parent->setVisible(true);
    _parent->moveToFront();
}

void CommandPrompt::closePrompt(bool* abortTguiClose)
{
    _parent->setEnabled(false);
    _parent->setVisible(false);

    if (abortTguiClose != nullptr)
        (*abortTguiClose) = true;
}

void CommandPrompt::print(const std::string& str)
{
    if (_chatBox && _allowPrint)
        _chatBox->addLine(str);
}

bool CommandPrompt::isPrintAllowed()
{
    return _allowPrint;
}

void CommandPrompt::MaximizePrompt()
{
    if (_parent->getFullSize() == _parentSize.getValue())
    {
        _parentSize = _parent->getSizeLayout();
        _parentPos = _parent->getPositionLayout();
        _parent->setPosition({0,0});
        _parent->setPositionLocked(true);
        _parent->onSizeChange.setEnabled(false);
        _parent->setSize({"100%", "100%"});
        _parent->onSizeChange.setEnabled(true);
    }
    else
    {
        _parent->setSize(_parentSize);
        _parent->setPosition(_parentPos);
        _parent->setPositionLocked(false);
        ResizePrompt();
    }
}

void CommandPrompt::ResizePrompt()
{
    if (_parent->getSizeLayout().x.toString() != "100%")
        _parentSize = _parent->getFullSize();
}

void CommandPrompt::UpdateAutoFill()
{
    std::list<std::string> commands = Command::Handler::autoFillSearch(_textBox->getText().toStdString());;
    _autoFillList->removeAllItems();

    if (commands.size() == 0)
    {
        _autoFillList->setVisible(false);
        return;
    }
    else 
        _autoFillList->setVisible(true);

    for (auto cmd: commands)
    {
        _autoFillList->addItem(cmd);
    }

    _autoFillList->setSize({"100%", std::min(float(_autoFillList->getItemHeight() * _autoFillList->getItemCount() + 5), _parent->getSize().y / 3)});
    _autoFillList->setSelectedItemByIndex(0);
}

void CommandPrompt::AutoFill()
{
    _textBox->setFocused(true);
    tgui::String temp = _textBox->getText();
    size_t subCommandPos = temp.find_last_of('('); // if there is a sub command
    size_t lastSpace = temp.find_last_of(' ');
    if (subCommandPos != std::string::npos && subCommandPos > lastSpace)
    {
        _textBox->setText(temp.substr(0, subCommandPos + 1) + _autoFillList->getSelectedItem());
    }
    else
    {
        _textBox->setText(temp.substr(0, lastSpace + 1) + _autoFillList->getSelectedItem());
    }
    UpdateAutoFill();
}

void CommandPrompt::addHistory(const std::string& command)
{
    if (StringHelper::trim_copy(command).size() == 0) return;
    if (_maxHistory > _commandHistory.size())
    {
        if (command != "" && _commandHistory.back() != command)
            _commandHistory.push_back(command);
    }
    else
    {
        _commandHistory.erase(_commandHistory.begin());
        _commandHistory.push_back(command);
    }
}

void CommandPrompt::setMaxHistory(const size_t& size)
{
    _maxHistory = size;
}

size_t CommandPrompt::getMaxHistory()
{
    return _maxHistory;
}

void CommandPrompt::clearHistory()
{
    _commandHistory.clear();
}
