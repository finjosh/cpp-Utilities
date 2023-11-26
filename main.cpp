#include <iostream>

#include "SFML\Graphics.hpp"
#include "TGUI\TGUI.hpp"
#include "TGUI\Backend\SFML-Graphics.hpp"

#include "include\Utils\TerminatingFunction.h"
#include "include\Utils\EventHelper.h"
#include "include/Utils/Debug/LiveVar.h"
#include "include/Utils/Debug/VarDisplay.h"
#include "include/Utils/Debug/CommandPrompt.h"
#include "include/Utils/Debug/CommandHandler.h"

using namespace std;
using namespace sf;

int main()
{
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Industralia");
    window.setFramerateLimit(144);
    window.setPosition(Vector2i(-8, -8));

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});

    //! USED FOR TESTING AND DEBUGGING
    VarDisplay::init(gui);
    CommandPrompt::init(gui);
    //! ------------------------------

    float deltaTime = 0;
    while (window.isOpen())
    {
        window.clear();
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            //! USED FOR TESTING AND DEBUGGING
            LiveVar::UpdateLiveVars(event);
            CommandPrompt::UpdateEvent(event);
            //! ------------------------------

            if (event.type == sf::Event::Closed)
                window.close();
        }
        //! USED FOR TESTING AND DEBUGGING
        VarDisplay::Update();
        //! ------------------------------
        //* Updating Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime);
        //* ------------------------------

        gui.draw();

        window.display();
    }

    //! USED FOR TESTING AND DEBUGGING
    VarDisplay::close();
    CommandPrompt::close();
    //! ------------------------------

    return 1;
}
