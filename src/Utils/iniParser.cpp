#include "Utils/iniParser.hpp"
#include "Utils/StringHelper.hpp"
#include <cassert>

//* iniParser::SectionData defs

std::string* iniParser::SectionData::getValue(const std::string& key)
{
    auto interator = m_data.find(key);
    if (interator == m_data.end())
        return nullptr;

    return &(interator->second);
}

const std::string* iniParser::SectionData::getValue(const std::string& key) const
{
    auto interator = m_data.find(key);
    if (interator == m_data.end())
        return nullptr;

    return &(interator->second);
}

std::string& iniParser::SectionData::operator[](const std::string& key)
{ 
    auto interator = m_data.find(key);
    if (interator == m_data.end())
        return m_data.insert({key, ""}).first->second;

    return interator->second;
}

const std::map<std::string, std::string>& iniParser::SectionData::getData() const
{ return m_data; }

void iniParser::SectionData::insert(const std::string& key, const std::string& value)
{ m_data.insert({key, value}); }

void iniParser::SectionData::setValue(const std::string& key, const std::string& value)
{
    auto iterator = m_data.find(key);
    if (iterator == m_data.end())
        this->insert(key, value);
    else
        iterator->second = value;
}

void iniParser::SectionData::changeValue(const std::string& key, const std::string& value)
{ this->setValue(key, value); }

bool iniParser::SectionData::changeKey(const std::string& originalKey, const std::string& newKey)
{
    auto iterator = m_data.find(originalKey);
    if (iterator == m_data.end())
        return false;
    std::string temp = iterator->second;
    m_data.erase(iterator);
    m_data.insert({newKey, temp});
    return true;
}

bool iniParser::SectionData::remove(const std::string& key)
{
    auto iterator = m_data.find(key);
    if (iterator == m_data.end())
        return false;
    m_data.erase(iterator);
    return true;
}

void iniParser::SectionData::clear()
{ m_data.clear(); }

//* iniParser defs

iniParser::SectionData iniParser::m_invalidSectionData;

iniParser::iniParser()
{
    this->m_filePath = "NULL";
}

iniParser::iniParser(const std::string& generic_path)
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

bool iniParser::setFile(const std::string& generic_path)
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
            return true;
        }
    }
    return false;
}

std::string iniParser::getFilePath() const
{ return this->m_filePath.generic_string(); }

bool iniParser::isOpen() const
{ return this->m_file.is_open(); }

bool iniParser::isDataLoaded() const
{ return this->m_DataWasLoaded; }

void iniParser::overrideData()
{ this->m_DataWasLoaded = true; }

bool iniParser::loadData(bool ignoreDuplicateSections)
{
    // clearing any old data
    this->unloadData();

    // Current Line
    std::string curLine;
    
    bool gettingSectionData = false;
    std::string currentSectionName = "";
    SectionData currentSectionData;

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
                m_loadedDataErrors.section = true;
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
                m_loadedDataErrors.key = true;
                continue;
            }

            // if data was there insert into map
            currentSectionData.insert(curLine.substr(0, temp), curLine.substr(temp+1));
        }
    }

    // adding the map into the data map if the section that was being made had not been added yet
    if (currentSectionName != "") 
    {
        std::string temp = currentSectionName;
        if (!ignoreDuplicateSections)
        {
            int i = 0;
            while (m_loadedData.find(temp) != m_loadedData.end())
            {
                temp = currentSectionName + '(' + std::to_string(++i) + ')';
                m_loadedDataErrors.duplicateSections = true;
            }
        }
        
        this->m_loadedData.emplace(temp, currentSectionData);
    }

    if (this->m_loadedData.size() == 0) 
    {
        // no data was loaded
        return false;
    }
    this->m_DataWasLoaded = true;
    return true;
}

bool iniParser::setData(const std::map<std::string, SectionData>& Data)
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
    if (this->m_DataWasLoaded && this->m_AutoSave) this->save();
    this->m_DataWasLoaded = false;
    this->m_loadedData.clear();
    this->clearFormatErrors();
}

void iniParser::clearFormatErrors()
{
    m_loadedDataErrors.section = false;
    m_loadedDataErrors.key = false;
    m_loadedDataErrors.duplicateSections = false;
}

bool iniParser::isFormatError() const
{
    return m_loadedDataErrors.duplicateSections || m_loadedDataErrors.key ||
           m_loadedDataErrors.section;
}

const iniParser::FormatErrors iniParser::getFormatErrors() const
{
    return m_loadedDataErrors;
}

void iniParser::createCopyError(const std::string path)
{
    auto tempPath = this->m_filePath;
    if (path != "" && std::filesystem::exists(path))
        tempPath.parent_path() = path;

    tempPath.replace_filename((tempPath.stem().generic_string() + "Error.ini"));
    long long i = 1;
    while (std::filesystem::exists(tempPath))
    {
        tempPath.replace_filename((this->m_filePath.stem().generic_string() + "Error(" + std::to_string(i) + ").ini"));
        i++;
    }
    std::filesystem::copy_file(this->m_filePath, tempPath);
}

const std::map<std::string, iniParser::SectionData>& iniParser::getLoadedData() const
{ return this->m_loadedData; }

