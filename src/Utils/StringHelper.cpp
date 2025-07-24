#include "Utils/StringHelper.hpp"
#include <cctype>
#include <charconv>
#include <algorithm>
#include <iomanip>

const char *const StringHelper::whitespaceDelimiters = " \t\n\r\f\v";

std::string& StringHelper::trim(std::string& string)
{
    string.erase(string.find_last_not_of(whitespaceDelimiters) + 1);
	string.erase(0, string.find_first_not_of(whitespaceDelimiters));
    return string;
}

std::string StringHelper::trim_copy(std::string string)
{ StringHelper::trim(string); return string; }

void StringHelper::toLower(std::string& string)
{ std::transform(string.begin(), string.end(), string.begin(), [](const char c) { return static_cast<char>(std::tolower(c)); }); }

std::string StringHelper::toLower_copy(std::string string)
{ StringHelper::toLower(string); return string; }

void StringHelper::toUpper(std::string& string)
{
    std::transform(string.begin(), string.end(), string.begin(), [](const char c) { return static_cast<char>(std::toupper(c)); });
}

std::string StringHelper::toUpper_copy(std::string string)
{
    StringHelper::toUpper(string);
    return string;
}

bool StringHelper::attemptToInt(std::string string, int& result)
{
    StringHelper::trim(string);
    return std::from_chars(string.data(), string.data() + string.length(), result).ec == std::errc{};
}
bool StringHelper::attemptToUInt(std::string string, unsigned int& result)
{
    StringHelper::trim(string);
    return std::from_chars(string.data(), string.data() + string.length(), result).ec == std::errc{};
}
bool StringHelper::attemptToULong(std::string string, unsigned long& result)
{
    StringHelper::trim(string);
    return std::from_chars(string.data(), string.data() + string.length(), result).ec == std::errc{};
}

bool StringHelper::attemptToFloat(std::string string, float& result)
{
    StringHelper::trim(string);
    return std::from_chars(string.data(), string.data() + string.length(), result).ec == std::errc{};
}

bool StringHelper::attemptToLongDouble(std::string string, long double& result)
{ 
    StringHelper::trim(string);
    if (std::from_chars(string.data(), string.data() + string.length(), result).ec == std::errc{})
    { result = std::stold(string); return true; }
    return false;
}

bool StringHelper::attemptToBool(std::string string, bool& result)
{
    StringHelper::trim(string);
    StringHelper::toLower(string);
    if (string == "true" || string == "1") 
    {
        result = true;
        return true;
    }
    else if (string == "false" || string == "0")
    {
        result = false;
        return true;
    }
    return false;
}

int StringHelper::toInt(std::string string, int defaultValue)
{
    int result;
    if (attemptToInt(std::move(string), result))
        return result;
    else
        return defaultValue;
}

unsigned int StringHelper::toUInt(std::string string, unsigned int defaultValue)
{
    unsigned int result;
    if (attemptToUInt(std::move(string), result))
        return result;
    else
        return defaultValue;
}

unsigned long StringHelper::toULong(std::string string, int defaultValue)
{
    unsigned long result;
    if (attemptToULong(std::move(string), result))
        return result;
    else
        return defaultValue;
}

float StringHelper::toFloat(std::string string, float defaultValue)
{
    float result;
    if (attemptToFloat(std::move(string), result))
        return result;
    else
        return defaultValue;
}

long double StringHelper::toLongDouble(std::string string, long double defaultValue)
{
    long double result;
    if (attemptToLongDouble(std::move(string), result))
        return result;
    else 
        return defaultValue;
}

bool StringHelper::toBool(std::string string, bool defaultValue)
{
    bool result;
    if (attemptToBool(std::move(string), result))
        return result;
    else
        return defaultValue;
}

std::string StringHelper::FloatToStringRound(float value, unsigned int decimalRoundingPlace)
{ 
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << std::fixed << std::setprecision(static_cast<int>(decimalRoundingPlace));
    oss << value;
    return {oss.str()};
}

uint8_t StringHelper::fromHex(std::string hex)
{
    return (fromHex(hex[0]) << 4 | fromHex(hex[1]));
}

uint8_t StringHelper::fromHex(char hex)
{
    switch (hex)
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'A':
        return 10;
    case 'B':
        return 11;
    case 'C':
        return 12;
    case 'D':
        return 13;
    case 'E':
        return 14;
    case 'F':
        return 15;
    default:
        return 0;
    }
}

std::string StringHelper::intToHex(uint8_t value)
{
    std::string rtn;
    rtn += intToHex_char(value >> 4);
    rtn += intToHex_char(value);
    return rtn;
}

