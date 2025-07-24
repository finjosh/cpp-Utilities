#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#pragma once

#include <string>
#include <list>
#include <vector>
#include <stdint.h>
#include <ostream>
#include <sstream>

class StringHelper
{
public:
    
    /// @brief removes the white space from the start and end of the string
    /// @returns a reference to the inputted string
    static std::string& trim(std::string& string);
    /// @brief returns a copy of the given string without any white space in it
    static std::string trim_copy(std::string string);
    static void toLower(std::string& string);
    static std::string toLower_copy(std::string string);
    static void toUpper(std::string& string);
    static std::string toUpper_copy(std::string string);
    /// @brief Converts the string to an integer
    /// @param result Integer value if the string contains a base 10 integer. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been placed into the reference parameter. 
    static bool attemptToInt(std::string string, int& result);
    /// @brief Converts the string to an integer
    /// @param result unsigned Integer value if the string contains a base 10 unsigned integer. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
    static bool attemptToUInt(std::string string, unsigned int& result);
    /// @brief Converts the string to an integer
    /// @param result Integer value if the string contains a base 10 integer. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
    static bool attemptToULong(std::string string, unsigned long& result);
    /// @brief Converts the string to a float
    /// @param result  Float value if the string contains a float. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
    static bool attemptToFloat(std::string string, float& result);
    /// @brief Converts the string to a float
    /// @param result  Long Double value if the string contains a Long Double. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been placed into the reference parameter.
    static bool attemptToLongDouble(std::string string, long double& result);
    /// @brief Converts the string to a bool
    /// @note checks for "true" or "false" or "1" or "0"
    /// @note trims and converts given string to lower case
    /// @param result  Bool Value if the string contains a bool. Unmodified if string is invalid.
    /// @return Returns whether the string was valid and a value has been place into the reference parameter.
    static bool attemptToBool(std::string string, bool& result);
    /// @brief Converts the string to an integer
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
    static int toInt(std::string string, int defaultValue = 0);
    /// @brief Converts the string to an integer
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
    static unsigned int toUInt(std::string string, unsigned int defaultValue = 0); 
    /// @brief Converts the string to an integer
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
   static unsigned long toULong(std::string string, int defaultValue = 0);
    /// @brief Converts the string to a float
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the float value or defaultValue if the string didn't contain a float
    static float toFloat(std::string string, float defaultValue = 0);
    /// @brief Converts the string to a long double
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the long double value or defaultValue if the string didn't contain a long double
    static long double toLongDouble(std::string string, long double defaultValue = 0);
    /// @brief Converts the string to a bool
    /// @param defaultValue  Value to return if conversion fails
    /// @return Returns the bool value or defaultValue if the string didn't contain a bool
    static bool toBool(std::string string, bool defaultValue = 0);
    /// @brief Converts a float to a string while rounding its decimal value
    /// @param value is the float value to be converted
    /// @param decimalRoundingPlace is the number of decimals to round to
    static std::string FloatToStringRound(float value, unsigned int decimalRoundingPlace = 1);
    /// @note if given char is not hex then returns 0
    static uint8_t fromHex(char hex);
    /// @param hex only uses first two chars
    static uint8_t fromHex(std::string hex);
    /// @returns the value as a string containing the two chars that represent it in hex
    static std::string intToHex(uint8_t value);
    /// @returns the hex char representing the first 4 bits in the given int
    static char intToHex_char(uint8_t value);

    // TODO add options to change bracket type and quotation type while parsing from/converting to string
    template <typename T = float, typename ConvertFunc = T(std::string, T), typename std::enable_if_t<!std::is_same_v<std::string, T>>* = nullptr>
    static inline std::list<T> toList(const std::string& str, const ConvertFunc& convFunc = &StringHelper::toFloat, T defaultValue = 0)
    {
        std::list<T> rtn;
        size_t curPos = str.find_first_of('[');
        size_t last = str.find_last_not_of(whitespaceDelimiters)+1; // last index of useful chars
        if (curPos != std::string::npos) 
            curPos++;
        else
            curPos = 0;
        
        while (curPos < last)
        {
            size_t nextComma = str.find_first_of(',', curPos);

            if (nextComma == std::string::npos)
            {
                nextComma = str.find_last_of(']', last-1);
                if (nextComma == std::string::npos) 
                    nextComma = last;
            }
            std::string temp = str.substr(curPos,nextComma-curPos); // getting string between quotes
            curPos = nextComma + 1;
            trim(temp);
            rtn.emplace_back(convFunc(std::move(temp), defaultValue));
        }
        return rtn;
    }

