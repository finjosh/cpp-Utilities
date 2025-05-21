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
#include "include/Examples/TestHelper.hpp"

using namespace std;
using namespace sf;

int main()
{
    std::cout << GET_FUNCTION_INFO() << std::endl;
    // TestHelperTest::test();
    // funcHelperTest::test();
    // EventHelperTest::test();
    // StopwatchTest::test();
    StringHelperTest::test();
    // UpdateLimiterTest::test();
    // LogTest::test();
    iniParserTest::test();
    // TerminatingFunctionTest::test(); 
    // // live var test is put after the VarDisplay is initalized so the vars will be in the display
    VarDisplayTest::test();
    CommandHandlerTest::test();

    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "C++ Utilities");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::getDefault()->load("themes/Dark.txt");
    // -----------------------

    //! Adding a command for showing off the different themes
    Command::Handler::get().addCommand("setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"});
    Command::Handler::get().findCommand("setTheme") // no need to check if nullptr since we just added it
    ->addCommand("dark", "Sets the them to the dark theme", 
        [](){ 
            tgui::Theme::getDefault()->load("themes/Dark.txt");
        })
    .addCommand("black", "Sets the them to the black theme", 
        [](){ 
            tgui::Theme::getDefault()->load("themes/Black.txt");
        })
    .addCommand("grey", "Sets the them to the transparent grey theme", 
        [](){ 
            tgui::Theme::getDefault()->load("themes/TransparentGrey.txt");
        });
    //! -----------------------------------------------------

    Command::Handler::get().addCommand("tests", "Function used to test the different features of the library\nMultiple line description\nWhat about a line that is way to long to find on one line so that it wraps aroundaooidshfiogushdofiguhsdoifughisdufhgisdhfiguhsdifhgsiudhfgilsuhdfiguhsidfughsiudfhisuhdfiugsdfhiuiusdhfgoiudshfogiusdhfoiguhsdoifughsdiufhg", {Command::print, "Trying calling one of the sub commands"});
    Command::Handler::get().findCommand("tests") // no need to check if nullptr since we just added it
    ->addCommand("liveVar", "Tests the live var feature", [](){ LiveVarTest::test(); })
    .addCommand("varDisplay", "Tests the var display feature", [](){ VarDisplayTest::test(); })
    .addCommand("tFuncDisplay", "Tests the tFuncDisplay feature", [](){ TFuncDisplayTest::test(); });

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
    // TFuncDisplay::setVisible(true);

    //* LiveVarTest
    // LiveVarTest::test(); //! NOTE - live vars will not show in the VarDisplay unless added after VarDisplay is initalized

    //* GraphTest
    // GraphTest::test(gui);

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
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            //! Required CommandPrompt to work as intended
            if (Command::Prompt::UpdateEvent(event.value()))
                continue;
            //! ------------------------------------------
            
            if (gui.handleEvent(event.value()))
                continue;

            //! Required for Live Vars to work as intended
            LiveVar::UpdateLiveVars(event.value());
            //! ------------------------------------------

            if (event->is<sf::Event::Closed>())
                window.close();
        }
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime);
        //* Updates for the terminating functions display
        TFuncDisplay::Update();
        //! ------------------------------

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    return EXIT_SUCCESS;
}
