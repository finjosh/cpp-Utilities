#ifndef INIPARSER_H
#define INIPARSER_H

#pragma once

#include "StringHelper.hpp"

#include <filesystem>
#include <string>
#include <fstream>
#include <map>

//* -------------------------------------------

// ['SectionName']
// eg. [GraphicSettings]

// KeyName=KeyValue
// eg. Name=Josh

//* -------------------------------------------

class iniParser
{
public:

    struct formatError
    {
        bool key = false;
        bool section = false;
    };

    /// @brief creates the 'iniParser' with a 'null' file path not opening any file
    iniParser();
    /// @brief creates the 'iniParser' opening the given file
    /// @note Does NOT load the data
    iniParser(const std::string& generic_path);

    /// @brief saves the loaded data before closing the folder and this obj being destructed
    ~iniParser();

    /// @note Does NOT load the data
    /// @warning closes (no matter what) and saves the file that is open (if auto save is true)
    /// @returns true if the file was found and opened, false otherwise
    bool setFile(const std::string& generic_path);
    /// @brief returns a copy of the filepath
    std::string getFilePath() const;

    /// @returns true if there is an open file
    bool isOpen() const;
    /// @returns true if the data was loaded
    /// @note there could still be format errors (i.e. lost data)
    bool isDataLoaded() const;
    /// @brief allows the iniParser to override any data that was in the file (data does not have to be loaded first)
    /// @note if there was any data already loaded that data will be kept, also sets (isDataLoaded=true)
    void overrideData();

    /// @brief loads the 'KeyName' and the value it has into a 'Dictionary' (map)
    /// @exception if the file had no properly formatted data returns false
    /// @exception any faulty formatted data is erased
    /// @warning if any data was loaded before it will be unloaded
    /// @returns if data was loaded (NOT if there was a format error)
    bool loadData();

    /// @brief counts as loading data and will override any data in the opened file
    /// @returns false if there is no file open 
    bool setData(const std::map<std::string, std::map<std::string, std::string>>& Data);

    /// @brief unloads the data that is stored
    /// @warning will save the data if autosave is on
    /// @note does not close the file
    void unloadData();

    /// @note if there was no data in the file then there was no format error
    /// @returns true for a format error
    bool isFormatError() const;

    /// @brief erases the errors from loading data
    void clearFormatErrors();

    /// @returns a struct storing the current format errors states
    const formatError getFormatErrors() const;

    /// @return true if error
    bool isKeyFormatError() const;
    /// @return true if error
    bool isSectionFormatError() const;
    void clearKeyError();
    void clearSectionError();

    /// @brief creates a copy of the current file with a Error suffix on the name (Writes a log error)
    /// @note Does not edit the loaded data
    /// @warning does not save any data that was changed
    void createCopy_error();

    /// @brief the entire map holding all the loaded data data is stored as follows std::map<"SectionName", std::map<"KeyName", "KeyValue">>
    /// @warning DO NOT EDIT GIVEN DATA
    const std::map<std::string, std::map<std::string, std::string>>& getLoadedData() const;
    /// @brief the map holding data from the section inputted which is stored as std::map<"KeyName", "KeyValue">
    /// @returns the map holding data for the given section if it exists, otherwise it returns nullptr
    /// @warning DO NO EDIT GIVEN DATA
    const std::map<std::string, std::string>* getSectionData(const std::string& SectionName) const;

    /// @warning if the data was never loaded then it will return "\0\0\0"
    /// @returns the keyValue as a String OR a string containing "\0\0" if there was no keyName found and "\0" for no sectionName found
    std::string getValue(const std::string& SectionName, const std::string& keyName) const;
    /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    /// @warning if the data was never loaded then it will return "\0\0\0"
    /// @returns the keyValue as a String OR a string containing "\0\0" if there was no keyName found and "\0" for no sectionName found
    std::string getValue(const std::pair<std::string, std::string>& Section_Key_Name) const;
    /// @brief sets the given value if possible
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the value was not set meaning that there was either no sectionName of keyName
    bool setValue(const std::string& SectionName, const std::string& keyName, const std::string& keyValue);
    /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    /// @brief sets the given value if possible
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the value was not set meaning that there was either no sectionName of keyName
    bool setValue(const std::pair<std::string, std::string>& Section_Key_Name, const std::string& keyValue);
    /// @brief adds a value to the loadedData and will be added to the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @note adds the SectionName if there is not already one
    /// @returns false if the inputted SectionName and KeyName already exist
    bool addValue(const std::string& SectionName, const std::string& keyName, const std::string& keyValue);
    /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    /// @brief adds a value to the loadedData and will be added to the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @note adds the SectionName if there is not already one
    /// @returns false if the inputted SectionName and KeyName already exist
    bool addValue(const std::pair<std::string, std::string>& Section_Key_Name, const std::string& keyValue);
    /// @brief removes a value from the loadedData and will be removed from the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the value was not found
    bool removeValue(const std::string& SectionName, const std::string& keyName);
    /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    /// @brief removes a value from the loadedData and will be removed from the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the value was not found
    bool removeValue(const std::pair<std::string, std::string>& Section_Key_Name);
    /// @brief adds a section to the loadedData and will be added to the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the inputted SectionName already exists
    bool addSection(const std::string& SectionName);
    /// @brief removes a section and all of its keyValues from the loadedData and will be removed from the ini file at next save
    /// @exception if the data was not loaded then it will always return false and does nothing
    /// @returns false if the inputted SectionName was not found
    bool removeSection(const std::string& SectionName);

    /// @brief AutoSave[Default] = true;
    /// @param AutoSave false then the data will not be saved when closing the file or when destroying this obj
    void setAutoSave(bool AutoSave);
    /// @return false when data is not going to be auto saved
    /// @brief Default is true
    bool isAutoSave() const;

    /// @brief saves the data by rewriting the entire file
    /// @returns false if the data was not able to be saved or the data was never loaded
    bool save();

    /// @brief attempts to create the given file
    static void createFile(const std::filesystem::path& filePath);

private:

    bool m_DataWasLoaded = false;
    formatError m_loadedDataErrors;
    bool m_AutoSave = true;

    std::filesystem::path m_filePath;
    
    std::ifstream m_file;

    /// map<sectionName, map<keyName, value_string>>
    std::map<std::string, std::map<std::string, std::string>> m_loadedData;

};

#endif
