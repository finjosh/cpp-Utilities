#include "Utils/Debug/VarDisplay.hpp"

std::map<std::string, float> VarDisplay::m_vars;
bool VarDisplay::m_varChanged = false;

tguiCommon::ChildWindow VarDisplay::m_windowHandler;
tgui::ChildWindow::Ptr VarDisplay::m_parent = nullptr;
tgui::RichTextLabel::Ptr VarDisplay::m_varLabel = nullptr;
tgui::ScrollablePanel::Ptr VarDisplay::m_scrollPanel = nullptr;

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

    m_parent = tgui::ChildWindow::create("Debugging Vars", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
    m_windowHandler.setMinimize_Maximize(m_parent);
    m_windowHandler.setSoftClose(m_parent);
    gui.add(m_parent);

    m_parent->setSize({"10%","15%"});
    m_parent->setPosition({"90%", "0%"});
    m_parent->setResizable(true);
    m_scrollPanel = tgui::ScrollablePanel::create({m_parent->getSize() - tgui::Vector2f(0, m_parent->getSharedRenderer()->getTitleBarHeight())});
    m_parent->add(m_scrollPanel);
    m_scrollPanel->setSize({"100%", "100%"});

    m_varLabel = tgui::RichTextLabel::create();
    m_scrollPanel->add(m_varLabel);
    m_varLabel->onSizeChange([](){ VarDisplay::m_scrollPanel->setContentSize(VarDisplay::m_varLabel->getSize()); });

    setVisible(false);

    initCommands();
}

void VarDisplay::close()
{
    m_parent = nullptr;
    m_varLabel = nullptr;
    m_scrollPanel = nullptr;
}

void VarDisplay::Update()
{
    if (m_varChanged)
    {
        std::string str = "";
        for (auto i: m_vars)
        {
            str += ("<b><i>" + i.first + "</i></b> = " + std::to_string(i.second) + "\n"); 
        }
        if (str.length() > 0)
            str.erase(--str.end());
        
        m_varLabel->setText(str);

        m_varChanged = false;
    }
}

void VarDisplay::setVisible(bool visible)
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

void VarDisplay::addVar(const std::string& name, const float& value)
{
    m_varChanged = true;

    m_vars.insert({name, value});
}

void VarDisplay::setVar(const std::string& name, const float& value)
{
    auto iter = m_vars.find(name);

    if (iter == m_vars.end()) return;

    m_varChanged = true;
    iter->second = value;
}

void VarDisplay::removeVar(const std::string& name)
{
    if (m_vars.erase(name))
    {
        m_varChanged = true;
    }
}
