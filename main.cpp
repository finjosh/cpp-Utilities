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

using namespace std;
using namespace sf;

int main()
{
    //! If you want Terminating functions to be attached to the CommandHandler you need to call the initializer
    TerminatingFunction::initCommands();
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
    // -----------------------

    //! Adding a command for showing off the different themes
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
                tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the them to the dark theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Dark.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/TransparentGrey.txt"); 
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
    //* CommandPromptTest
    // CommandPromptTest::test(); // Needs to be called after CommandPrompt is initalized
    //* TFuncDisplayTest
    // TFuncDisplayTest::test();

    //* LiveVarTest
    // LiveVarTest::test(); //! NOTE - live vars will not show in the VarDisplay unless added after VarDisplay is initalized

    EventHelper::EventDynamic<int> test;
    test.connect([](const int& v){ cout << std::to_string(v) << endl; });
    test.invoke(7);
    test.invoke(127);

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

    return 0;
}
