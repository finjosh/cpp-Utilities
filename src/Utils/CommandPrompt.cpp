#include "Utils/CommandPrompt.hpp"

tguiCommon::ChildWindow Command::Prompt::m_windowHandler;

tgui::ChildWindow::Ptr Command::Prompt::m_parent{nullptr};
tgui::EditBox::Ptr Command::Prompt::m_textBox{nullptr};
tgui::ListBox::Ptr Command::Prompt::m_autoFillList{nullptr};
tgui::ChatBox::Ptr Command::Prompt::m_chatBox{nullptr};

std::list<std::string> Command::Prompt::m_commandHistory;
size_t Command::Prompt::m_maxHistory = 64;

bool Command::Prompt::m_allowPrint = true;
bool Command::Prompt::m_ignoreInputText = false;

void Command::Prompt::init(tgui::Gui& sfmlGui)
{
    init(sfmlGui.getContainer());
}

void Command::Prompt::init(tgui::Container::Ptr parent)
{
    TGUI_ASSERT(parent, "Need to give a valid parent for Command::Prompt to init");

    m_parent = tgui::ChildWindow::create("Command Prompt", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    m_parent->onClose(Command::Prompt::close);
    parent->add(m_parent);

    // * setup of child window
    m_parent->setTitleAlignment(tgui::HorizontalAlignment::Left);
    m_parent->setResizable();
    m_parent->setSize({"40%","35%"});
    m_parent->setPosition({"60%","65%"});

    // * setup for edit box
    {
        m_textBox = tgui::EditBox::create();
        m_parent->add(m_textBox);
        m_textBox->setSize({"100%", "25"});
        m_textBox->setPosition({0, "100% - 25"});

        // * setup for auto closing m_autoFillList when focus is lost
        m_textBox->onUnfocus([]()
        {
            if (m_autoFillList)
            {
                m_autoFillList->setVisible(false);
            }
        });

        // * setup for chat m_textBox
        m_chatBox = tgui::ChatBox::create();
        m_parent->add(m_chatBox);
        m_chatBox->setSize({"100%", "100%-25"});
        m_chatBox->setLinesStartFromTop();
        m_chatBox->setLineLimit(256);
        m_chatBox->setFocusable(false);

        // * setup for the command prompt custom commands
        {
            auto temp = m_chatBox.get();
            Command::Handler::addCommand(command{"cp", "Prefix for any Command Prompt specific commands", {Command::helpPrint, "Not a valid cp command. Try using 'help cp' for more info"}, {},
            // list of sub commands
            {
                Command::command("clear", "clears the command prompt", {[temp](){ temp->removeAllLines(); }}),
                Command::command("setMaxLines", "sets the max number of lines the command prompt will hold (min value of 50)", {[temp](Command::Data* input)
                { 
                    unsigned long t;
                    if (Command::isValidInput<unsigned long>("Please enter a valid number (min - 50)", *input, input->getToken(0), t, 0, 
                            [](const unsigned long& i){ return i >= 50; }))
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
                    m_allowPrint = StringHelper::toBool(input->getToken(0), m_allowPrint);
                    input->setReturnStr("Printing is now: ");
                    input->addToReturnStr(m_allowPrint ? "allowed" : "not allowed");
                }}, {"True", "False"}),
                Command::command("getPrintingAllowed", "prints if printing is allowed", {[](Command::Data* input)
                {
                    input->setReturnStr(m_allowPrint ? "True" : "False");
                }}),
                Command::command("clearHistory", "clears the command history", {[](Command::Data* data){ Command::Prompt::clearHistory(); data->setReturnStr("History Cleared"); data->setReturnColor({0,255,0}); }}),
                Command::command("getMaxHistory", "prints the max number of commands in history", {[](Command::Data* data){ Command::print(std::to_string(Command::Prompt::getMaxHistory()), data); }}),
                Command::command("setMaxHistory", "sets the max number of commands in history", {[](Command::Data* data){
                    unsigned long temp = 0;
                    if (!Command::isValidInput<unsigned long>("Invalid max entered", *data, data->getToken(), temp, 64))
                        return;
                    Command::Prompt::setMaxHistory(temp);
                    data->setReturnStr("Max history set successfully");
                    data->setReturnColor({0,255,0});
                }}),
                { "getRandom", "[min = 0] [max = " + std::to_string(RAND_MAX) + " ] [amount = 1]" +
                            " | prints n random numbers in the following syntax: \"a(1) a(2) a(3) a(4) ... a(n)\" (hard max of " + std::to_string(RAND_MAX) + " and hard min of 0)",
                    {[](Command::Data* input)
                    {
                        int min = 0;
                        int max = RAND_MAX;

                        if (input->getNumTokens() > 0 && !Command::isValidInput<int>("Invalid min entered", *input, input->getToken(0), min, 0, [](int& v){ return v >= 0;}) 
                            || 
                            input->getNumTokens() > 1 && !Command::isValidInput<int>("Invalid max entered", *input, input->getToken(1), max, RAND_MAX, [&max, &min](int& v){ return !(v < 0 || v > max || min > max);}))
                            return;

                        unsigned long amount = 1;
                        if (input->getNumTokens() == 3)
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
        m_autoFillList = tgui::ListBox::create();
        m_parent->add(m_autoFillList);
        m_autoFillList->setSize({"100%", "0"});
        m_autoFillList->setAutoScroll(false);
        m_autoFillList->setOrigin(0,1);
        m_autoFillList->setPosition({"0"}, {"100%-" + std::to_string(m_textBox->getSize().y)});
        m_autoFillList->setFocusable(false);

        m_textBox->onTextChange(Command::Prompt::UpdateAutoFill);
        m_textBox->onFocus([]()
        {
            Command::Prompt::UpdateAutoFill(); 
            m_textBox->onFocus.setEnabled(false);
        });
        m_textBox->onUnfocus([]()
        {
            if (m_textBox)
                m_textBox->onFocus.setEnabled(true);
        });

        m_textBox->onReturnKeyPress([]()
        {
            if (m_autoFillList->isVisible())
                AutoFill(false);

            Command::color tColor;
            m_chatBox->addLine("> " + m_textBox->getText(), tgui::Color(tColor.r, tColor.g, tColor.b, tColor.a), tgui::TextStyle::Bold);

            auto commandData = Command::Handler::callCommand(m_textBox->getText().toStdString());
            Command::Prompt::addHistory(m_textBox->getText().toStdString());
            
            if (commandData.getReturnStr() != "")
            {
                tColor = commandData.getReturnColor();
                m_chatBox->addLine(commandData.getReturnStr(), {tColor.r, tColor.g, tColor.b, tColor.a});
            }

            m_textBox->setText("");
        });
    }

    //* setting up the maximize functionality
    m_windowHandler.setMaximize(m_parent);
    m_windowHandler.setSoftClose(m_parent);
    setVisible(false);
}

void Command::Prompt::close()
{
    if (m_parent)
        m_parent->destroy();
    m_parent = nullptr;
    m_textBox = nullptr;
    m_autoFillList = nullptr;
    m_chatBox = nullptr;
}

bool Command::Prompt::UpdateEvent(const sf::Event& event)
{
    if (!m_parent)
        return false;
    if (m_ignoreInputText && event.type == sf::Event::TextEntered && (event.text.unicode == 96 || event.text.unicode == 126)) // TODO make this more dynamic
    {
        m_ignoreInputText = false;
        return true;
    }
    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Key::Tilde) // TODO make this more dynamic
        {
            if (m_parent->isEnabled())
                setVisible(false);
            else
            {
                setVisible(true);
                m_textBox->setFocused(true);
            }
            m_ignoreInputText = true;
            return true;
        }

        // checking after checking the open and close key
        if (!m_parent->isFocused())
            return false;

        if (event.key.code == sf::Keyboard::Key::Escape)
        {
            if (m_autoFillList->isVisible() && (m_textBox->isFocused() || m_autoFillList->isFocused()))
            {
                m_autoFillList->setVisible(false);
                return true;
            }
            else
            {
                m_textBox->setFocused(false);
                return true;
            }
        }

        if (event.key.code == sf::Keyboard::Key::Up && (m_textBox->isFocused() || m_autoFillList->isFocused()))
        {
            if (!m_autoFillList->isVisible() && m_commandHistory.size() != 0)
            {
                m_autoFillList->removeAllItems();
                for (auto i = m_commandHistory.begin(); i != m_commandHistory.end(); i++)
                    m_autoFillList->addItem(*i);
                m_autoFillList->setVisible(true);
                m_autoFillList->setSize({"100%", std::min(float(m_autoFillList->getItemHeight() * m_autoFillList->getItemCount() + 5), m_parent->getSize().y / 3)});
            }
            int temp = m_autoFillList->getSelectedItemIndex() - 1;
            temp = temp < 0 ? m_autoFillList->getItemCount()-1 : temp; // checking if we want to wrap to the last item in the auto fill list

            m_autoFillList->setSelectedItemByIndex(temp);
            m_textBox->setCaretPosition(m_textBox->getText().size());

            return true;
        }

        if (event.key.code == sf::Keyboard::Key::Down)
        {
            if (m_autoFillList->isVisible())
            {
                m_autoFillList->setSelectedItemByIndex((m_autoFillList->getSelectedItemIndex() + 1)%m_autoFillList->getItemCount());
                m_textBox->setCaretPosition(m_textBox->getText().size());
                return true;
            }
        }

        if (event.key.code == sf::Keyboard::Key::Tab)
        {
            AutoFill();
            return true;
        }
    }
    return false;
}

void Command::Prompt::UpdateDefaultColor()
{
    tgui::Color temp = tgui::Theme::getDefault()->getGlobalProperty("TextColor").getColor();
    Command::color::setDefaultColor({temp.getRed(), temp.getGreen(), temp.getBlue(), temp.getAlpha()});
}

void Command::Prompt::setVisible(bool visible)
{
    if (!m_parent)
        return;
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

void Command::Prompt::setInputBoxFocused(bool focused)
{
    if (!m_parent)
        return;
    m_textBox->setFocused(focused);
}

void Command::Prompt::print(const tgui::String& str, Command::color color)
{
    if (m_chatBox && m_allowPrint)
        m_chatBox->addLine(str, tgui::Color(color.r, color.g, color.b, color.a));
}

bool Command::Prompt::isPrintAllowed()
{
    return m_allowPrint;
}

void Command::Prompt::allowPrint(bool print)
{
    m_allowPrint = print;
}

void Command::Prompt::UpdateAutoFill()
{
    std::list<std::string> commands = Command::Handler::autoFillSearch(m_textBox->getText().toStdString());;
    m_autoFillList->removeAllItems();

    if (commands.size() == 0)
    {
        m_autoFillList->setVisible(false);
        return;
    }
    else 
        m_autoFillList->setVisible(true);

    for (auto cmd: commands)
    {
        m_autoFillList->addItem(cmd);
    }

    m_autoFillList->setSize({"100%", std::min(float(m_autoFillList->getItemHeight() * m_autoFillList->getItemCount() + 5), m_parent->getSize().y / 3)});
    m_autoFillList->setSelectedItemByIndex(0);
}

void Command::Prompt::AutoFill(bool updateAutoFill)
{
    m_textBox->onFocus.setEnabled(false);
    m_textBox->setFocused(true);
    m_textBox->onFocus.setEnabled(true);
    Command::Data temp(m_textBox->getText().toStdString());
    
    if (m_textBox->getText().back() == ' ')
        temp.addToken("");

    size_t lastToken = temp.getNumTokens()-1;
    if (temp.getToken(lastToken).starts_with('('))
    {
        temp.setToken(lastToken, "(" + m_autoFillList->getSelectedItem().toStdString());
    }
    else
    {
        if (temp.getNumTokens() == 0)
            temp.addToken(m_autoFillList->getSelectedItem().toStdString());
        else
            temp.setToken(lastToken, m_autoFillList->getSelectedItem().toStdString());
    }
    m_textBox->setText(temp.getTokensStr());
    if (updateAutoFill)
        UpdateAutoFill();
}

void Command::Prompt::addHistory(const std::string& command)
{
    if (StringHelper::trim_copy(command).size() == 0) return;
    if (m_maxHistory > m_commandHistory.size())
    {
        if (command != "" && m_commandHistory.back() != command)
            m_commandHistory.push_back(command);
    }
    else
    {
        m_commandHistory.erase(m_commandHistory.begin());
        m_commandHistory.push_back(command);
    }
}

void Command::Prompt::setMaxHistory(size_t size)
{
    m_maxHistory = size;
}

size_t Command::Prompt::getMaxHistory()
{
    return m_maxHistory;
}

void Command::Prompt::clearHistory()
{
    m_commandHistory.clear();
}
