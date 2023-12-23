#include "include/Examples/LiveVar.hpp"

void LiveVarTest::test()
{
    using namespace std;

    //! Depends on SFML Events, FuncHelper, and EventHelper
    cout << "Outputs for LiveVar:" << endl;

    // To start you have to initialize a Var
    //     - LiveVar only supports floats
    // There are three ways to do this
    // 1. A Var that can only be updated through the setter function
    cout << "Creating Var SomeVar:" << endl;
    cout << LiveVar::initVar("SomeVar", 7) << endl;

    // 2. A Var that can be changed by a given increment and key inputs
    //        - With this var you can implement a min and max for the value
    cout << "Creating Var Increment:" << endl;
    cout << LiveVar::initVar("Increment", 0, 1.5, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, 0, 75) << endl;

    // 3. A Var that has specific key binds for specific values
    cout << "Creating Var KeyBinds:" << endl;
    cout << LiveVar::initVar("KeyBinds", 0, VALUE_SET{sf::Keyboard::Key::Numpad0, 0},
                                    VALUE_SET{sf::Keyboard::Key::Numpad1, 1},
                                    VALUE_SET{sf::Keyboard::Key::Numpad2, 2},
                                    VALUE_SET{sf::Keyboard::Key::Numpad3, 3},
                                    VALUE_SET{sf::Keyboard::Key::Numpad4, 4},
                                    VALUE_SET{sf::Keyboard::Key::Numpad5, 5},
                                    VALUE_SET{sf::Keyboard::Key::Numpad6, 6},
                                    VALUE_SET{sf::Keyboard::Key::Numpad7, 7}) << endl;
    // to get and set the value of a Var use the getter and setter functions

    // if you want a function to be called when ever a Var is changed 
    // you can add it to the Vars event like so
    LiveVar::getVarEvent("Increment")->connect([](const float& v){ cout << "Increments new value: " << v << endl; });

    // to remove a Var you use the remove functions

    //! NOTE you have to call the update function for every event that SFML gets
    // Look in the main for an example of the function being used
}
