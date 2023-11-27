#include <iostream>

#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "include/Utils/TerminatingFunction.h"
#include "include/Utils/EventHelper.h"
#include "include/Utils/Stopwatch.h"
#include "include/Utils/Log.h"
#include "include/Utils/StringHelper.h"
#include "include/Utils/Debug/LiveVar.h"
#include "include/Utils/Debug/VarDisplay.h"
#include "include/Utils/Debug/CommandPrompt.h"
#include "include/Utils/Debug/CommandHandler.h"

using namespace std;
using namespace sf;

class testingClass
{
public:
    testingClass(std::string s) : str(s) {};

    void function()
    {
        cout << str << endl;
    }

private:
    std::string str;
};

int main()
{
    //* funcHelper
    // Note: funcHelper is not dependent on any other class contained in this project
    cout << "Outputs for funcHelper: " << endl;
    {
        using namespace funcHelper;

        // the usage of a func is the same with a normal function as it is with a lambda
        // just give the fucking pointer in place of the lambda
        
        // creating a func with no vars
        func<void> function([](){ cout << "A function stored in func" << endl; });
        // Note: for void returns you can also write "func<void>" as "func"

        // calling said func is as easy as calling the invoke function
        function.invoke();

        // you can reassign a func to a new function like so
        function.setFunction([](){ cout << "A new function" << endl; });
        function.invoke();

        // to set a function with input vars you have to add the input values after the function
        //! NOTE: You can NOT take in references
        function.setFunction([](std::string input){ cout << "A function with input: " << input << endl; }, "Some input");
        function.invoke();

        //* Reminder when using with a normal function it will work the same just with the function pointer in place of the lambda

        // if you are calling a function from a class then you must give a pointer to an instance of the class as an input
        // EX 
        testingClass temp("Some stored data in class");
        function.setFunction(&testingClass::function, &temp);
        // Note: you can also call a func as if it where a normal function
        function();

        // the only difference between a function with no return value and one with a return value is that one returns something
        func<std::string> rtnFunction([](){ return "A returned value"; });
        cout << rtnFunction() << endl;

        //* now onto dynamic functions
        // You have the option between a function that takes in 1-5 vars which are dynamic and an infinite amount of static vars (set at creation)
        // Note: you can not return anything with dynamic functions
        // Note: the function being assigned is not required to take in a dynamic var (could treat the funcDynamic as a normal func)
        // Ex of one dynamic var
        funcDynamic<std::string> dynFunction([](std::string str){ cout << str << endl;});
        // when calling the function you must give an input even if the function does not use it
        dynFunction.invoke("A dynamic input");
        dynFunction("Another dynamic input");

        // if you dont want to use the dynamic functionality
        dynFunction.setFunction([](std::string str){ cout << str << endl; }, "Const input with dynamic func");
        dynFunction.invoke("Dynamic input that is not used");

        // when using funcDynamic up to 5 it works the same as with one
        // - you can either use the dynamic inputs or not
        // - you have to input them when calling the function
        // - you can use only some of the dynamic inputs
    }
    //* ----------

    //* EventHelper 
    // Note: Event help is not dependent on any other classes in this project
    cout << "Outputs for EventHelper: " << endl;
    {
        // EventHelper is assigned functions the same way as funcHelper
        // the main difference is you call ".connect" or use the "()" operator to add a function to the event
        EventHelper::Event event;
        event.connect([](){ cout << "An callback for an event" << endl; });
        event([](){ cout << "Another callback for an event" << endl; });
        // to call an event you must use ".invoke"
        event.invoke();

        // will disable the event, the callbacks are no longer called when invoking
        event.setEnabled(false);
        cout << "When disabled: " << endl;
        event.invoke();

        event.setEnabled(true);
        cout << "When enabled: " << endl;
        event.invoke();

        // removing all callbacks
        event.disconnectAll();
        cout << "After disconnecting all callbacks: " << endl;
        event.invoke();

        // dynamic events are assigned the same as a funcDynamic
        // when invoking you must give all the inputs
        // has the same features as a normal event
    }
    //* -----------

    //* Stopwatch
    cout << "Outputs for Stopwatch: " << endl;
    {
        using namespace timer;

        // when creating the stopwatch it starts the timer
        Stopwatch timer; 
        
        // but you can restart the timer by calling "start"
        timer.start();
        std::this_thread::sleep_for(std::chrono::duration<double, std::ratio<1,1>>(1)); // sleeping this thread for 1 second
        cout << timer.lap<Stopwatch::TimeFormat::MILLISECONDS>() << endl;
        cout << float(timer.elapsed<Stopwatch::TimeFormat::MILLISECONDS>())/1000.0 << endl; // printing seconds as float
    }

    //* String Helper
    cout << "Outputs for StringHelper: " << endl;
    {
        // the main function continued in StringHelper are the following: 
        //      trim, toLower, toInt, toUInt, toULong, toFloat, toLongDouble, toBool, FloatToStringRound
        // the rest of the functions serve the same functionality 

        // for example trim and trim_copy
        std::string str = "      a string that needs to be trimmed          ";
        cout << "Untrimmed string: " << str << endl;
        cout << "Trimmed copy string: " << StringHelper::trim_copy(str) << endl;
        cout << "Original string: " << str << endl;
        StringHelper::trim(str);
        cout << "Trimmed original string: " << str << endl;

        // if you dont want to use toInt you could also use attemptToInt
        
    }

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
