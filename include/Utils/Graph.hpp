#ifndef GRAPH_H
#define GRAPH_H

#pragma once

#include <algorithm>
#include <cmath>

#include "SFML/Graphics.hpp"
#include "External/Spline.hpp"

#include "GraphData.hpp"
#include "StringHelper.hpp"

class GraphData;

// TODO add a default constructor
class Graph : public sf::Drawable
{
private:
    //* obj data

    /**
     * @brief the size of the image when drawn to the main window
    */
    sf::Vector2f _size;
    /**
     * @brief the resolution that the original graph is drawn at
    */
    sf::Vector2u _resolution;
    sf::Texture _graphTexture;
    sf::Sprite _graphImage;
    sf::Color _backgroundColor;

    // ---------

    //* other data

    std::vector<GraphData> _dataSets;
    std::vector<std::pair<GraphType, sw::Spline>> _graphedDataLines;

    sf::Font _font;

    std::string _xAxisLabel;
    std::string _yAxisLabel;

    sf::Color _axesColor;
    unsigned int _axesThickness = 10;
    unsigned int _backgroundLinesThickness = 3;
    unsigned int _lineThickness = 10;
    unsigned int _barThickness = 10;
    unsigned int _dotSize = 10;
    unsigned int _margin = 75;
    unsigned int _antialiasingLevel = 0;
    // @brief min, max
    std::pair<float, float> _xBounds;
    // @brief min, max
    std::pair<float, float> _yBounds;
    // @brief the number of coord steps for the x and y axis
    sf::Vector2u _numSteps = sf::Vector2u(10, 10);
    sf::Vector2f _StepSize;
    size_t _decimalPrecision = 2;

    sf::VertexArray _axesVertexArray;
    sf::VertexArray _axesStepIndicators;
    sf::VertexArray _backgroundGraphLines;

    // all text elements
    std::vector<sf::Text> _textElements;

    // @brief used to reduce the amount of times that the graph is redrawn
    bool _wasChanged = true;

    /**
     * SFML Function override
     * @param target sf::RenderTarget
     * @param states sf::RenderStates
     */
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    /**
     * Generate SFML VertexArrays from Datasets
     */
    void GenerateVertices();

    /**
     * Clear SFML VertexArrays
     */
    void ClearVertices();

    /**
     * @brief converts the given data value into a position vector
    */
    sf::Vector2f ConvertValueToPoint(sf::Vector2f dataValue);

public:

    /**
     * @brief used to update the graph if anything was changed
    */
    void Update();

    /**
     * Creates a string representation of a float with 'precision' number of digits after comma
     * @param  d         float to convert
     * @param  precision number of digits after comma
     * @return           String representation
     */
    std::string ToString(const float& d, const size_t& precision = 2);

    /**
     * Constructor using essential values
     * @param position          top left position of graph
     * @param size              size of the graph (NOT RELATED TO RESOLUTION)
     * @param margin            distance of axes from the borders of the graph
     * @param font              sf::Font reference
     * @param xLabel            label of x-axis
     * @param yLabel            label of y-axis
     * @param resolution        the x and y resolution of the original image (before scaling)
     * @param antialiasingLevel the level of antialiasing applied to the original image
     * @note                    if the size is scaled equally in the x and y then the graph will be stretched
     */
    Graph(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& xLabel, const std::string& yLabel, 
            const sf::Color& backgroundColor = sf::Color::Black, const unsigned int& antialiasingLevel = 0, const float& margin = 75, 
            const sf::Vector2u& resolution = sf::Vector2u(1000, 1000), const unsigned int& decimalPrecision = 2);

    void setPostion(const sf::Vector2f& pos);
    inline sf::Vector2f getPosition() const { return _graphImage.getPosition(); }
    void setRotation(const float& degree);
    inline float getRotation() const { return _graphImage.getRotation(); }
    /**
     * @brief origin according to the size
    */
    void setOrigin(const sf::Vector2f origin);
    inline sf::Vector2f getOrigin() { return _graphImage.getOrigin(); }
    void setSize(const sf::Vector2f& size);
    inline sf::Vector2f getSize() const { return _size; }
    void setResolution(const sf::Vector2u& resolution = sf::Vector2u(1000, 1000));
    inline sf::Vector2u getResolution() const { return sf::Vector2u(this->_resolution); }
    void setXLable(const std::string& xLabel);
    inline std::string getXLable() const { return this->_xAxisLabel; }
    void setYLable(const std::string& yLabel);
    inline std::string getYLable() const { return this->_yAxisLabel; }
    /**
     * @note the antialiasing is only on the original graph texture 
    */
    void setAntialiasingLevel(const unsigned int& antialiasingLevel = 0);
    inline unsigned int getAntialiasingLevel() const { return this->_antialiasingLevel; }
    void setMargin(const float& margin = 50);
    inline float getMargin() const { return this->_margin; } 
    void setFont(const sf::Font& font);
    void setBackgroundColor(const sf::Color& color);
    inline sf::Color getBackgroundColor() { return this->_backgroundColor; };
    void setDecimalPrecision(const unsigned int& decimalPrecision = 2);
    inline unsigned int getDecimalPrecision() { return _decimalPrecision; }
    void setAxesThickness(const unsigned int& thickness = 10);
    inline unsigned int getAxesThickness() { return _axesThickness; }
    /**
     * @brief the thickness of the the background indicator lines
    */
    void setBackgroundLinesThickness(const unsigned int& thickness = 3);
    inline unsigned int getBackgroundLinesThickness() { return _backgroundLinesThickness; };
    /**
     * @brief the thickness of the lines used for line graphs
    */
    void setLineThickness(const unsigned int& thickness = 5);
    inline unsigned int getLineThickness() { return _lineThickness; }
    /**
     * @brief the size of the dots use on the scatter plot
     * @param size dot size used for scatter plot
     * @warning size is not in pixels
    */
    void setScatterPlotSize(const unsigned int& size = 1);
    inline unsigned int getScatterPlotSize() { return _dotSize; };
    /**
     * @brief the thickness of bar graphs bars
    */
    void setBarThickness(const unsigned int& thickness = 10);
    inline unsigned int getBarThickness() { return _barThickness; }

    inline sf::Texture getGraphTexture_copy() { return this->_graphTexture; };

    /**
     * Function for automatically deducing axes labeling
     */
    void SetupAxes();

    /**
     * Function for setting axes labeling manually
     * @param xSteps    x number of steps
     * @param ySteps    y number of steps
     * @param axesColor color of axes
     */
    void SetupAxes(const sf::Color& axesColor, const unsigned int& xSteps = 10, const unsigned int& ySteps = 10);

    /**
     * Function for adding a dataset to the plot
     * @param set the set to be added
     * @warning call "SetupAxes" if needed as they are not automatically updated
     */
    void AddDataSet(const GraphData& data_set);

    void ClearDataSets();
    void RemoveDataSet(const size_t& ID);
    /**
     * @param ID the id of the wanted data set
     * @returns nullptr if there was not dataSet with that ID
    */ 
    GraphData* getDataSet(const size_t& ID);
};

#endif