char StringHelper::intToHex_char(uint8_t value)
{
    switch (value & 0b00001111)
    {
    case 0b0000:
        return '0';
    case 0b0001:
        return '1';
    case 0b0010:
        return '2';
    case 0b0011:
        return '3';
    case 0b0100:
        return '4';
    case 0b0101:
        return '5';
    case 0b0110:
        return '6';
    case 0b0111:
        return '7';
    case 0b1000:
        return '8';
    case 0b1001:
        return '9';
    case 0b1010:
        return 'A';
    case 0b1011:
        return 'B';
    case 0b1100:
        return 'C';
    case 0b1101:
        return 'D';
    case 0b1110:
        return 'E';
    case 0b1111:
        return 'F';
    }
    return '0';
}

std::vector<std::string> StringHelper::tokenize(const std::string& str, char delim)
{
    std::vector<std::string> tokens;

    std::size_t start = 0;
    std::size_t end = 0;
    while ((end = str.find(delim, start)) != std::string::npos) {
        tokens.push_back(StringHelper::trim_copy(str.substr(start, end - start)));
        start = end + 1;
    }

    tokens.push_back(StringHelper::trim_copy(str.substr(start)));
    return tokens;
}

std::string StringHelper::fromVector(const std::vector<std::string>& vector, char quotation)
{
    std::string rtn = "[";
    for (auto i: vector)
    {
        rtn += quotation + i + quotation + ',';
    }
    if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
    rtn += ']';
    return rtn;
}

std::vector<std::string> StringHelper::toVector(const std::string& str, char quotation)
{
    std::vector<std::string> rtn;
    size_t curPos = str.find_first_of('[');
    size_t last = str.find_last_not_of(whitespaceDelimiters)+1; // last index of useful chars
    if (curPos != std::string::npos) 
        curPos++;
    else
        curPos = 0;
    
    while (curPos < last)
    {
        size_t nextComma = str.find_first_of(',', curPos);
        size_t start = str.find_first_of(quotation, curPos);
        size_t end = start;
        if (start < nextComma)
        {
            start++;
            end = str.find_first_of(quotation, end+1);

            // we found the ending quote so find nextComma outside of quotes
            nextComma = str.find_first_of(',', end);
            if (end != std::string::npos && nextComma == std::string::npos)
            {
                nextComma = last;
            }
        }
        else
        {
            start = curPos;
            end = nextComma;
        }

        if (end == std::string::npos || nextComma == std::string::npos)
        {
            nextComma = str.find_last_of(']', last-1);
            if (nextComma == std::string::npos) 
                nextComma = last;
            
            end = nextComma;
        }
        start = start > last-1 ? last-1 : start;
        std::string temp = str.substr(start,end-start); // getting string between quotes
        curPos = nextComma + 1;
        trim(temp);
        rtn.emplace_back(temp);
    }
    return rtn;
}

std::list<std::string> StringHelper::toList(const std::string& str, char quotation)
{
    std::list<std::string> rtn;
    size_t curPos = str.find_first_of('[');
    size_t last = str.find_last_not_of(whitespaceDelimiters)+1; // last index of useful chars
    if (curPos != std::string::npos) 
        curPos++;
    else
        curPos = 0;
    
    while (curPos < last)
    {
        size_t nextComma = str.find_first_of(',', curPos);
        size_t start = str.find_first_of(quotation, curPos);
        size_t end = start;
        if (start < nextComma)
        {
            start++;
            end = str.find_first_of(quotation, end+1);

            // we found the ending quote so find nextComma outside of quotes
            nextComma = str.find_first_of(',', end);
            if (end != std::string::npos && nextComma == std::string::npos)
            {
                nextComma = last;
            }
        }
        else
        {
            start = curPos;
            end = nextComma;
        }

        if (end == std::string::npos || nextComma == std::string::npos)
        {
            nextComma = str.find_last_of(']', last-1);
            if (nextComma == std::string::npos) 
                nextComma = last;
            
            end = nextComma;
        }
        start = start > last-1 ? last-1 : start;
        std::string temp = str.substr(start,end-start); // getting string between quotes
        curPos = nextComma + 1;
        trim(temp);
        rtn.emplace_back(temp);
    }
    return rtn;
}

std::string StringHelper::fromList(const std::list<std::string>& list, char quotation)
{
    std::string rtn = "[";
    for (auto i: list)
    {
        rtn += quotation + i + quotation + ',';
    }
    if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
    rtn += ']';
    return rtn;
}
