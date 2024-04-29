#ifndef GRAPHTEST_H
#define GRAPHTEST_H

#pragma once

#include "Utils/Graph.hpp"
#include "TGUI/AllWidgets.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "Utils/Debug/CommandHandler.hpp"

class GraphTest
{
public:
    static void test(tgui::Gui& gui);

protected:

private:
    // static tgui::ChildWindow::Ptr _parent;
    // static tgui::PanelListBox::Ptr _list;

};

#endif
