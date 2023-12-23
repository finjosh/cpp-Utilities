#include "include/Examples/funcHelper.hpp"

void funcHelperTest::test()
{
    using namespace std;
    using namespace funcHelper;

    cout << "Outputs for funcHelper: " << endl;

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

    // how function type ids work
    cout << function.getTypeid() << endl;
    function.setFunction([](){});
    cout << function.getTypeid() << endl;
    function.setFunction([](int i){}, 345);
    cout << function.getTypeid() << endl;
    function.setFunction([](int i){}, 3);
    cout << function.getTypeid() << endl;
    function.setFunction(testingClass::function2, &temp, 1);
    cout << function.getTypeid() << endl;
    function.setFunction(testingClass::function2, &temp, 2);
    cout << function.getTypeid() << endl;
    // The summary of how function type ids work is the following:
    //      - No matter what you input into the function it will have the same name
    //      - The id is given base on the give functions input (NOT what you gave as input but what the function requires i.e. int, float) and which class it was created in
    //      - Lambda functions will never have the same id even if they are the exact same

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
