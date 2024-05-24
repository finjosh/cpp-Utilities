#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#pragma once

#include <numeric>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "Utils/iniParser.hpp"
#include "Utils/Stopwatch.hpp"
#include "Utils/funcHelper.hpp"
#include "Utils/Graph.hpp"

class TestHelper
{
public:
    enum FileExists
    {
        Replace = 0,
        DoNothing = 1, // skips the test
        MakeNew = 2 // adds a counter to the end of the new file as windows does
    };

    /// @brief sets the current name for collected data
    static void setName(const std::string& name);
    /// @returns the current name
    static std::string getName();
    /// @brief resets all data back to default
    static void reset();
    /// @brief sets up the test to be run
    /// @param name the name of this test
    /// @param xName the name of the iterated values
    /// @param test the function that we are testing time for
    /// @param iterateTest how we iterate to the next part of the test (should iterate by 1 in the X)
    /// @param resetTest the function that will reset the test (only required if doing multiple repetitions)
    /// @param iterations the number of iterations the test has
    /// @param repetitions the number of times we want to run the test (used to find average)
    /// @param timeFormat how the time will be stored (it will always be recorded in nanoseconds but converted to the given format)
    /// @param startingValue the starting value for iteration (when reseting will call iterateTest until reaching this value without testing)
    static void initTest(const std::string& name, const std::string& xName,
                         const funcHelper::func<>& test, const funcHelper::func<>& iterateTest, const size_t& iterations, const size_t& startingValue = 0,
                         const funcHelper::func<>& resetTest = {[](){}}, const size_t& repetitions = 1, const timer::TimeFormat& timeFormat = timer::TimeFormat::MILLISECONDS);
    /// @note This runs the test and opens a window that shows the progress of the test
    /// @note this also saves the data collected to a 'name of test'Test.ini file
    /// @warning if the window is closed early then nothing will be saved
    /// @param fileExists an enum to decide what happens if the file already exists
    /// @param suffix the suffix that will be added to the file name (added after number add if file name had copies)
    /// @param folderPath the folder to put the files in
    /// @param inf the value that infinity and -infinity values will be converted to (graph will break otherwise)
    /// @returns the name of the file that was made (if no file was made returns "")
    static std::string runTest(const TestHelper::FileExists& fileExists = TestHelper::MakeNew, const std::string& suffix = "", std::string folderPath = "", const float& inf = 0);
    static void setXName(const std::string& name);
    static std::string getXName();
    /// @brief opens a window that attempts to graph all .ini files in the given folder
    /// @param folder the folder where the ini files are stored (put "" for the current folder)
    /// @param suffix the required suffix for the file to have (anything before the .ini)
    /// @note creates a thread when ever loading a graph
    static void graphData(const std::string& folder = "", const std::string& suffix = "");
    /// @brief opens a window that attempts to graph all .ini files in the given folder
    /// @param files the list of paths to files that are to be graphed
    static void graphData(const std::list<std::string>& files);

protected:
    static bool makeGraph(Graph& graph, const iniParser& data, const float& thickness = 5);

private:
    inline TestHelper() = default;

    static funcHelper::func<> m_test;
    static funcHelper::func<> m_iterateTest;
    static funcHelper::func<> m_resetTest;
    static size_t m_iterations;
    static size_t m_startingValue;

    static std::string m_name;
    static std::string m_xName;
    /// @brief the total number of tests to run
    static size_t m_repetitions;
    static size_t m_currentTest;
    static timer::TimeFormat m_timeFormat;
    static std::vector<float> m_yData;
};

#endif
