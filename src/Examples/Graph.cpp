#include "Examples/Graph.hpp"

tguiCommon::ChildWindow GraphTest::_windowData;

void GraphTest::test(tgui::Gui& gui)
{
    auto parent = tgui::ChildWindow::create("Graph Tests", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    parent->setResizable();
    auto panel = tgui::ScrollablePanel::create();
    parent->add(panel);
    auto list = tgui::HorizontalWrap::create();
    panel->add(list);

    sf::Font font;
    font.loadFromFile("JetBrainsMono-Regular.ttf");
    GraphData data({1,2,3,4,5,6,7}, {3,2,7,14,3,9,0}, sf::Color::Magenta, "Random Data", GraphType::Line);
    Graph graph({0,0}, {1000,1000}, font, "X data label", "Y data label");
    graph.addDataSet(data);
    data.setColor(sf::Color::Green);
    data.setGraphType(GraphType::Bar);
    graph.addDataSet(data);
    graph.Update();

    tgui::Texture texture;
    texture.loadFromPixelData(graph.getTexture().getSize(), graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->add(tgui::Picture::create(texture));

    list->getRenderer()->setSpaceBetweenWidgets(10);
    list->setSize({"100% - 15", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});
    list->onSizeChange([list](){list->setSize({"100% - 15", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});});

    parent->onClosing(tguiCommon::ChildWindow::closeWindow, parent);
    parent->onMaximize(tguiCommon::ChildWindow::maximizeWindow, &_windowData);

    gui.add(parent);
}
