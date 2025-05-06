#include "Utils/CommandPrompt.hpp"

tguiCommon::ChildWindow Command::Prompt::m_windowHandler;

tgui::ChildWindow::Ptr Command::Prompt::m_parent{nullptr};
tgui::EditBox::Ptr Command::Prompt::m_textBox{nullptr};
tgui::ListBox::Ptr Command::Prompt::m_autoFillList{nullptr};
tgui::ChatBox::Ptr Command::Prompt::m_chatBox{nullptr};

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
            Command::Handler::get().addCommand("cp", Command::Definition{"Prefix for any Command Prompt specific commands", {Command::helpPrint, "Not a valid cp command. Try using 'help cp' for more info"}, {}});
            Command::Handler::get().findCommand("cp")
            ->addCommand("clear", "clears the command prompt", {[temp](){ temp->removeAllLines(); }})
            // Note that the following command only affects the command prompts max lines not the command handlers max lines
            .addCommand("setMaxLines", "sets the max number of lines the command prompt will hold (min value of 50)", 
                [temp](Command::Data* data)
                { 
                    unsigned long t;
                    if (!Command::isValidInput<unsigned long>(data->getToken(0), t, 0) || t < 50)
                    {
                        data->addError(ERROR_COLOR + "Please enter a valid number (min = 50)");
                    }
                    else
                    {
                        temp->setLineLimit(t);
                        data->setReturnStr("Max lines was successfully set to: " + data->getToken(0)); // TODO make this green text
                    }
                }
            )
            .addCommand("getMaxLines", "retuns the current max lines", 
                [temp](Command::Data* data){
                    data->setReturnStr(std::to_string(temp->getLineLimit()));
                }
            )
            .addCommand("allowPrinting", "controls if printing to the command prompt is allowed (true/false or 1/0)", 
                [](Command::Data* data)
                {
                    m_allowPrint = StringHelper::toBool(data->getToken(0), m_allowPrint);
                    data->setReturnStr("Printing is now: ");
                    data->addToReturnStr(m_allowPrint ? "allowed" : "not allowed");
                }, {"True", "False"}
            )
            .addCommand("getPrintingAllowed", "prints if printing is allowed", 
                [](Command::Data* data)
                {
                    data->setReturnStr(m_allowPrint ? "True" : "False");
                }
            )
            .addCommand("getMaxHistory", "prints the max number of commands in history", 
                [](Command::Data* data){ 
                    Command::print(std::to_string(Command::Handler::get().getMaxCommandHistory()), data); 
                })
            .addCommand("setMaxHistory", "sets the max number of commands in history", 
                [](Command::Data* data){
                    unsigned long temp = 0;
                    if (!Command::isValidInput<unsigned long>(data->getToken(), temp, 64))
                    {
                        data->addError(ERROR_COLOR + "Invalid max entered");
                        return;
                    }
                    Command::Handler::get().setMaxCommandHistory(temp);
                    data->setReturnStr("Max history set successfully"); // TODO make this green text
                }
            )
            .addCommand("getRandom", "[min = 0] [max = " + std::to_string(RAND_MAX) + " ] [amount = 1]" +
                        " | prints n random numbers with a total max of " + std::to_string(RAND_MAX) + " and total min of 0",
                [](Command::Data* data)
                {
                    int min = 0;
                    int max = RAND_MAX;

                    if (data->getNumTokens() > 0 && (!Command::isValidInput<int>(data->getToken(0), min, 0) || min < 0))
                    {
                        data->addError(ERROR_COLOR + "Invalid min entered");
                        return;
                    }
                    if (data->getNumTokens() > 1 && (!Command::isValidInput<int>(data->getToken(1), max, RAND_MAX) || max < min || max > RAND_MAX))
                    {
                        data->addError(ERROR_COLOR + "Invalid max entered");
                        return;
                    }

                    unsigned long amount = 1;
                    if (data->getNumTokens() == 3 && (!Command::isValidInput<unsigned long>(data->getToken(2), amount, 1) || amount < 1))
                    {
                        data->addError(ERROR_COLOR + "Invalid amount entered - Defaulted to 1");
                    }

                    while (amount != 0)
                    {
                        data->addToReturnStr(std::to_string((min + (rand())%(max+1 - min))) + " ");
                        amount--;
                    }
                });
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

            m_chatBox->addLine("> " + m_textBox->getText());

            auto commandData = Command::Handler::get().invokeCommand(m_textBox->getText().toStdString());
            
            if (commandData.getReturnStr() != "")
            {
                m_chatBox->addLine(commandData.getReturnStr());
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
        m_parent->getParent()->remove(m_parent);
    m_parent = nullptr;
    m_textBox = nullptr;
    m_autoFillList = nullptr;
    m_chatBox = nullptr;
}

bool Command::Prompt::UpdateEvent(const sf::Event& event)
{
    if (!m_parent)
        return false;
    if (m_ignoreInputText)
    {
        if (const sf::Event::TextEntered* textEntered = event.getIf<sf::Event::TextEntered>())
            if (textEntered->unicode == 96 || textEntered->unicode == 126) // TODO make this more dynamic
            {
                m_ignoreInputText = false;
                return true;
            }
    }
    if (const sf::Event::KeyPressed* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Grave) // TODO make this more dynamic
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

        if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            if (m_autoFillList->isVisible() && (m_textBox->isFocused() || m_autoFillList->isFocused()))
            {
                m_autoFillList->setVisible(false);
                return true;
            }
            else if (m_textBox->isFocused())
            {
                m_textBox->setFocused(false);
                return true;
            }
        }

        if (keyPressed->code == sf::Keyboard::Key::Up && (m_textBox->isFocused() || m_autoFillList->isFocused()))
        {
            if (!m_autoFillList->isVisible() && Command::Handler::get().getCommandHistory().size() != 0)
            {
                m_autoFillList->removeAllItems();
                for (auto i = --Command::Handler::get().getCommandHistory().end(); i != --Command::Handler::get().getCommandHistory().begin(); i--)
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

        if (keyPressed->code == sf::Keyboard::Key::Down)
        {
            if (m_autoFillList->isVisible())
            {
                m_autoFillList->setSelectedItemByIndex((m_autoFillList->getSelectedItemIndex() + 1)%m_autoFillList->getItemCount());
                m_textBox->setCaretPosition(m_textBox->getText().size());
                return true;
            }
        }

        if (keyPressed->code == sf::Keyboard::Key::Tab)
        {
            AutoFill();
            return true;
        }
    }
    return false;
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

void Command::Prompt::print(const tgui::String& str, bool forcePrint)
{
    if (m_chatBox && (m_allowPrint || forcePrint))
        m_chatBox->addLine(str);
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
    std::list<std::string> commands = Command::Handler::get().autoFillSearch(m_textBox->getText().toStdString());
    commands.sort();
    m_autoFillList->removeAllItems();
    
    if (commands.size() == 0)
    {
        m_autoFillList->setVisible(false);
        return;
    }
    else 
    {
        m_autoFillList->setVisible(true);
        for (auto str: commands)
        {
            m_autoFillList->addItem(str);
        }
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
