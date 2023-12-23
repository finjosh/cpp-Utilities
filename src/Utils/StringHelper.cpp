#include "include\Utils\StringHelper.hpp"

const char *const StringHelper::whitespaceDelimiters = " \t\n\r\f\v";

void StringHelper::trim(std::string& string)
{
    string.erase(string.find_last_not_of(whitespaceDelimiters) + 1);
	string.erase(0, string.find_first_not_of(whitespaceDelimiters));
}

std::string StringHelper::trim_copy(std::string string)
{ StringHelper::trim(string); return string; }

void StringHelper::toLower(std::string& string)
{ std::transform(string.begin(), string.end(), string.begin(), [](const char c) { return static_cast<char>(std::tolower(c)); }); }

std::string StringHelper::toLower_copy(std::string string)
{ StringHelper::toLower(string); return string; }

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

int StringHelper::toInt(const std::string& string, const int& defaultValue)
{
    int result;
    if (attemptToInt(string, result))
        return result;
    else
        return defaultValue;
}

unsigned int StringHelper::toUInt(const std::string& string, const unsigned int& defaultValue)
{
    unsigned int result;
    if (attemptToUInt(string, result))
        return result;
    else
        return defaultValue;
}

unsigned long StringHelper::toULong(const std::string& string, const int& defaultValue)
{
    unsigned long result;
    if (attemptToULong(string, result))
        return result;
    else
        return defaultValue;
}

float StringHelper::toFloat(const std::string& string, const float& defaultValue)
{
    float result;
    if (attemptToFloat(string, result))
        return result;
    else
        return defaultValue;
}

long double StringHelper::toLongDouble(const std::string& string, const long double& defaultValue)
{
    long double result;
    if (attemptToLongDouble(string, result))
        return result;
    else 
        return defaultValue;
}

bool StringHelper::toBool(const std::string& string, const bool& defaultValue)
{
    bool result;
    if (attemptToBool(string, result))
        return result;
    else
        return defaultValue;
}

std::string StringHelper::FloatToStringRound(float value, unsigned int decimalRoundingPlace)
{ 
    std::string temp = std::to_string(value); 
    return temp.substr(0, temp.length() - (decimalRoundingPlace == 0 ? 7 : 6 - decimalRoundingPlace));
}
