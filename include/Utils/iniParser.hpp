#ifndef INIPARSER_H
#define INIPARSER_H

#pragma once

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
        inline SectionData() {}
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
        inline std::map<std::string, std::string>& getData() { return m_data; }
        inline const std::map<std::string, std::string>& getData() const { return m_data; }

        /// @brief inserts the <key, value> pair to the data in this section
        /// @note if the key already exists then it wont be replaced (boolean will be false)
        /// @returns a pair<value str, replaced> where first element is the keys value string and the second element is true if the given value was used
        inline std::pair<std::string&, bool> insert(const std::string& key, const std::string& value) 
        { 
            auto temp = m_data.emplace(key, value);
            return {temp.first->second, temp.second};
        }
        /// @note if the key does not exists it will be created
        /// @returns a reference to the value string
        inline std::string& setValue(const std::string& key, const std::string& value)
        {
            std::string& rtn = m_data[key];
            rtn = value;
            return rtn;
        }
        /// @note if the newKey already exists then it will return a valid pointer and false
        /// @returns a pair<value str, found> where first element is the keys value string (if found else nullptr) and the second element is true if the given key was found and renamed
        std::pair<std::string*, bool> changeKey(const std::string& originalKey, const std::string& newKey);
        /// @brief removes the key if in this section
        /// @returns false if the key was not found
        bool remove(const std::string& key);
        /// @brief removes all data from this section
        inline void removeAll() { m_data.clear(); }

    private:
        /// @brief map<keyName, valueString>
        std::map<std::string, std::string> m_data = {};
    };

    struct FormatErrors
    {
        bool key = false;
        bool section = false;
        bool duplicateSections = false;
    };

    /// @brief basic iniParser without any file or data set
    inline iniParser() = default;
    /// @brief creates the 'iniParser' opening the given file
    /// @note Does not parse the file for data
    iniParser(const std::string& generic_path);
    /// @brief saves the current data before closing the file
    ~iniParser();

    /// @note does not clear the current data
    /// @note does not attempt to parse data from the new file
    /// @note closes the current file via "closeFile(ignoreAutosave)" and attempts to open the new one
    /// @param ignoreAutosave whether autosave should be ignored
    /// @returns true if opened new file successfully
    bool setFile(const std::string& generic_path, bool ignoreAutosave = false);
    /// @note same as "setFile(const std::string& generic_path)"
    inline bool openFile(const std::string& generic_path, bool ignoreAutosave = false) { return this->setFile(generic_path, ignoreAutosave); }
    /// @brief returns a copy of the filepath
    std::string getFilePath() const;
    /// @returns true if this has a file opened
    bool isOpen() const;
    /// @brief if a file is open then saves data to the file (if autosave is on and ignoreAutosave is false) and closes it 
    /// @param ignoreAutosave whether autosave should be ignored
    void closeFile(bool ignoreAutosave = false);
    /// @brief clears all data from the parser without saving no matter the value for autosave
    void clearData();

    /// @brief attempts to parse data from the file that is open
    /// @param ignoreDuplicateSections whether the duplicate section should be kept and renamed or just removed/ignored (this is only for sections not key/value pairs within sections)
    /// @exception any faulty formatted data is erased
    /// @exception if there are two sections with the same name there will be numbers appended to the section name if ignoreDuplicateSections=false e.g. section, section(1), section(2), section(3), ect.
    /// @warning all data will be removed before parsing (no autosave)
    /// @returns if there was any data parsed (could still have format errors)
    bool parseData(bool ignoreDuplicateSections = true);

    /// @brief sets the data that the iniParse is holding
    void setData(const std::map<std::string, SectionData>& Data);

    /// @brief errors entail key errors, section errors, and duplicate sections
    /// @warning data could be lost with key and section errors
    /// @note duplicate sections do not lose data but create an extra section
    /// @returns a struct storing the current format errors states
    const FormatErrors getFormatErrors() const;
    /// @brief erases the errors from parsing data
    void clearFormatErrors();
    /// @returns true if there are any format errors
    bool hasFormatError() const;

    /// @brief creates a copy of the current file with a Error suffix on the name (Writes a log error)
    /// @param path the path to add the new file to (if empty then adds to the original files path)
    /// @note this only copies the file and does not save, parse, or edit any data
    void createCopyError(const std::string path = "");

    /// @brief the data is stored as follows std::map<"SectionName", std::map<"KeyName", "KeyValue">>
    inline std::map<std::string, SectionData>& getData() { return m_data; }
    inline const std::map<std::string, SectionData>& getData() const { return m_data; }

    /// @exception if the section does not exist returns nullptr
    /// @returns the pointer to the SectionData with the given name
    SectionData* getSection(const std::string& section);
    /// @exception if the section does not exist returns nullptr
    /// @returns the pointer to the SectionData with the given name
    const SectionData* getSection(const std::string& section) const;
    /// @note if the section does not exist then it will be made
    /// @note if you need a getter without making the SectionData if it does not exist use "getSection"
    SectionData& operator[](const std::string& section);

    /// @brief adds the given section if it does not already exist
    /// @note if the section already exists it will not be replaced
    /// @returns a pair where the first element is a reference to the section and the second element is whether a new section was made or it already existed
    inline std::pair<iniParser::SectionData&, bool> insertSection(const std::string& section, const SectionData& sectionData = SectionData())
    {
        auto temp = m_data.emplace(section, sectionData);
        return {temp.first->second, temp.second};
    }
    /// @note if the section does not exist it will be created
    inline iniParser::SectionData& setSection(const std::string& section, const SectionData& sectionData)
    {
        iniParser::SectionData& rtn = m_data[section];
        rtn = sectionData;
        return rtn;
    }
    /// @returns true if the section was found and removed 
    bool removeSection(const std::string& section);

    /// @brief Autosave[Default] = true;
    /// @param Autosave false then the data will not be saved when closing the file or when destroying this obj
    void setAutosave(bool Autosave = true);
    /// @return false when data is not going to be auto saved
    /// @brief Default is true
    bool isAutosave() const;

    /// @returns false if the data was not able to be saved or there was no data to save
    bool save();

    /// @brief attempts to create the given file and its directories if it does not already exit
    static void createFile(const std::filesystem::path& filePath);

private:
    FormatErrors m_formatErrors;
    bool m_autosave = true;

    std::filesystem::path m_filePath = "";
    
    std::ifstream m_file;

    /// map<sectionName, SectionData>
    std::map<std::string, SectionData> m_data;
};

#endif
