#include <atomic>
#include "Utils/TestHelper.hpp"
#include <thread>
#include "TGUI/DefaultFont.hpp"
#include "Utils/TerminatingFunction.hpp"
#include "Utils/TGUICommon.hpp"

std::string TestHelper::m_name = "Unknown";
funcHelper::func<> TestHelper::m_test = {[](){}};
funcHelper::func<> TestHelper::m_iterateTest = {[](){}};
funcHelper::func<> TestHelper::m_resetTest = {[](){}};
size_t TestHelper::m_iterations = 1;
size_t TestHelper::m_startingValue = 0;
std::string TestHelper::m_xName = "X";
size_t TestHelper::m_repetitions = 1;
size_t TestHelper::m_currentTest = 1;
timer::TimeFormat TestHelper::m_timeFormat = timer::TimeFormat::MILLISECONDS;
std::vector<float> TestHelper::m_yData;

void TestHelper::setName(const std::string& name)
{
    m_name = name;
}

std::string TestHelper::getName()
{
    return m_name;
}

void TestHelper::reset()
{
    m_name = "Unknown";
    m_xName = "X";
    m_repetitions = 1;
    m_currentTest = 0;
    m_test.setFunction([](){});
    m_iterateTest.setFunction([](){});
    m_resetTest.setFunction([](){});
    m_iterations = 1;
    m_startingValue = 0;
    m_yData.clear();
    m_timeFormat = timer::TimeFormat::MILLISECONDS;
}

void TestHelper::initTest(const std::string& name, const std::string& xName,
                          const funcHelper::func<>& test, const funcHelper::func<>& iterateTest, size_t iterations, size_t startingValue,
                          const funcHelper::func<>& resetTest, size_t repetitions, timer::TimeFormat timeFormat)
{
    reset();
    m_name = name;
    m_xName = xName;
    m_test = test;
    m_iterateTest = iterateTest;
    m_resetTest = resetTest;
    m_iterations = iterations;
    m_startingValue = startingValue;
    m_repetitions = repetitions;
    m_timeFormat = timeFormat;
    m_yData.resize(iterations);
}

