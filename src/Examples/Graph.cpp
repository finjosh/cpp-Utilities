#include "Examples/Graph.hpp"

tguiCommon::ChildWindow GraphTest::_windowData;
Graph GraphTest::_graph;

void GraphTest::test(tgui::Gui& gui)
{
    VarDisplay::setVisible();
    sf::Font font;
    font.loadFromFile("JetBrainsMono-Regular.ttf");
    GraphData data({1,2,3,4,5,6,7}, {3,2,7,14,3,9,0}, sf::Color::Magenta, "Random Data", GraphType::Line);
    _graph.setXLable("X data label");
    _graph.setYLable("Y data label");
    _graph.setFont(font);
    _graph.addDataSet(data);
    data.setColor(sf::Color::Green);
    data.setGraphType(GraphType::Bar);
    _graph.addDataSet(data);
    _graph.Update();

    auto parent = tgui::ChildWindow::create("Graph Tests", tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize);
    parent->setResizable();
    parent->setSize({"30%","100%"});
    auto panel = tgui::ScrollablePanel::create();
    parent->add(panel);
    auto list = tgui::HorizontalWrap::create();
    panel->add(list);

    auto reloadPicture = [list](){ 
        _graph.Update(); 
        tgui::Texture texture;
        texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
        list->getWidgets()[0]->cast<tgui::Picture>()->getRenderer()->setTexture(texture); 
    };
    LiveVar::initVar("(0) Add Random Value",0,1,sf::Keyboard::Key::Up,sf::Keyboard::Key::Down);
    LiveVar::initVar("(1) Axis Thickness", _graph.getAxesThickness(), 1, sf::Keyboard::Key::Num1,sf::Keyboard::Key::Q, 0);
    LiveVar::initVar("(2) Background Line Thickness", _graph.getBackgroundLinesThickness(), 1, sf::Keyboard::Key::Num2,sf::Keyboard::Key::W, 0);
    LiveVar::initVar("(3) Decimal Precision", _graph.getDecimalPrecision(), 1, sf::Keyboard::Key::Num3,sf::Keyboard::Key::E, 0, 6);
    LiveVar::initVar("(4) Margin", _graph.getMargin(), 1, sf::Keyboard::Key::Num4,sf::Keyboard::Key::R, 0);
    LiveVar::initVar("(5) Resolution (x and y)", _graph.getResolution().x, 64, sf::Keyboard::Key::Num5,sf::Keyboard::Key::T, 0, 4000);
    LiveVar::initVar("(6) X Axis Text Rotation", _graph.getXTextRotation(), 1, sf::Keyboard::Key::Num6,sf::Keyboard::Key::Y);
    LiveVar::initVar("(7) Y Axis Text Rotation", _graph.getYTextRotation(), 1, sf::Keyboard::Key::Num7,sf::Keyboard::Key::U);
    LiveVar::initVar("(8) CharSize", _graph.getCharSize(), 1, sf::Keyboard::Key::Num8, sf::Keyboard::Key::I, 1);
    LiveVar::getVarEvent("(0) Add Random Value")->connect([reloadPicture](){ 
        auto temp = _graph.getDataSet(12);
        temp->push_back({temp->getDataValue(temp->getDataLength()-1).x + 0.1f, temp->getDataValue(temp->getDataLength()-1).y+(rand()%201-100)/100.f});
        temp = _graph.getDataSet(13);
        temp->push_back({temp->getDataValue(temp->getDataLength()-1).x + 0.1f, temp->getDataValue(temp->getDataLength()-1).y+(rand()%201-100)/100.f});
        reloadPicture();
    });
    LiveVar::getVarEvent("(1) Axis Thickness")->connect([reloadPicture](const float& value){ 
        _graph.setAxesThickness(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(2) Background Line Thickness")->connect([reloadPicture](const float& value){ 
        _graph.setBackgroundLinesThickness(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(3) Decimal Precision")->connect([reloadPicture](const float& value){ 
        _graph.setDecimalPrecision(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(4) Margin")->connect([reloadPicture](const float& value){ 
        _graph.setMargin(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(5) Resolution (x and y)")->connect([reloadPicture](const float& value){ 
        _graph.setResolution({(unsigned int)value,(unsigned int)value});
        reloadPicture();
    });
    LiveVar::getVarEvent("(6) X Axis Text Rotation")->connect([reloadPicture](const float& value){ 
        _graph.setXTextRotation(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(7) Y Axis Text Rotation")->connect([reloadPicture](const float& value){ 
        _graph.setYTextRotation(value);
        reloadPicture();
    });
    LiveVar::getVarEvent("(8) CharSize")->connect([reloadPicture](const float& value){ 
        _graph.setCharSize(value);
        reloadPicture();
    });

    tgui::Texture texture;
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});
    
    _graph.clearDataSets();
    data.clearPairValues();
    data.setValues({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}, {7,4,3,2,5,5,6,4,5,6,3,4,5,6,8,4,5,6,2,8,9});
    data.setGraphType(GraphType::Histogram);
    data.setColor(sf::Color::Yellow);
    _graph.addDataSet(data);
    data.setGraphType(GraphType::Scatter);
    data.setColor(sf::Color::Cyan);
    _graph.addDataSet(data);
    data.setGraphType(GraphType::Line);
    data.setColor(sf::Color::Magenta);
    _graph.addDataSet(data);
    _graph.Update();
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});
    
    _graph.clearDataSets();
    data.clearPairValues();
    data.setValues({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}, {7,4,3,2,5,5,6,4,5,6,3,4,5,6,8,4,5,6,2,8,9});
    data.setGraphType(GraphType::Bar);
    data.setColor(sf::Color::Green);
    _graph.addDataSet(data);
    _graph.Update();
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});

    _graph.clearDataSets();
    data.clearPairValues();
    {
    std::vector<sf::Vector2f> temp = {{0,0}};
    for (int i = 0; i < 50; i++)
    {
        temp.push_back({temp.back().x + (float)rand()/RAND_MAX*5, (float)rand()/RAND_MAX*50});
    }
    data.setValues(temp);
    }
    data.setGraphType(GraphType::Line);
    data.setColor(sf::Color::Magenta);
    _graph.addDataSet(data);
    data.setGraphType(GraphType::Bar);
    data.setColor(sf::Color::Green);
    _graph.addDataSet(data);
    _graph.Update();
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});

    _graph.clearDataSets();
    data.clearPairValues();
    {
    std::vector<sf::Vector2f> temp = {{0,0}};
    for (int i = 0; i < 50; i++)
    {
        temp.push_back({temp.back().x + (float)rand()/RAND_MAX*5, (float)rand()/RAND_MAX*100-50});
    }
    data.setValues(temp);
    }
    data.setGraphType(GraphType::Scatter);
    data.setColor(sf::Color::Cyan);
    _graph.addDataSet(data);
    _graph.Update();
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});

    _graph.clearDataSets();
    data.clearPairValues();
    {
    std::vector<sf::Vector2f> temp;
    for (int i = 0; i < 50; i++)
    {
        temp.push_back({(float)rand()/RAND_MAX*100, (float)rand()/RAND_MAX*100});
    }
    std::sort(temp.begin(), temp.end(), [](const sf::Vector2f& first, const sf::Vector2f& second){ return first.x < second.x; });
    data.setValues(temp);
    }
    data.setGraphType(GraphType::Histogram);
    data.setColor(sf::Color::Green);
    _graph.addDataSet(data);
    data.setGraphType(GraphType::Line);
    data.setColor(sf::Color::Magenta);
    _graph.addDataSet(data);
    _graph.Update();
    texture.loadFromPixelData(_graph.getTexture().getSize(), _graph.getTexture().copyToImage().getPixelsPtr());
    list->add(tgui::Picture::create(texture));
    list->getWidgets().back()->setSize({500,500});

    _graph.clearDataSets();
    data.clearPairValues();
    data.setValues({1,2,3,4,5,6,7}, {3,2,7,14,3,9,0});
    data.setGraphType(GraphType::Line);
    _graph.setXLable("X data label");
    _graph.setYLable("Y data label");
    _graph.setFont(font);
    _graph.addDataSet(data);
    data.setColor(sf::Color::Magenta);
    data.setGraphType(GraphType::Bar);
    data.setColor(sf::Color::Green);
    _graph.addDataSet(data);
    _graph.Update();

    list->getRenderer()->setSpaceBetweenWidgets(10);
    list->setSize({"100%", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});
    // list->onSizeChange([list](){list->setSize({"100% - 15", list->getWidgets().back()->getPosition().y + list->getWidgets().back()->getFullSize().y + 15});});

    tguiCommon::ChildWindow::setSoftClose(parent);
    _windowData.setMaximize(parent);

    tguiCommon::ChildWindow::createOpenCloseCommand("GraphExamples", parent);

    gui.add(parent);
}
