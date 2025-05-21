#include "include/Examples/iniParser.hpp"

void iniParserTest::test()
{
    using namespace std;

    //! Depends on StringHelper
    cout << "Outputs for ini Parser" << endl;
    // first we want to open the file
    iniParser file;
    file.setFile("iniParser/testing.ini");

    if (file.isOpen())
    {
        if (!file.parseData())
        {
            cout << "Unable to parse data" << endl;
        }
        else if (file.hasFormatError())
        {
            cout << "Data has format error" << endl;
            
            // This makes a copy of the data in the file and continues with the program
            // any data that was formatted properly will still be loaded
            // This will not override any older error files
            file.createCopyError();
        }
        else
        {
            cout << "Some data was parsed successfully" << endl;
        }
    }
    else
    {
        cout << "File does not exist" << endl;
        iniParser::createFile("iniParser/testing.ini");
        if (!file.setFile("iniParser/testing.ini"))
        {
            cout << "Not able to create and open file" << endl;
            return;
        }
    }

    // now that we have the file open and data loaded we can start adding data to the file
    file.insertSection("Test");
    // NOTE: No data is saved to the file until you either set a new path, delete the iniParser instance with autosave enabled (default = true), or call the save function
    // NOTE: if autosave is off then it will only save when calling the save function explicitly 
    
    file.getSection("Test")->setValue("v", "789");
    file.save();
    if (!std::filesystem::exists("iniParser/ChangedFile.ini"))
        iniParser::createFile("iniParser/ChangedFile.ini");
    if (file.setFile("iniParser/ChangedFile.ini", true))
    {
        file.save();
    }
    else
    {
        std::cout << "Could not change file location" << std::endl;
    }

    cout << "Getting the var we just added: " << *file.getSection("Test")->getValue("v") << endl;
    // Assuming that the section and value exist
    cout << "This should be the same thing: " << file["Test"]["v"] << endl;

    // creating a file if it does not already exist
    file.createFile("iniParser/CreatingFile.ini");

    //* duplicate section name loading
    {
        // creating the file with the data
        iniParser::createFile("iniParser/duplicate.ini");
        std::fstream temp = std::fstream("iniParser/duplicate.ini");
        temp << "[section]\nkey=value\n[section]\nkey2=value";
        temp.flush();
        temp.close();
    }

    file.clearData();
    file.setFile("iniParser/duplicate.ini");
    file.parseData(false);
    file.setAutosave(false);

    for (auto section : file.getData())
    {
        std::cout << section.first << std::endl;
        for (auto keyValue : section.second.getData())
        {
            std::cout << keyValue.first << "=" << keyValue.second << std::endl;
        }
    }
}