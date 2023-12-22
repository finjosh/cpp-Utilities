#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "include/Examples/funcHelper.h"
#include "include/Examples/EventHelper.h"
#include "include/Examples/Stopwatch.h"
#include "include/Examples/StringHelper.h"
#include "include/Examples/UpdateLimiter.h"
#include "include/Examples/Log.h"
#include "include/Examples/iniParser.h"
#include "include/Examples/TerminatingFunction.h"
#include "include/Examples/LiveVar.h"
#include "include/Examples/VarDisplay.h"
#include "include/Examples/CommandHandler.h"
#include "include/Examples/CommandPrompt.h"

using namespace std;
using namespace sf;

int main()
{
    //! If you want Terminating functions to be attached to the CommandHandler you need to call the initializer
    TerminatingFunction::initCommands();
    //! --------------------------

    funcHelperTest::test();
    EventHelperTest::test();
    StopwatchTest::test();
    StringHelperTest::test();
    UpdateLimiterTest::test();
    LogTest::test();
    iniParserTest::test();
    //* uncomment the next line for the Terminating function tests (may disrupt output order)
    // TerminatingFunctionTest::test(); 
    // live var test is put after the VarDisplay is initalized so the vars will be in the display
    VarDisplayTest::test();
    CommandHandlerTest::test();

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "C++ Utilities");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    // -----------------------

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    CommandPrompt::init(gui);
    //! ---------------------------------------------------
    //* CommandPromptTest
    CommandPromptTest::test(); // Needs to be called after CommandPrompt is initalized

    //* LiveVarTest
    LiveVarTest::test(); //! NOTE - live vars will not show in the VarDisplay unless added after VarDisplay is initalized

    float deltaTime = 0;
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        window.clear();
        // updating the delta time var
        deltaTime = deltaClock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            CommandPrompt::UpdateEvent(event);
            //! ----------------------------------------------------------

            if (event.type == sf::Event::Closed)
                window.close();
        }
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime);
        //! ------------------------------

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    CommandPrompt::close();
    //! --------------------------------------------------------------

    return 0;
}