iniParser::SectionData* iniParser::getSection(const std::string& section)
{
    if (!m_DataWasLoaded)
        return nullptr;

    auto iterator = m_loadedData.find(section);
    if (iterator == m_loadedData.end())
        return nullptr;

    return &(iterator->second);
}

const iniParser::SectionData* iniParser::getSection(const std::string& section) const
{
    if (!m_DataWasLoaded)
        return nullptr;

    auto iterator = m_loadedData.find(section);
    if (iterator == m_loadedData.end())
        return nullptr;

    return &(iterator->second);
}

const std::string* iniParser::getValue(const std::string& section, const std::string& key) const
{
    if (!m_DataWasLoaded)
        return nullptr;

    auto iter = this->getSection(section);
    if (iter == nullptr)
        return nullptr;
    
    return iter->getValue(key);
}

const std::string* iniParser::getValue(const std::pair<std::string, std::string>& sectionKeyPair) const
{
    return getValue(sectionKeyPair.first, sectionKeyPair.second);
}

std::string* iniParser::getValue(const std::string& section, const std::string& key)
{
        if (!m_DataWasLoaded)
        return nullptr;

    auto iter = this->getSection(section);
    if (iter == nullptr)
        return nullptr;
    
    return iter->getValue(key);
}

std::string* iniParser::getValue(const std::pair<std::string, std::string>& sectionKeyPair)
{
    return getValue(sectionKeyPair.first, sectionKeyPair.second);
}

iniParser::SectionData& iniParser::operator[](const std::string& section)
{ 
    if (!m_DataWasLoaded)
    {
        assert(m_DataWasLoaded && "Data must be loaded to get valid SectionData");
        m_invalidSectionData.clear(); // making sure it is empty
        return m_invalidSectionData;
    }

    auto iterator = m_loadedData.find(section);
    if (iterator == m_loadedData.end())
        return m_loadedData.insert({section, SectionData{}}).first->second;

    return iterator->second;
}

bool iniParser::addSection(const std::string& section)
{
    if (!m_DataWasLoaded)
        return false;

    auto iter = m_loadedData.find(section);
    if (iter == m_loadedData.end())
    {
        m_loadedData.insert({section, SectionData{}});
        return true; // section was added
    }

    return false; // section already exists
}

void iniParser::setSection(const std::string& section, const SectionData& sectionData)
{
    if (!m_DataWasLoaded)
        return;

    auto sectionIter = m_loadedData.find(section);
    if (sectionIter == m_loadedData.end())
        m_loadedData.insert({section, sectionData});
    else
        sectionIter->second = sectionData;
}

void iniParser::setValue(const std::string& section, const std::string& key, const std::string& value)
{
    if (!m_DataWasLoaded)
        return;

    auto sectionIter = m_loadedData.find(section);
    if (sectionIter == m_loadedData.end())
        sectionIter = m_loadedData.insert({section, SectionData{}}).first;
    
    sectionIter->second.setValue(key, value);
}

void iniParser::setValue(const std::pair<std::string, std::string>& sectionKeyPair, const std::string& value)
{
    if (!m_DataWasLoaded)
        return;

    auto sectionIter = m_loadedData.find(sectionKeyPair.first);
    if (sectionIter == m_loadedData.end())
        sectionIter = m_loadedData.insert({sectionKeyPair.first, SectionData{}}).first;
    
    sectionIter->second.setValue(sectionKeyPair.second, value);
}

bool iniParser::removeSection(const std::string& section)
{
    if (!m_DataWasLoaded)
        return false;

    auto iter = m_loadedData.find(section);
    if (iter == m_loadedData.end())
        return false;

    m_loadedData.erase(iter);
    return true;
}

bool iniParser::removeValue(const std::string& section, const std::string& key)
{
    if (!m_DataWasLoaded)
        return false;

    auto sectionIter = this->getSection(section);
    if (sectionIter == nullptr)
        return false;

    return sectionIter->remove(key);
}

bool iniParser::removeValue(const std::pair<std::string, std::string>& sectionKeyPair)
{
    if (!m_DataWasLoaded)
        return false;

    auto sectionIter = this->getSection(sectionKeyPair.first);
    if (sectionIter == nullptr)
        return false;

    return sectionIter->remove(sectionKeyPair.second);
}

void iniParser::setAutoSave(bool AutoSave)
{ this->m_AutoSave = AutoSave; }

bool iniParser::isAutoSave() const
{ return this->m_AutoSave; }

bool iniParser::save()
{
    if (this->m_DataWasLoaded && this->isOpen())
    {
        std::ofstream file(this->m_filePath, std::ios_base::binary);
        if (!file.is_open()) return false;

        for (auto section = m_loadedData.begin(); section != m_loadedData.end(); section++)
        {
            if (section != this->m_loadedData.begin()) file << "\n";
            file << '[' << section->first << "]\n";
            for (std::map<std::string, std::string>::const_iterator key = section->second.getData().begin(); key != section->second.getData().end(); key++)
            {
                file << key->first << '=' << key->second;
                if (key != (--section->second.getData().end())) file << "\n";
            }
        }

        this->m_file.clear();
        return true;
    }
    return false;
}

void iniParser::createFile(const std::filesystem::path& filePath)
{
    if (std::filesystem::exists(filePath))
        return;

    if (filePath.has_parent_path())
        std::filesystem::create_directories(filePath.parent_path());

    std::ofstream(filePath, std::ios_base::binary);
}
