#include "include\Utils\iniParser.h"

const std::map<std::string, std::string> iniParser::InvalidSectionData;

iniParser::iniParser()
{
    this->m_filePath = "NULL";
}

iniParser::iniParser(std::string generic_path)
{
    if (std::filesystem::exists(generic_path))
    {
        this->m_file.open(generic_path, std::ios_base::binary);
        if (this->m_file.is_open())
        {
            this->m_filePath = generic_path;
        }
    }
}

iniParser::~iniParser()
{
    if (this->isOpen())
    {
        this->unloadData();
        this->m_file.close();
    }
}

void iniParser::setFilePath(std::string generic_path)
{
    if (this->isOpen())
    {
        this->unloadData();
        this->m_filePath = "NULL";
        this->m_file.close();
    }
    if (std::filesystem::exists(generic_path))
    {
        this->m_file.open(generic_path, std::ios_base::binary);
        if (this->m_file.is_open())
        {
            this->m_filePath = generic_path;
        }
    }
}

std::string iniParser::getFilePath() const
{ return this->m_filePath.generic_string(); }

bool iniParser::isOpen() const
{ return this->m_file.is_open(); }

bool iniParser::isDataLoaded() const
{ return this->m_DataWasLoaded; }

void iniParser::overrideData()
{ this->m_DataWasLoaded = true; }

bool iniParser::LoadData()
{
    // clearing any old data
    this->unloadData();

    // Current Line
    std::string curLine;
    
    bool gettingSectionData = false;
    std::string currentSectionName = "";
    std::map<std::string, std::string> currentSectionData;

    while (std::getline(this->m_file, curLine))
    {
        StringHelper::trim(curLine);

        // this line is a section name
        if (curLine[0] == '[')
        {
            // if we finished finding section data add it to map before starting next section
            if (gettingSectionData)
            {
                this->m_loadedData.insert({currentSectionName, currentSectionData});
                currentSectionData.clear();
            }
            
            // ignore comment
            auto commentAt = curLine.find_first_of(';');
            if (commentAt != std::string::npos)
            {
                curLine = curLine.substr(0, commentAt);
            }
            // find name of section
            auto closingBracketAt = curLine.find_last_of(']');
            if (closingBracketAt != std::string::npos)
            {
                currentSectionName = curLine.substr(1, closingBracketAt - 1);
                StringHelper::trim(currentSectionName);
            }
            // if no closing bracket then faulty formatting
            else
            {
                // faulty formatted section
                m_loadedDataErrors[0] = formattingError::section;
                continue;
            }

            gettingSectionData = true;
        }
        // this line is a key not a section name
        else if (curLine != "")
        {
            size_t temp = curLine.find('=');
            
            if (temp == std::string::npos) 
            {
                // faulty formatted line
                m_loadedDataErrors[1] = formattingError::key;
                continue;
            }

            // if data was there insert into map
            currentSectionData.insert({curLine.substr(0, temp), curLine.substr(temp+1)});
        }
    }

    // adding the map into the data map if the section that was being made had not been added yet
    if (currentSectionName != "") this->m_loadedData.insert({currentSectionName, currentSectionData});

    if (this->m_loadedData.size() == 0) 
    {
        // no data was loaded
        return false;
    }
    this->m_DataWasLoaded = true;
    return true;
}

bool iniParser::SetData(const std::map<std::string, std::map<std::string, std::string>>& Data)
{
    if (this->isOpen())
    {
        this->m_loadedData = Data;
        this->m_DataWasLoaded = true;
        return true;
    }
    return false;
}

void iniParser::unloadData()
{
    if (this->m_DataWasLoaded && this->m_AutoSave) this->SaveData();
    this->m_DataWasLoaded = false;
    this->m_loadedData.clear();
}

bool iniParser::wasFormattingError() const
{
    return !(m_loadedDataErrors[0] == formattingError::none && m_loadedDataErrors[1] == formattingError::none);
}

void iniParser::clearFormattingErrors()
{
    m_loadedDataErrors[0] = formattingError::none;
    m_loadedDataErrors[1] = formattingError::none;
}

const iniParser::formattingError* iniParser::getFormattingErrors() const
{
    return &m_loadedDataErrors[0];
}

void iniParser::CopyFile_Error()
{
    auto tempPath = this->m_filePath;
    tempPath.replace_filename((tempPath.stem().generic_string() + "Error.ini"));
    long long i = 1;
    while (std::filesystem::exists(tempPath))
    {
        tempPath.replace_filename((this->m_filePath.stem().generic_string() + "Error(" + std::to_string(i) + ").ini"));
        i++;
    }
    std::filesystem::copy_file(this->m_filePath, tempPath);
}

const std::map<std::string, std::map<std::string, std::string>>& iniParser::getLoadedData() const
{ return this->m_loadedData; }

const std::map<std::string, std::string>& iniParser::getSectionData(std::string SectionName) const
{
    const auto& temp = this->m_loadedData.find(SectionName);
    return (temp == this->m_loadedData.end() ? iniParser::InvalidSectionData : temp->second);
}

