#include "include\Examples\EventHelper.hpp"

void EventHelperTest::test()
{
    using namespace std;

    cout << "Outputs for EventHelper: " << endl;
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

    // For thread safe events you need to call the update function in the main
    // To call an event in thread safe mode just pass in true when calling it
    event.invoke(true);
    // For dynamic events the bool is the last input
    // Thread safe is default to false as the events are not called immediately

    // dynamic events are assigned the same as a funcDynamic
    // when invoking you must give all the inputs
    // has the same features as a normal event
}