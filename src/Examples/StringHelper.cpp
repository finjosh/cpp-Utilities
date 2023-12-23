#include "include/Examples/StringHelper.hpp"

void StringHelperTest::test()
{
    using namespace std;

    cout << "Outputs for StringHelper: " << endl;
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

    // the following example is basically the same this for floats and the other supported variable types
    int temp = StringHelper::toInt("1234", 0);
    // if you dont want to use toInt you could also use attemptToInt
    if (StringHelper::attemptToInt("1234", temp))
        cout << "Successfully converted to int" << endl;
    // useful when you need to know if it was successfully converted
}