bool iniParser::isSectionDataValid(const std::map<std::string, std::string>& SectionData) const
{ return (SectionData != iniParser::InvalidSectionData); }

std::string iniParser::getValue(std::string SectionName, std::string keyName) const
{
    if (!this->m_DataWasLoaded) return "\0\0\0";

    StringHelper::trim(SectionName);
    StringHelper::trim(keyName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
        return "\0";

    auto key = section->second.find(keyName);
    if (key == section->second.end())
        return "\0\0";

    return key->second;
}

std::string iniParser::getValue(std::pair<std::string, std::string> Section_Key_Name) const
{ return this->getValue(Section_Key_Name.first, Section_Key_Name.second); }

bool iniParser::setValue(std::string SectionName, std::string keyName, std::string keyValue)
{
    if (!this->m_DataWasLoaded) return false;
    
    StringHelper::trim(SectionName);
    StringHelper::trim(keyName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
        return false;

    auto key = section->second.find(keyName);
    if (key == section->second.end())
        return false;

    key->second = keyValue;
    return true;
}

bool iniParser::setValue(std::pair<std::string, std::string> Section_Key_Name, std::string keyValue)
{ return this->setValue(Section_Key_Name.first, Section_Key_Name.second, keyValue); }

bool iniParser::addValue(std::string SectionName, std::string keyName, std::string keyValue)
{
    if (!this->m_DataWasLoaded) return false;
    
    StringHelper::trim(SectionName);
    StringHelper::trim(keyName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
    {
        this->m_loadedData.insert({SectionName, std::map<std::string, std::string>()});
        this->m_loadedData.find(SectionName)->second.insert({keyName, keyValue});
        return true;
    }

    auto key = section->second.find(keyName);
    if (key == section->second.end())
    {
        section->second.insert({keyName, keyValue});
        return true;
    }

    return false;
}

bool iniParser::addValue(std::pair<std::string, std::string> Section_Key_Name, std::string keyValue)
{ return this->addValue(Section_Key_Name.first, Section_Key_Name.second, keyValue); }

bool iniParser::removeValue(std::string SectionName, std::string keyName)
{
    if (!this->m_DataWasLoaded) return false;
    
    StringHelper::trim(SectionName);
    StringHelper::trim(keyName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
        return false;

    auto key = section->second.find(keyName);
    if (key == section->second.end())
        return false;

    section->second.erase(key);
    return true;
}

bool iniParser::removeValue(std::pair<std::string, std::string> Section_Key_Name)
{ return this->removeValue(Section_Key_Name.first, Section_Key_Name.second); }

bool iniParser::addSection(std::string SectionName)
{
    if (!this->m_DataWasLoaded) return false;
    
    StringHelper::trim(SectionName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
    {
        this->m_loadedData.insert({SectionName, std::map<std::string, std::string>()});
        return true;
    }

    return false;
}

bool iniParser::removeSection(std::string SectionName)
{
    if (!this->m_DataWasLoaded) return false;
    
    StringHelper::trim(SectionName);

    auto section = this->m_loadedData.find(SectionName);
    if (section == this->m_loadedData.end())
        return false;

    this->m_loadedData.erase(section);
    return true;
}

void iniParser::setAutoSave(bool AutoSave)
{ this->m_AutoSave = AutoSave; }

bool iniParser::isAutoSave() const
{ return this->m_AutoSave; }

bool iniParser::SaveData()
{
    if (this->m_DataWasLoaded)
    {
        std::ofstream file(this->m_filePath, std::ios_base::binary);
        if (!this->isOpen()) return false;

        for (auto& Section: this->m_loadedData)
        {
            if (Section != this->m_loadedData.begin().operator*()) file << "\n";
            file << '[' << Section.first << "]\n";
            for (std::map<std::string, std::string>::iterator key = Section.second.begin(); key != Section.second.end(); key++)
            {
                file << key->first << '=' << key->second;
                if (key != (--Section.second.end())) file << "\n";
            }
        }

        this->m_file.clear();
        return true;
    }
    return false;
}

bool iniParser::addValue_ToEnd(std::string SectionName, std::string keyName, std::string keyValue)
{
    bool returnValue = false;

    StringHelper::trim(SectionName);
    StringHelper::trim(keyName);

    if (this->m_DataWasLoaded)
    {
        // checking if section name is already a thing in the loaded data if it is not it gets added
        auto section = this->m_loadedData.find(SectionName);
        if (section == this->m_loadedData.end())
            return false;

        // checking if key name is already a thing in the loaded data if it is not it gets added
        auto key = section->second.find(keyName);
        if (key == section->second.end())
        {
            section->second.insert({keyName, keyValue});
            returnValue = true;
        }
    }
    else returnValue = true;

    if (returnValue == true)
    {
        if (!this->m_file.is_open()) return false;

        // opening and out stream to append to the file
        std::ofstream file(this->m_filePath, std::ios_base::app | std::ios_base::binary);
        if (!this->isOpen()) return false;

        // save directly to end of file
        file << '\n' << keyName << '=' << keyValue;

        return true;
    }

    return false;
}