std::string TestHelper::runTest(TestHelper::FileExists fileExists, const std::string& suffix, std::string folderPath, float inf)
{
    if (!std::filesystem::is_directory(folderPath))
    {
        folderPath = "";
    }

    iniParser data;
    // if the y data size there is no test to do
    data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    if (m_yData.size() == 0 || (data.isOpen() && fileExists == FileExists::DoNothing))
    {
        return "";
    }

    auto desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width / 3, desktopMode.height / 3, desktopMode.bitsPerPixel), "Test: " + m_name, sf::Style::Resize);
    tgui::Gui gui{window};

    std::list<std::string> themePaths = {"Assets/themes/Dark.txt", "themes/Dark.txt",
                                         "Assets/themes/Black.txt", "themes/Black.txt"};

    for (auto path: themePaths)
    {
        if (std::filesystem::exists(path))
        {
            tgui::Theme::setDefault(path);
            break;
        }
    }

    gui.setTextSize(window.getSize().x/20);

    auto panel = tgui::HorizontalWrap::create();
    panel->setAutoLayoutUpdateEnabled(true);
    auto testProgress = tgui::ProgressBar::create();
    auto progress = tgui::ProgressBar::create();
    auto subPanel = tgui::Panel::create();
    auto IPS = tgui::Label::create("IPS"); // iterations per second
    auto lastRuntime = tgui::Label::create("IT"); // time of the last iteration
    auto time = tgui::Label::create("Run Time: 0s");
    auto pause_resume = tgui::Button::create("Pause");
    auto stop = tgui::Button::create("Stop");

    // Setup for testProgress
    panel->add(testProgress);
    testProgress->setSize({"100%", "25%"});
    testProgress->setText("Test: " + std::to_string(m_currentTest) + "/" + std::to_string(m_repetitions));
    testProgress->setMaximum(m_repetitions);

    // setup for progress
    panel->add(progress);
    progress->setSize({"100%", "25%"});
    progress->setText("Test Progress: 0%");
    progress->setMaximum(m_iterations);

    // setup for extra info
    panel->add(subPanel);
    subPanel->setSize({"100%","50%"});
    subPanel->add(time);
    time->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    time->setSize({"66%", "33%"});
    subPanel->add(IPS);
    IPS->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    IPS->setSize({"66%", "33%"});
    IPS->setPosition({0, "33%"});
    subPanel->add(lastRuntime);
    lastRuntime->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    lastRuntime->setSize({"66%", "33%"});
    lastRuntime->setPosition({0, "66%"});
    subPanel->add(pause_resume);
    pause_resume->setSize({"34%", "50%"});
    pause_resume->setPosition({"66%", 0});
    bool paused = false;
    pause_resume->onClick([&paused, pause_resume](){ paused = !paused; pause_resume->setText(paused ? "Resume" : "Pause"); });
    subPanel->add(stop);
    stop->setSize({"34%", "50%"});
    stop->setPosition({"66%", "50%"});
    bool STOP = false;
    stop->onClick([&STOP, stop](){ if (stop->getText() == "Stop") stop->setText("Confirm"); else STOP = true; });
    stop->onUnfocus([stop](){ stop->setText("Stop"); });
    
    gui.add(panel);

    panel->setSize({"100%", "100%"});
    panel->updateChildrenWithAutoLayout();

    timer::Stopwatch timer;
    size_t iter = 0;
    double conversionFactor = 1.0;
    std::string timeFormatStr = "ns";
    switch (m_timeFormat)
    {
    case timer::SECONDS:
        conversionFactor = 1000000000.0;
        timeFormatStr = "s";
        break;
    
    case timer::MILLISECONDS:
        conversionFactor = 1000000.0;
        timeFormatStr = "ms";
        break;

    case timer::MICROSECONDS:
        conversionFactor = 1000.0;
        timeFormatStr = "mus";
        break;

    default:
        break;
    }

    for (size_t i = 0; i < m_startingValue; i++)
    {
        m_iterateTest.invoke();
    }

    sf::Clock deltaClock;
    float deltaTime = 0;
    float second = 0;
    float totalTime = 0;
    size_t ips = 0;
    while (window.isOpen())
    {
        if (STOP) return "";

        deltaTime = deltaClock.restart().asSeconds();
        second += deltaTime;
        window.clear();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                return "";

            if (event.type == sf::Event::Resized)
            {
                panel->setSize(window.getSize().x, window.getSize().y);
                panel->updateChildrenWithAutoLayout();
                gui.setTextSize(window.getSize().x/20);
            }

            gui.handleEvent(event);
        }

        if (!paused)
        {
            if (iter >= m_iterations)
            {
                m_currentTest++;
                if (m_currentTest >= m_repetitions)
                    break;
                iter = 0;
                m_resetTest.invoke();
                testProgress->setText("Test: " + std::to_string(m_currentTest) + "/" + std::to_string(m_repetitions));
                testProgress->setValue(m_currentTest);

                for (size_t i = 0; i < m_startingValue; i++)
                {
                    m_iterateTest.invoke();
                }
            }
            
            uint64_t nanoSec;
            timer.start();
            m_test.invoke();
            nanoSec = timer.lap<timer::nanoseconds>();
            m_yData[iter] = (float)(nanoSec/conversionFactor); // using y data as total for now
            iter++;
            m_iterateTest.invoke();

            progress->setText("Test Progress: " + StringHelper::FloatToStringRound((float)(iter)/(m_iterations)*100, 1) + "%");
            progress->setValue(iter);
            totalTime += deltaTime;
            time->setText("Run Time: " + StringHelper::FloatToStringRound(totalTime, 1) + "s");
            lastRuntime->setText("IT: " + std::to_string((float)(nanoSec/conversionFactor)) + timeFormatStr);
            if (second >= 1)
            {
                IPS->setText("IPS: " + std::to_string(ips));
                ips = 0;
                second = 0;
            }
            ips++;
        }

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }
    window.close();

    // finding the average of the data collected
    for (size_t i: m_yData)
    {
        m_yData[i] /= m_repetitions;
        if (abs(m_yData[i]) == std::numeric_limits<float>::infinity())
            m_yData[i] = inf;
    }

    size_t temp = 0;
    data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    while (data.isOpen() && fileExists != FileExists::Replace)
    {
        temp++;
        data.setFilePath(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
    }

    if (temp == 0)
    {
        data.createFile(folderPath + m_name + "Test" + suffix + ".ini");
        data.setFilePath(folderPath + m_name + "Test" + suffix + ".ini");
    }
    else
    {
        data.createFile(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
        data.setFilePath(folderPath + m_name + "Test (" + std::to_string(temp) + ")" + suffix + ".ini");
    }
    data.overrideData();
    data.addValue("General", "XLabel", m_xName);
    data.addValue("General", "YLabel", "Time (" + timeFormatStr + ")");
    data.addValue("General", "Average Test Time", std::to_string(totalTime/m_repetitions));
    data.addValue(m_name, "Values", StringHelper::fromVector<float>(m_yData));
    std::vector<size_t> xValues;
    xValues.resize(m_iterations);
    std::iota(xValues.begin(), xValues.end(), m_startingValue);
    data.addValue("General", "X", StringHelper::fromVector<size_t>(xValues));
    data.SaveData();
    return data.getFilePath(); // path is just the file name in this case
}

void TestHelper::setXName(const std::string& name)
{
    m_xName = name;
}

std::string TestHelper::getXName()
{
    return m_xName;
}

void TestHelper::graphData(const std::string& folder, const std::string& suffix)
{
    if (folder != "" && !std::filesystem::is_directory(folder))
        return;

    std::list<std::string> files;
    for (const auto& entry: std::filesystem::directory_iterator(folder == "" ? std::filesystem::current_path() : folder))
    {
        if (entry.path().filename().generic_string().ends_with(suffix + ".ini"))
            files.push_back(entry.path().filename().generic_string());
    }

    TestHelper::graphData(files);
}

void TestHelper::graphData(const std::list<std::string>& files)
{
    auto screenMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(screenMode, "Test: " + m_name, sf::Style::Fullscreen);
    tgui::Gui gui{window};
    gui.setTextSize(window.getSize().x/100);

    std::list<std::string> themePaths = {"Assets/themes/Dark.txt", "themes/Dark.txt",
                                         "Assets/themes/Black.txt", "themes/Black.txt"};

    for (auto path: themePaths)
    {
        if (std::filesystem::exists(path))
        {
            tgui::Theme::setDefault(path);
            break;
        }
    }

    auto Next = tgui::Button::create("Next");
    auto Last = tgui::Button::create("Last");
    auto label = tgui::Label::create("No files available");
    auto childWindow = tgui::ChildWindow::create("Navigation", tgui::ChildWindow::TitleButton::None);
    tguiCommon::ChildWindow windowData;
    windowData.setMinimize_Maximize(childWindow);
    childWindow->setResizable(false);
    gui.add(childWindow);
    childWindow->setSize({"20%","15%"});
    childWindow->setPosition({"30%", "5%"});
    childWindow->add(Next);
    childWindow->add(Last);
    Next->setSize("20%", "66%");
    Next->setPosition("80%",0);
    Last->setSize("20%", "66%");
    Last->setPosition(0,0);
    childWindow->add(label);
    label->setSize({"60%", "66%"});
    label->setMaximumTextWidth(label->getSize().x);
    label->setPosition({"20%", 0});
    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    auto filesBox = tgui::ComboBox::create();
    childWindow->add(filesBox);
    filesBox->setSize({"100%", "34%"});
    filesBox->setPosition({0, "66%"});
    for (auto path: files)
    {
        filesBox->addItem(path, path);
    }

    std::list<std::string>::const_iterator currentFile = files.begin();

    enum graphState 
    {
        Updating = 0,
        Finished = 1,
        Failed = 2
    };
    std::atomic<graphState> state = graphState::Finished;
    std::thread* graphThread = nullptr;
    Graph graph;
    sf::Font font;
    font.loadFromMemory(static_cast<const unsigned char*>(defaultFontBytes), sizeof(defaultFontBytes));
    graph.setFont(font);
    graph.setResolution({window.getSize().x, window.getSize().y});
    graph.setSize({(float)window.getSize().x, (float)window.getSize().y});
    graph.setDecimalPrecision(3);

    auto updateGraph = [&graph, &state, label, &graphThread, filesBox, Next, Last](std::string path){         
        state = graphState::Updating;

        filesBox->setEnabled(false);
        Next->setEnabled(false);
        Last->setEnabled(false);

        auto id = TFunc::Add([label](TData* data){
            data->setRunning();
            std::string temp = "Loading";
            for (int i = 0; i < (int)(data->totalTime*2)%3 + 1; i++)
                temp += '.';
            label->setText(temp);
        });
        graphThread = new std::thread([&graph, &state, path, id, filesBox, Next, Last](){ 
            iniParser temp(path);
            if (temp.LoadData() && makeGraph(graph, temp)) 
                state = graphState::Finished; 
            else 
                state = graphState::Failed; 
            TerminatingFunction::remove(id);
        }); 
    };

    filesBox->onItemSelect([Next, Last, &currentFile, &files, &graph, label, &updateGraph](const tgui::String& item){
        if (item != *currentFile)
        {   
            currentFile = std::find(files.begin(), files.end(), item.toStdString()); // there should be no item in the list that is not in the files list
            label->setText(*currentFile);
            Last->setEnabled(currentFile != files.begin());
            Next->setEnabled(currentFile != --files.end());
            updateGraph(*currentFile);
        }
    });
    Next->setEnabled(currentFile != --files.end());
    Next->onClick([Last, Next, &currentFile, label, &files, &graph, filesBox, &updateGraph](){
        currentFile++;
        label->setText(*currentFile);
        Last->setEnabled(currentFile != files.begin());
        Next->setEnabled(currentFile != --files.end());
        updateGraph(*currentFile);
        filesBox->setSelectedItem(*currentFile);
    });
    Last->setEnabled(false);
    Last->onClick([Last, Next, &currentFile, label, &files, &graph, filesBox, &updateGraph](){
        currentFile--;
        label->setText(*currentFile);
        Last->setEnabled(currentFile != files.begin());
        Next->setEnabled(currentFile != --files.end());
        updateGraph(*currentFile);
        filesBox->setSelectedItem(*currentFile);
    });

    if (files.size() != 0)
    {
        label->setText(*currentFile);
        updateGraph(*currentFile);
        filesBox->setSelectedItem(*currentFile);
    }

    sf::Clock deltaClock;
    float deltaTime = 0;
    while (window.isOpen())
    {
        deltaTime = deltaClock.restart().asSeconds();
        window.clear();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

            gui.handleEvent(event);
        }
        TerminatingFunction::UpdateFunctions(deltaTime);

        window.draw(graph);
        if (graphThread && state != graphState::Updating)
        {
            if (state == graphState::Finished)
                label->setText(*currentFile);
            else if (state == graphState::Failed)
                label->setText("Failed to Graph");
            graphThread->join();
            graphThread = nullptr;
            filesBox->setEnabled(true);
            Next->setEnabled(true);
            Last->setEnabled(true);
        }

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }
    window.close();
    graphThread->join();
}

bool TestHelper::makeGraph(Graph& graph, const iniParser& data, float thickness)
{
    if (data.getValue("General", "X") == "\0")
        return false;

    graph.clearDataSets();
    graph.setXLable(data.getValue("General", "XLabel"));
    graph.setYLable(data.getValue("General", "YLabel"));
    GraphData graphData;
    graphData.setXValues(StringHelper::toVector<float>(data.getValue("General", "X")));
    
    for (auto i: data.getLoadedData())
    {
        if (i.first == "General")
            continue;
        graphData.setYValues(StringHelper::toVector<float>(data.getValue(i.first, "Values")));
        graphData.setLabel(i.first);

        graphData.setThickness(thickness);

        graph.addDataSet(graphData);
        break;
    }
    graph.Update();
    return true;
}
