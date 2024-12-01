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

    class SectionData
    {
    public:
        /// @exception if the key was not found then returns nullptr
        /// @returns pointer to the value from the given key
        const std::string* getValue(const std::string& key) const;
        /// @exception if the key was not found then returns nullptr
        /// @returns pointer to the value from the given key
        std::string* getValue(const std::string& key);
        /// @brief tries to find the given key and return the value
        /// @note if the key is not found it will be created with an empty string
        /// @note if you need a getter without making the key value pair if they dont exist use "getValue"
        std::string& operator[](const std::string& key);
        const std::map<std::string, std::string>& getData() const;

        /// @brief inserts the <key, value> pair to the data in this section
        void insert(const std::string& key, const std::string& value);
        /// @note if the key does not exists it will be created
        void setValue(const std::string& key, const std::string& value);
        /// @note same as "setValue"
        void changeValue(const std::string& key, const std::string& value);
        /// @returns if the original key was found and replaced or not
        bool changeKey(const std::string& originalKey, const std::string& newKey);
        /// @brief removes the key if in this section
        /// @returns false if the key was not found
        bool remove(const std::string& key);

    protected:
        friend iniParser;

        /// @brief removes all data from this section
        void clear();

    private:
        /// @brief map<keyName, valueString>
        std::map<std::string, std::string> m_data = {};
    };

    struct FormatErrors
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
    bool setData(const std::map<std::string, SectionData>& Data);

    /// @brief unloads the data that is stored
    /// @warning will save the data if autosave is on
    /// @note does not close the file
    void unloadData();

    /// @returns a struct storing the current format errors states
    const FormatErrors getFormatErrors() const;
    /// @note if there was no data in the file then there was no format error
    /// @returns true for a format error
    bool isFormatError() const;
    /// @return true if error
    bool isKeyFormatError() const;
    /// @return true if error
    bool isSectionFormatError() const;
    void clearKeyError();
    void clearSectionError();
    /// @brief erases the errors from loading data
    void clearFormatErrors();

    /// @brief creates a copy of the current file with a Error suffix on the name (Writes a log error)
    /// @note Does not edit the loaded data
    /// @param path the path to add the new file to (if empty then adds to the original files path)
    /// @warning does not save any data that was changed only copies the file that is open
    void createCopyError(const std::string path = "");

    /// @brief the entire map holding all the loaded data data is stored as follows std::map<"SectionName", std::map<"KeyName", "KeyValue">>
    /// @warning DO NOT EDIT GIVEN DATA
    const std::map<std::string, SectionData>& getLoadedData() const;

    /// @exception if the data is not loaded then returns nullptr
    /// @exception if the section does not exist returns nullptr
    /// @returns the pointer to the SectionData with the given name
    SectionData* getSection(const std::string& section);
    /// @note same as "getSection"
    /// @exception if the data is not loaded then returns nullptr
    /// @exception if the section does not exist returns nullptr
    /// @returns the pointer to the SectionData with the given name
    const SectionData* getSection(const std::string& section) const;
    /// @exception if the data is not loaded then returns nullptr
    /// @exception if the section or key does not exist returns nullptr
    /// @returns the pointer to the value with the given section and key
    const std::string* getValue(const std::string& section, const std::string& key) const;
    /// @exception if the data is not loaded then returns nullptr
    /// @exception if the section or key does not exist returns nullptr
    /// @returns the pointer to the value with the given section and key
    const std::string* getValue(const std::pair<std::string, std::string>& sectionKeyPair) const;
    /// @note same as "getSection"
    /// @exception if the data is not loaded then returns nullptr
    /// @exception if the section does not exist returns nullptr
    /// @returns the pointer to the SectionData with the given name
    const SectionData* operator[](const std::string& section) const;
    /// @exception if data is not loaded then an empty SectionData will be returned (also asserts)
    /// @note if the section does not exist then it will be made
    /// @note if you need a getter without making the SectionData if it does not exist use "getSection"
    SectionData& operator[](const std::string& section);

    /// @brief adds the given section if it does not already exist
    /// @returns true if the section was created, false if the section already exists
    bool addSection(const std::string& section);
    /// @note if the section does not exist it will be created
    /// @exception if the data is not loaded then nothing happens
    void setSection(const std::string& section, const SectionData& sectionData);
    /// @note if the key or section does not exist it will be created
    /// @exception if the data is not loaded then nothing happens
    void setValue(const std::string& section, const std::string& key, const std::string& value);
    /// @note if the key or section does not exist it will be created
    /// @exception if the data is not loaded then nothing happens
    void setValue(const std::pair<std::string, std::string>& sectionKeyPair, const std::string& value);
    /// @returns true if the section was found and removed 
    bool removeSection(const std::string& section);
    /// @returns true if the key value pair was found and removed
    bool removeValue(const std::string& section, const std::string& key);
    /// @returns true if the key value pair was found and removed
    bool removeValue(const std::pair<std::string, std::string>& sectionKeyPair);

    // /// @deprecated
    // /// @warning if the data was never loaded then it will return "\0\0\0"
    // /// @returns the keyValue as a String OR a string containing "\0\0" if there was no keyName found and "\0" for no sectionName found
    // [[deprecated]] std::string getValue(const std::string& SectionName, const std::string& keyName) const;
    // /// @deprecated
    // /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    // /// @warning if the data was never loaded then it will return "\0\0\0"
    // /// @returns the keyValue as a String OR a string containing "\0\0" if there was no keyName found and "\0" for no sectionName found
    // [[deprecated]] std::string getValue(const std::pair<std::string, std::string>& Section_Key_Name) const;
    // /// @deprecated
    // /// @brief sets the given value if possible
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the value was not set meaning that there was either no sectionName of keyName
    // [[deprecated]] bool setValue(const std::string& SectionName, const std::string& keyName, const std::string& keyValue);
    // /// @deprecated
    // /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    // /// @brief sets the given value if possible
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the value was not set meaning that there was either no sectionName of keyName
    // [[deprecated]] bool setValue(const std::pair<std::string, std::string>& Section_Key_Name, const std::string& keyValue);
    // /// @deprecated
    // /// @brief adds a value to the loadedData and will be added to the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @note adds the SectionName if there is not already one
    // /// @returns false if the inputted SectionName and KeyName already exist
    // [[deprecated]] bool addValue(const std::string& SectionName, const std::string& keyName, const std::string& keyValue);
    // /// @deprecated
    // /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    // /// @brief adds a value to the loadedData and will be added to the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @note adds the SectionName if there is not already one
    // /// @returns false if the inputted SectionName and KeyName already exist
    // [[deprecated]] bool addValue(const std::pair<std::string, std::string>& Section_Key_Name, const std::string& keyValue);
    // /// @deprecated
    // /// @brief removes a value from the loadedData and will be removed from the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the value was not found
    // [[deprecated]] bool removeValue(const std::string& SectionName, const std::string& keyName);
    // /// @deprecated
    // /// @param Section_Key_Name the pair is in order of SectionName then KeyName
    // /// @brief removes a value from the loadedData and will be removed from the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the value was not found
    // [[deprecated]] bool removeValue(const std::pair<std::string, std::string>& Section_Key_Name);
    // /// @deprecated
    // /// @brief adds a section to the loadedData and will be added to the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the inputted SectionName already exists
    // [[deprecated]] bool addSection(const std::string& SectionName);
    // /// @deprecated
    // /// @brief removes a section and all of its keyValues from the loadedData and will be removed from the ini file at next save
    // /// @exception if the data was not loaded then it will always return false and does nothing
    // /// @returns false if the inputted SectionName was not found
    // [[deprecated]] bool removeSection(const std::string& SectionName);

    /// @brief AutoSave[Default] = true;
    /// @param AutoSave false then the data will not be saved when closing the file or when destroying this obj
    void setAutoSave(bool AutoSave);
    /// @return false when data is not going to be auto saved
    /// @brief Default is true
    bool isAutoSave() const;

    /// @brief saves the data by rewriting the entire file
    /// @returns false if the data was not able to be saved or the data was never loaded
    bool save();

    /// @brief attempts to create the given file and its directories if it does not already exit
    static void createFile(const std::filesystem::path& filePath);

private:

    bool m_DataWasLoaded = false;
    FormatErrors m_loadedDataErrors;
    bool m_AutoSave = true;

    std::filesystem::path m_filePath;
    
    std::ifstream m_file;

    /// map<sectionName, SectionData>
    std::map<std::string, SectionData> m_loadedData;
    static SectionData m_invalidSectionData;
};

#endif
