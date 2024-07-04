#ifndef GRAPH_TEST_H
#define GRAPH_TEST_H

#pragma once

#include "Utils/Graph.hpp"
#include "TGUI/AllWidgets.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"
#include "Utils/CommandHandler.hpp"
#include "Utils/TGUICommon.hpp"
#include "Utils/Debug/LiveVar.hpp"
#include "Utils/Debug/VarDisplay.hpp"

class GraphTest
{
public:
    static void test(tgui::Gui& gui);

protected:

private:
    static tguiCommon::ChildWindow _windowData;
    static Graph _graph;
};

#endif
