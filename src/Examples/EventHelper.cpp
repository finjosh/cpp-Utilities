#include "include/Examples/EventHelper.hpp"

void EventHelperTest::test()
{
    using namespace std;

    cout << "--- EventHelper --- " << endl;
    // EventHelper is assigned functions the same way as funcHelper
    // the main difference is you call ".connect" or use the "()" operator to add a function to the event
    EventHelper::Event event;
    event.connect([](){ cout << "An callback for an event" << endl; });
    event([](){ cout << "Another callback for an event" << endl; });
    // to call an event you must use ".invoke"
    event.invoke();

    // will disable the event, the callbacks are no longer called when invoking
    event.setEnabled(false);
    cout << "Invoking when disabled: " << endl;
    event.invoke();

    event.setEnabled(true);
    cout << "Invoking when enabled: " << endl;
    event.invoke();

    // removing all callbacks
    event.disconnectAll();
    cout << "Invoking after disconnecting all (public) callbacks: " << endl;
    event.invoke();

    cout << "-- Public vs. Private callbacks --" << endl;
    cout << "- Public ids decrease while private ids increase" << endl;
    cout << "- Public callbacks are invoked in the reveres order that they where added" << endl;
    cout << "- Private callbacks are invoked in the order they where added" << endl;
    EventHelper::Event event2;
    cout << "Event ID: " << event2.connect([](){ cout << "Public Function 1" << endl; }) << endl;
    cout << "Event ID: " << event2.connectPrivate([](){ cout << "Private Function 1" << endl; }) << endl;
    cout << "Event ID: " << event2.connect([](){ cout << "Public Function 2" << endl; }) << endl;
    cout << "Event ID: " << event2.connectPrivate([](){ cout << "Private Function 2" << endl; }) << endl;
    cout << "- Invoking -" << endl;
    event2.invoke();
    cout << "Callback count: " << event2.getNumCallbacks() << endl;
    event2.disconnectAll();
    cout << "Callback count after disconnectAll(): " << event2.getNumCallbacks() << endl;
    cout << "- Invoking -" << endl;
    event2.invoke();

    cout << "-- Changing a callback based on id --" << endl;
    EventHelper::Event event3;
    cout << "Event ID: " << event3.connect([](){ cout << "Public Function 1" << endl; }) << endl;
    cout << "Event ID: " << event3.connectPrivate([](){ cout << "Private Function 1" << endl; }) << endl;
    cout << "Event NextPublicId: " << event3.getNextPublicID() << endl;
    cout << "Event NextPrivateId: " << event3.getNextPrivateID() << endl;
    cout << "- Invoking -" << endl;
    event3.invoke();
    cout << "Changing the callbacks" << endl;
    event3.setCallback(event3.getNextPublicID()+1, [](){ cout << "Updated Public Function 1" << endl; });
    event3.setCallback(event3.getNextPrivateID()-1, [](){ cout << "Updated Private Function 1" << endl; });
    cout << "Event NextPublicId: " << event3.getNextPublicID() << endl;
    cout << "Event NextPrivateId: " << event3.getNextPrivateID() << endl;
    cout << "- Invoking -" << endl;
    event3.invoke();

    // For thread safe events you need to call the update function in the main
    // To call an event in thread safe mode just pass in true when calling it
    event.invoke(true);
    // For dynamic events the bool is the last input
    // Thread safe is default to false as the events are not called immediately

    // dynamic events are assigned the same as a funcDynamic
    // when invoking you must give all the inputs
    // has the same features as a normal event
    cout << "--- End EventHelper --- " << endl;
}