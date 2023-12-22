#include "include/Examples/iniParser.h"

void iniParserTest::test()
{
    using namespace std;

    //! Depends on StringHelper
    cout << "Outputs for ini Parser" << endl;
    // first we want to open the file
    iniParser file;
    file.setFilePath("testing.ini");

    if (file.isOpen())
    {
        file.LoadData();
        if (!file.isDataLoaded())
        {
            cout << "Data was not loaded" << endl;
            // overriding data so we can write to the file
            file.overrideData();
        }
        else if (file.isFormatError())
        {
            cout << "Data has format error" << endl;
            
            // This makes a copy of the data in the file and continues with the program
            // any data that was formatted properly will still be loaded
            // This will not override any older error files
            file.CopyFile_Error();
        }
    }
    else
    {
        cout << "File was not opened" << endl;
    }

    // now that we have the file open and data loaded we can start adding data to the file
    file.addSection("Test");
    // NOTE: No data is saved to the file until you either set a new path, delete the iniParser instance, or call the save function
    // NOTE: if autosave is off then it will only save when calling the save function explicitly 
    
    file.addValue("Test", "v", "789");
    file.SaveData();

    cout << "Getting the var we just added: " << file.getValue("Test", "v") << endl;
}