#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "include/Examples/funcHelper.hpp"
#include "include/Examples/EventHelper.hpp"
#include "include/Examples/Stopwatch.hpp"
#include "include/Examples/StringHelper.hpp"
#include "include/Examples/UpdateLimiter.hpp"
#include "include/Examples/Log.hpp"
#include "include/Examples/iniParser.hpp"
#include "include/Examples/TerminatingFunction.hpp"
#include "include/Examples/LiveVar.hpp"
#include "include/Examples/VarDisplay.hpp"
#include "include/Examples/CommandHandler.hpp"
#include "include/Examples/CommandPrompt.hpp"
#include "include/Examples/TFuncDisplay.hpp"
#include "include/Examples/Graph.hpp"

//! TEMP
#include "include/Utils/Graph.hpp"

using namespace std;
using namespace sf;

int main()
{
    //! If you want Terminating functions to be attached to the CommandHandler you need to call the initializer
    // TerminatingFunction::initCommands();
    //! --------------------------

    // funcHelperTest::test();
    // EventHelperTest::test();
    // StopwatchTest::test();
    // StringHelperTest::test();
    // UpdateLimiterTest::test();
    // LogTest::test();
    // iniParserTest::test();
    // //* uncomment the next line for the Terminating function tests (may disrupt output order)
    // // TerminatingFunctionTest::test(); 
    // // live var test is put after the VarDisplay is initalized so the vars will be in the display
    // VarDisplayTest::test();
    // CommandHandlerTest::test();

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "C++ Utilities");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    Command::color::setDefaultColor({255,255,255,255}); // white
    tgui::Theme::getDefault()->load("themes/Dark.txt");
    // -----------------------

    //! Adding a command for showing off the different themes
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            Command::command{"white", "(NOT WORKING) Sets the theme back to the white theme", {[](){ 
                tgui::Theme::setDefault(nullptr);
                tgui::Theme::getDefault()->replace(*tgui::Theme::getDefault().get());
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the theme to the dark theme", {[](){ 
                tgui::Theme::getDefault()->replace(tgui::Theme("themes/Dark.txt")); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the theme to the black theme", {[](){ 
                tgui::Theme::getDefault()->replace(tgui::Theme("themes/Black.txt")); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the theme to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->replace(tgui::Theme("themes/TransparentGrey.txt")); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}}
        }
    });
    //! -----------------------------------------------------

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);
    //! ---------------------------------------------------
    // //* CommandPromptTest
    // CommandPromptTest::test(); // Needs to be called after CommandPrompt is initalized
    // //* TFuncDisplayTest
    // TFuncDisplayTest::test();

    // //* LiveVarTest
    // LiveVarTest::test(); //! NOTE - live vars will not show in the VarDisplay unless added after VarDisplay is initalized

    TFunc::Add({[](TData* data){         
        data->setRunning();
    }});

    //! TEMP
    // TODO remove this
    VarDisplay::setVisible();
    sf::Font font;
    font.loadFromFile("JetBrainsMono-Regular.ttf");
    GraphData data({1,2,3,4,5,6,7}, {3,2,7,14,3,9,0}, sf::Color::Magenta, "Random Data", GraphType::Line);
    Graph graph({0,0}, {1000,1000}, font, "X data label", "Y data label");
    graph.addDataSet(data);
    data.setColor(sf::Color::Green);
    data.setGraphType(GraphType::Bar);
    graph.addDataSet(data);
    LiveVar::initVar("PlaceHolder",0,1,sf::Keyboard::Key::Up,sf::Keyboard::Key::Down);
    LiveVar::initVar("Axis Thickness", graph.getAxesThickness(), 1, sf::Keyboard::Key::Num1,sf::Keyboard::Key::Q);
    LiveVar::initVar("Background Line Thickness", graph.getBackgroundLinesThickness(), 1, sf::Keyboard::Key::Num2,sf::Keyboard::Key::W);
    LiveVar::initVar("Decimal Precision", graph.getDecimalPrecision(), 1, sf::Keyboard::Key::Num3,sf::Keyboard::Key::E);
    LiveVar::initVar("Margin", graph.getMargin(), 1, sf::Keyboard::Key::Num4,sf::Keyboard::Key::R);
    LiveVar::initVar("Resolution (x and y)", graph.getResolution().x, 64, sf::Keyboard::Key::Num5,sf::Keyboard::Key::T);
    LiveVar::initVar("X Axis Text Rotation", graph.getXTextRotation(), 1, sf::Keyboard::Key::Num6,sf::Keyboard::Key::Y);
    LiveVar::initVar("Y Axis Text Rotation", graph.getYTextRotation(), 1, sf::Keyboard::Key::Num7,sf::Keyboard::Key::U);
    LiveVar::getVarEvent("PlaceHolder")->connect([&graph](){ 
        auto temp = graph.getDataSet(1);
        temp->push_back({temp->getDataValue(temp->getDataLength()-1).x + 0.1, temp->getDataValue(temp->getDataLength()-1).y+(rand()%201-100)/100.f});
        temp = graph.getDataSet(2);
        temp->push_back({temp->getDataValue(temp->getDataLength()-1).x + 0.1, temp->getDataValue(temp->getDataLength()-1).y+(rand()%201-100)/100.f});
    });
    LiveVar::getVarEvent("Axis Thickness")->connect([&graph](const float& value){ 
        graph.setAxesThickness(value);
    });
    LiveVar::getVarEvent("Background Line Thickness")->connect([&graph](const float& value){ 
        graph.setBackgroundLinesThickness(value);
    });
    LiveVar::getVarEvent("Decimal Precision")->connect([&graph](const float& value){ 
        graph.setDecimalPrecision(value);
    });
    LiveVar::getVarEvent("Margin")->connect([&graph](const float& value){ 
        graph.setMargin(value);
    });
    LiveVar::getVarEvent("Resolution (x and y)")->connect([&graph](const float& value){ 
        graph.setResolution({value,value});
    });
    LiveVar::getVarEvent("X Axis Text Rotation")->connect([&graph](const float& value){ 
        graph.setXTextRotation(value);
    });
    LiveVar::getVarEvent("Y Axis Text Rotation")->connect([&graph](const float& value){ 
        graph.setYTextRotation(value);
    });
    //! --------------

    GraphTest::test(gui);

    graph.Update();
    auto temp = tgui::Picture::create({graph.getTexture()}, true);
    temp->setPosition({1000,0});
    temp->setSize({500,500});
    gui.add(temp);

    float deltaTime = 0;
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        //! should be called first thing every frame
        EventHelper::Event::ThreadSafe::update();
        //! ----------------------------------------
        window.clear();
        // updating the delta time var
        deltaTime = deltaClock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            Command::Prompt::UpdateEvent(event);
            //! ----------------------------------------------------------

            if (event.type == sf::Event::Closed)
                window.close();
        }
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime);
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        //! TEMP
        // TODO remove this
        graph.Update();
        temp->getRenderer()->setTexture({graph.getTexture()});
        // ----------------

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    Command::Prompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    return EXIT_SUCCESS;
}
