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

std::pair<std::string*, bool> iniParser::SectionData::changeKey(const std::string& originalKey, const std::string& newKey)
{
    auto originalIter = m_data.find(originalKey);
    auto newIter = m_data.find(newKey);
    if (originalIter == m_data.end())
        return {nullptr, false};
    else if (newIter != m_data.end())
        return {&originalIter->second, false};

    std::string temp = originalIter->second;
    m_data.erase(originalIter);
    auto iter = m_data.emplace(newKey, temp);
    return {&iter.first->second, true};
}

bool iniParser::SectionData::remove(const std::string& key)
{
    auto iterator = m_data.find(key);
    if (iterator == m_data.end())
        return false;
    m_data.erase(iterator);
    return true;
}

//* iniParser defs

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
    this->closeFile();
    this->clearData();
}

bool iniParser::setFile(const std::string& generic_path, bool ignoreAutosave)
{
    this->closeFile(ignoreAutosave);
    this->m_filePath = "";

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

void iniParser::setData(const std::map<std::string, SectionData>& Data)
{
    this->m_data = Data;
}

void iniParser::closeFile(bool ignoreAutosave)
{
    if (!ignoreAutosave && m_autosave && this->isOpen())
    {
        this->save();
    }
    m_file.close();
}

void iniParser::clearData()
{
    m_data.clear();
    this->clearFormatErrors();
}

bool iniParser::parseData(bool ignoreDuplicateSections)
{
    // clearing any old data
    this->clearData();

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
                this->m_data.insert({currentSectionName, currentSectionData});
                currentSectionData.removeAll();
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
                m_formatErrors.section = true;
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
                m_formatErrors.key = true;
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
            while (m_data.find(temp) != m_data.end())
            {
                temp = currentSectionName + '(' + std::to_string(++i) + ')';
                m_formatErrors.duplicateSections = true;
            }
        }
        
        this->m_data.emplace(temp, currentSectionData);
    }

    if (this->m_data.size() == 0) 
    {
        // no data was loaded
        return false;
    }
    return true;
}

void iniParser::clearFormatErrors()
{
    m_formatErrors.section = false;
    m_formatErrors.key = false;
    m_formatErrors.duplicateSections = false;
}

bool iniParser::hasFormatError() const
{
    return m_formatErrors.duplicateSections || m_formatErrors.key ||
           m_formatErrors.section;
}

const iniParser::FormatErrors iniParser::getFormatErrors() const
{
    return m_formatErrors;
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

iniParser::SectionData* iniParser::getSection(const std::string& section)
{
    auto iterator = m_data.find(section);
    if (iterator == m_data.end())
        return nullptr;

    return &(iterator->second);
}

const iniParser::SectionData* iniParser::getSection(const std::string& section) const
{
    auto iterator = m_data.find(section);
    if (iterator == m_data.end())
        return nullptr;

    return &(iterator->second);
}

iniParser::SectionData& iniParser::operator[](const std::string& section)
{ 
    auto iterator = m_data.find(section);
    if (iterator == m_data.end())
        return m_data.insert({section, SectionData{}}).first->second;

    return iterator->second;
}

bool iniParser::removeSection(const std::string& section)
{
    auto iter = m_data.find(section);
    if (iter == m_data.end())
        return false;

    m_data.erase(iter);
    return true;
}

void iniParser::setAutosave(bool Autosave)
{ this->m_autosave = Autosave; }

bool iniParser::isAutosave() const
{ return this->m_autosave; }

bool iniParser::save()
{
    if (this->isOpen() && m_data.size() > 0)
    {
        std::ofstream file(this->m_filePath, std::ios_base::binary);
        if (!file.is_open()) return false;

        for (auto section = m_data.begin(); section != m_data.end(); section++)
        {
            if (section != this->m_data.begin()) file << "\n";
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
