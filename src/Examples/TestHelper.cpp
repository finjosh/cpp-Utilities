#include "Examples/TestHelper.hpp"

void TestHelperTest::test()
{
    size_t temp = 0; // temp var for storing the current iteration of our test
    // setting up the test
    // Named: Power function
    // The "x" data is 2 to the power of the "x" value
    // The test function is calling the power function with 2 to the power of the current iteration
    // Iterating from 0 to 100000 for the "x" value
    // The reset function is reseting the value of temp to 0
    // We are going to repeat the test 2 times to get rid of outliers
    TestHelper::initTest("Power function", "2^x", {[&temp](){ std::pow(2, temp); }}, {[&temp](){ temp++; }}, 100000, 0, {[&temp](){ temp = 0; }}, 2);
    // Run the test
    // do nothing if there is already a file that has the name "Power functionTestTest Data.ini"
        // "Power function" is the function name
        // "Test Data" is the suffix added in the run test function for easier reference later
        // "Test.ini" is the default prefix
    TestHelper::runTest(TestHelper::FileExists::DoNothing, "Test Data");

    temp = 0;
    TestHelper::initTest("Division function", "Divisor (2/x)", {[&temp](){ 2/temp; }}, {[&temp](){ temp++; }}, 100000, 0, {[&temp](){ temp = 0; }}, 2);
    TestHelper::runTest(TestHelper::FileExists::DoNothing, "Test Data");

    // graphing all the .ini files in the exe's directory that have the prefix "Test Data"
    TestHelper::graphData("", "Test Data");
}
