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
enum class GraphType //! add the case in graph.cpp for drawing the data sets when adding a new graph type
{
    Histogram = 0,
    Bar = 1,
    Scatter = 3,
    Line = 2
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

    float _thickness = 10;

public:

    GraphData();

    /// @param color dataset color
    /// @param label dataset label
    /// @param type dataset type
    GraphData(const sf::Color& color, const std::string& label, const GraphType& type);

    /// @param xValues vector of floats, representing x axis
    /// @param yValues vector of floats, representing y axis
    /// @param color color of the bars/points/line
    /// @param label data label
    /// @param type  bars/points/line
    GraphData(const std::vector<float>& xValues, const std::vector<float>& yValues, const sf::Color& color, const std::string& label, const GraphType& type);

    /// @brief data is invalid when xValues.size() != yValues.size()
    /// @return false when data is invalid
    bool isValid() const;

    /// @brief the ID that is used to access this class later
    size_t getID() const;

    /// @return _xValues
    const std::vector<float>& getXValues() const;
    
    void setXValues(const std::vector<float>& xValues);

    /// @return _yValues
    const std::vector<float>& getYValues() const;

    void setYValues(const std::vector<float>& yValues);

    /// @brief Get element count (xValues.size() == yValues.size())
    /// @exception if this not valid then the x and y values are not the same size
    /// @return xValues.size()
    size_t getDataLength() const;

    /// @brief Get a specific value
    /// @param i index
    /// @return value pair at i
    sf::Vector2f getDataValue(const size_t& i) const;

    /// @brief Setter for a certain value, by index
    /// @param i index
    /// @param valuePair new x and y values
    void setDataValue(const size_t& i, const sf::Vector2f& valuePair);

    /// @returns color of this data when drawn on a graph
    sf::Color getColor() const;

    void setColor(const sf::Color& color);

    const std::string& getLabel() const;

    void setLabel(const std::string& label);

    GraphType getGraphType() const;

    void setGraphType(const GraphType& type);

    /// @brief Function for adding a pair of values to the dataset
    /// @param pair the pair to be added
    void push_back(const sf::Vector2f& pair);

    /// @brief Function for erasing a pair of values to the dataset
    /// @param number of elements to erase
    void popFront(const size_t n = 1);

    /// @brief Clears all the x and y values stored
    void clearPairValues();

    /// @note thickness only affects scatter, bar, and line
    /// @note default is 10
    void setThickness(const float& value);

    float getThickness() const;
};

#endif