    /// @tparam ConvertFunc takes the type that is stored in the list and returns a string
    template <typename T = float, typename ConvertFunc = std::string(T)>
    static inline std::string fromList(const std::list<T>& list, const ConvertFunc& convFunc)
    {
        std::string rtn = "[";
        for (auto i: list)
        {
            rtn += convFunc(i) + ',';
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    template <typename T = float, typename std::enable_if_t<!std::is_same_v<std::string, T>>* = nullptr>
    static inline std::string fromList(const std::list<T>& list)
    {
        std::string rtn = "[";
        for (auto i: list)
        {
            rtn += std::to_string(i) + ',';
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    template <typename T = float, typename ConvertFunc = T(std::string, T), typename std::enable_if_t<!std::is_same_v<std::string, T>>* = nullptr>
    static inline std::vector<T> toVector(const std::string& str, const ConvertFunc& convFunc = StringHelper::toFloat, T defaultValue = 0)
    {
        std::vector<T> rtn = {};
        size_t curPos = str.find_first_of('[');
        size_t last = str.find_last_not_of(whitespaceDelimiters)+1; // last index of useful chars
        if (curPos != std::string::npos) 
            curPos++;
        else
            curPos = 0;
        
        while (curPos < last)
        {
            size_t nextComma = str.find_first_of(',', curPos);

            if (nextComma == std::string::npos)
            {
                nextComma = str.find_last_of(']', last-1);
                if (nextComma == std::string::npos) 
                    nextComma = last;
            }
            std::string temp = str.substr(curPos,nextComma-curPos); // getting string between quotes
            curPos = nextComma + 1;
            trim(temp);
            rtn.emplace_back(convFunc(std::move(temp), defaultValue));
        }
        return rtn;
    }

    /// @tparam ConvertFunc takes the type that is stored in the list and returns a string
    template <typename T = float, typename ConvertFunc = std::string(T)>
    static inline std::string fromVector(const std::vector<T>& vector, const ConvertFunc& convFunc)
    {
        std::string rtn = "[";
        for (auto i: vector)
        {
            rtn += convFunc(i) + ',';
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    template <typename T = float, typename std::enable_if_t<!std::is_same_v<std::string, T>>* = nullptr>
    static inline std::string fromVector(const std::vector<T>& vector)
    {
        std::string rtn = "[";
        for (auto i: vector)
        {
            rtn += std::to_string(i) + ',';
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    /// @tparam ConvertFunc takes the type that is stored in the list and returns a string
    template <typename Iter, typename T = float, typename ConvertFunc = std::string(T), typename std::enable_if_t<!std::is_same_v<std::string, T>>* = nullptr>
    static inline std::string fromContainer(Iter begin, Iter end, const ConvertFunc& convFunc)
    {
        std::string rtn = "[";
        Iter current = begin;
        while (current != end)
        {
            rtn += convFunc(*current) + ',';
            current++;
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    template <typename Iter>
    static inline std::string fromContainer(Iter begin, Iter end)
    {
        std::string rtn = "[";
        Iter current = begin;
        while (current != end)
        {
            rtn += std::to_string(*current) + ',';
            current++;
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    /// @note adds quotations `"` around the string for each element
    /// @param quotation the type of quotation to use
    static std::string fromVector(const std::vector<std::string>& vector, char quotation = '"');
    /// @note assumes quotations `"` around the string for each element
    /// @param quotation the type of quotation to use
    static std::vector<std::string> toVector(const std::string& str, char quotation = '"');
    /// @note if an element has quotes around it they will be removed and any commas in the quotes will be ignored
    /// @note each element must have quotations around its value or else it will be registered as one value
    /// @note "[]" are optional
    /// @note each element in quotes must be separated via a comma ','
    /// @param quotation the type of quotation to use
    static std::list<std::string> toList(const std::string& str, char quotation = '"');
    /// @note adds quotations `"` around the string for each element
    /// @param quotation the type of quotation to use
    static std::string fromList(const std::list<std::string>& list, char quotation = '"');
    /// @note assumes quotations `"` around the string for each element
    /// @param quotation the type of quotation to use
    template <typename Iter>
    static std::string fromContainerString(Iter begin, Iter end, char quotation = '"')
    {
        std::string rtn = "[";
        Iter current = begin;
        while (current != end)
        {
            rtn += quotation + *current + quotation + ',';
            current++;
        }
        if (rtn.back() == ',') rtn.erase(rtn.size()-1,1);
        rtn += ']';
        return rtn;
    }

    /// @note each token is trimmed
    static std::vector<std::string> tokenize(const std::string& str, char delim);
    template<typename T>
    inline static std::string fromNumber(T value)
    {
        // checking if we can just use to_string
        if constexpr (std::is_integral<T>::value)
        {
            return {std::to_string(value)};
        }
        else
        {
            std::ostringstream oss;
            oss.imbue(std::locale::classic());
            oss << value;
            return {oss.str()};
        }
    }
    
    static const char *const whitespaceDelimiters;
private:
    inline StringHelper() = default;
};

#endif
