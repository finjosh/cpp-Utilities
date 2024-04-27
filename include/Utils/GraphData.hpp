#ifndef GRAPHDATA_H
#define GRAPHDATA_H

#pragma once

#include <vector>

#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"

#include "StringHelper.hpp"

/**
 * @note if the graph includes a histogram then the axes will be auto setup for the y axes when drawing
*/
enum class GraphType
{
    Histogram = 0,
    Bar = 1,
    Scatter = 2,
    Line = 3
};

class GraphData
{
protected:
    static size_t lastID;

    size_t _ID;

    GraphType _graphType;

    std::vector<float> _xValues;
    std::vector<float> _yValues;

    sf::Color _color;
    std::string _label;

public:

    GraphData();

    /**
     * @param color dataset color
     * @param label dataset label
     * @param type dataset type
     */
    GraphData(const sf::Color& color, const std::string& label, const GraphType& type);

    /**
     * @param xValues vector of floats, representing x axis
     * @param yValues vector of floats, representing y axis
     * @param color   color of the bars/points/line
     * @param label   data label
     * @param type    bars/points/line
     */
    GraphData(const std::vector<float>& xValues, const std::vector<float>& yValues, const sf::Color& color, const std::string& label, const GraphType& type);

    /**
     * data is invalid when xValues.size() != yValues.size()
     * @return false when data is invalid
     */
    bool isValid() const;

    /// @brief the ID that is used to access this class later
    size_t getID() const;

    /**
     * _xValues Getter
     * @return _xValues
     */
    const std::vector<float>& GetXValues() const;
    
    void setXValues(const std::vector<float>& xValues);

    /**
     * _yValues Getter
     * @return _yValues
     */
    const std::vector<float>& GetYValues() const;

    void setYValues(const std::vector<float>& yValues);

    /**
     * Get element count (xValues.size() == yValues.size())
     * @exception if this not valid then the x and y values are not the same size
     * @return xValues.size()
     */
    size_t GetDataLength() const;

    /**
     * Get a specific value
     * @param i index
     * @return value pair at i
     */
    sf::Vector2f GetDataValue(const size_t& i) const;

    /**
     * Setter for a certain value, by index
     * @param i index
     * @param valuePair new x and y values
     */
    void SetDataValue(const size_t& i, const sf::Vector2f& valuePair);

    /// @returns color of this data when drawn on a graph
    sf::Color GetColor() const;

    void SetColor(const sf::Color& color);

    const std::string& GetLabel() const;

    void SetLabel(const std::string& label);

    GraphType GetGraphType() const;

    void SetGraphType(const GraphType& type);

    /**
     * Function for adding a pair of values to the dataset
     * @param pair the pair to be added
     */
    void PushPair_back(const sf::Vector2f& pair);

    /**
     * Function for erasing a pair of values to the dataset
     * @param number of elements to erase
     */
    void PopFront(const size_t n = 1);

    /**
     * Clears all the x and y values stored
     */
    void ClearPairValues();
};

#endif
