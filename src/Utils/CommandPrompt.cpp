#include "Utils/CommandPrompt.hpp"

tguiCommon::ChildWindow Command::Prompt::m_windowHandler;

tgui::ChildWindow::Ptr Command::Prompt::m_parent{nullptr};
tgui::EditBox::Ptr Command::Prompt::m_textBox{nullptr};
tgui::ListBox::Ptr Command::Prompt::m_autoFillList{nullptr};
tgui::GrowVerticalLayout::Ptr Command::Prompt::m_lineContainer{nullptr};

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
        m_textBox->setSize({"100%", "5%"});
        m_textBox->setPosition({0, "95%"});

        // * setup for auto closing m_autoFillList when focus is lost
        m_textBox->onUnfocus([]()
        {
            if (m_autoFillList)
            {
                m_autoFillList->setVisible(false);
            }
        });

        // * setup for chat m_textBox
        auto linePanel = tgui::ScrollablePanel::create();
        m_parent->add(linePanel);
        linePanel->getHorizontalScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
        linePanel->setSize({"100%", "95%"});
        m_lineContainer = tgui::GrowVerticalLayout::create();
        linePanel->add(m_lineContainer);
        m_lineContainer->setWidth("100%");
        m_lineContainer->setFocusable(false);

        // * setup for the command prompt custom commands
        {
            auto temp = m_lineContainer.get();
            Command::Handler::get().addCommand("cp", Command::Definition{"Prefix for any Command Prompt specific commands", {Command::helpPrint, "Not a valid cp command. Try using 'help cp' for more info"}, {}});
            Command::Handler::get().findCommand("cp")
            ->addCommand("clear", "clears the command prompt", {[temp](){ temp->removeAllWidgets(); }})
            .addCommand("getRandom", "[min = 0] [max = " + std::to_string(RAND_MAX) + " ] [amount = 1]" +
                        " | prints n random numbers with a total max of " + std::to_string(RAND_MAX) + " and total min of 0",
                [](Command::Data* data)
                {
                    int min = 0;
                    int max = RAND_MAX;

                    if (data->getNumTokens() > 0 && (!Command::isValidInput<int>(data->getToken(0), min, 0) || min < 0))
                    {
                        data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid min entered (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")");
                        return;
                    }
                    if (data->getNumTokens() > 1 && (!Command::isValidInput<int>(data->getToken(1), max, RAND_MAX) || max < min || max > RAND_MAX))
                    {
                        data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid max entered (min=" + std::to_string(min) + ", max=" + std::to_string(max) + ")");
                        return;
                    }

                    unsigned long amount = 1;
                    if (data->getNumTokens() == 3 && (!Command::isValidInput<unsigned long>(data->getToken(2), amount, 1) || amount < 1))
                    {
                        data->addError(Command::ERROR_COLOR + "getRandom Error" + Command::END_COLOR + " - Invalid amount entered (amount=" + std::to_string(amount) + ")");
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

            
            addLine("> " + m_textBox->getText());

            auto commandData = Command::Handler::get().invokeCommand(m_textBox->getText().toStdString());
            
            for (auto error: commandData.getErrors())
            {
                addLine(error);
            }

            for (auto warning: commandData.getWarnings())
            {
                addLine(warning);
            }

            if (commandData.getReturnStr() != "")
            {
                addLine(commandData.getReturnStr());
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
    assert(m_parent != nullptr && "Command::Prompt::close() - Command Prompt not initialized");

    if (m_parent)
        m_parent->getParent()->remove(m_parent);
    m_parent = nullptr;
    m_textBox = nullptr;
    m_autoFillList = nullptr;
    m_lineContainer = nullptr;
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
    assert(m_parent != nullptr && "Command::Prompt::setVisible() - Command Prompt not initialized");

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
    assert(m_parent != nullptr && "Command::Prompt::setInputBoxFocused() - Command Prompt not initialized");

    m_textBox->setFocused(focused);
}

void Command::Prompt::print(const std::string &str)
{
    assert(m_parent != nullptr && "Command::Prompt::print() - Command Prompt not initialized");

    addLine(str);
}

void Command::Prompt::UpdateAutoFill()
{
    assert(m_parent != nullptr && "Command::Prompt::UpdateAutoFill() - Command Prompt not initialized");

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
    assert(m_parent != nullptr && "Command::Prompt::AutoFill() - Command Prompt not initialized");

    m_textBox->onFocus.setEnabled(false);
    m_textBox->setFocused(true);
    m_textBox->onFocus.setEnabled(true);
    Command::Data temp(m_textBox->getText().toStdString());
    
    if (m_textBox->getText().back() == ' ')
        temp.addToken("");

    size_t lastToken = temp.getNumTokens()-1;
    if (temp.getToken(lastToken).starts_with("$("))
    {
        temp.setToken(lastToken, "$(" + m_autoFillList->getSelectedItem().toStdString());
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

void Command::Prompt::addLine(const tgui::String& line)
{
    assert(m_parent != nullptr && "Command::Prompt::addLine() - Command Prompt not initialized");

    if (m_lineContainer->getWidgets().size() > Command::Handler::get().getMaxLineHistory())
    {
        m_lineContainer->remove(0);
    }
    auto label = tgui::RichTextLabel::create(line);
    label->getScrollbar()->setPolicy(tgui::Scrollbar::Policy::Never);
    label->setAutoSize(true);
    m_lineContainer->add(label);
}
