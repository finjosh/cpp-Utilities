#include "include/Examples/StringHelper.hpp"

template <typename T>
void printList(std::list<T> list)
{
    for (auto i: list)
    {
        std::cout << std::to_string(i) << ",";
    }
    std::cout << "\n";
}

void StringHelperTest::test()
{
    using namespace std;

    printList<float>(StringHelper::toList<float>("[7,6,5]", StringHelper::toFloat));
    printList<float>(StringHelper::toList<float>("7,6,5]", StringHelper::toFloat));
    printList<float>(StringHelper::toList<float>("7,6,5", StringHelper::toFloat));
    printList<float>(StringHelper::toList<float>("[7 ,6,5] ", StringHelper::toFloat));
    printList<float>(StringHelper::toList<float>("[7,6 , 5,4,34,5,6,645,45,45,45,645,45,64,56,,456,4,56,45,6456, ]", StringHelper::toFloat));
    printList<float>(StringHelper::toList<float>(StringHelper::fromList<int>({7,6,5,34,4,5,67,7,4,3,4,5,76}), StringHelper::toFloat));
    std::list<float> tempList = {7,6,5,34,4,5,67,7,4,3,4,5,76};
    printList<float>(StringHelper::toList<float>(StringHelper::fromContainer<std::list<float>::iterator>(tempList.begin(), tempList.end()), StringHelper::toFloat));
    std::vector<float> tempVector = {7,6,5,34,4,5,67,7,4,3,4,5,76};
    for (auto i: StringHelper::toVector<float>(StringHelper::fromContainer<std::vector<float>::iterator>(tempVector.begin(), tempVector.end()), StringHelper::toFloat))
    {
        std::cout << std::to_string(i) << ",";
    }
    std::cout << "\n";

    cout << StringHelper::fromList<float>({7,6,5,34,4,5,67,7,4,3,4,5,76}) << endl;
    cout << StringHelper::fromList<std::string>({"7","6","5","34","4","5","67","7","4","3","4","5","76"}, [](std::string str){ str.insert(str.begin(),'\"'); str.push_back('\"'); return str; }) << endl;
    cout << StringHelper::fromList<int>({7,6,5,34,4,5,67,7,4,3,4,5,76}) << endl;    

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