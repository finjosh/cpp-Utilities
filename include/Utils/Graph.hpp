#ifndef GRAPH_H
#define GRAPH_H

#pragma once

#include <algorithm>
#include <cmath>
#include <list>
#include <map>

#include "SFML/Graphics.hpp"
#include "External/Spline.hpp"

#include "GraphData.hpp"
#include "StringHelper.hpp"

class Graph : public sf::Drawable
{
private:
    static std::map<std::string, GraphType> m_stringToGraphType;
    static std::map<GraphType, std::string> m_graphTypeToString;

    static size_t m_lastID;

    /// @brief the size of the image when drawn to the main window
    sf::Vector2f m_size = {1000,1000};
    /// @brief the resolution that the original graph is drawn at
    sf::Vector2u m_resolution = {1000,1000};
    sf::Texture m_texture;
    // sf::Sprite _graphImage;
    sf::Color m_backgroundColor;
    sf::Vector2f m_position;
    sf::Angle m_rotation = sf::Angle::Zero;
    sf::Vector2f m_origin;
    float m_charSize = -1;

    std::list<GraphData> m_dataSets;

    sf::Font m_font;

    std::string m_xAxisLabel = "X";
    std::string m_yAxisLabel = "Y";

    sf::Color m_axesColor = sf::Color::White;
    unsigned int m_axesThickness = 10;
    unsigned int m_backgroundLinesThickness = 3;
    sf::Color m_backgroundLinesColor = sf::Color::White;
    unsigned int m_margin = 75;
    sf::Angle m_xTextRotation = sf::degrees(45);
    sf::Angle m_yTextRotation = sf::degrees(45);
    unsigned int m_antialiasingLevel = 0;
    /// @brief min, max
    std::pair<float, float> m_xBounds;
    /// @brief min, max
    std::pair<float, float> m_yBounds;
    /// @brief the number of coord steps for the x and y axis
    sf::Vector2u m_numSteps = sf::Vector2u(10, 10);
    sf::Vector2f m_StepSize;
    size_t m_decimalPrecision = 2;

    /// @brief used to reduce the amount of times that the graph is redrawn
    bool m_wasChanged = true;
    bool m_axesSetup = false;

    /// @brief SFML Function override
    /// @param target sf::RenderTarget
    /// @param states sf::RenderStates
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    /// @brief converts the given data value into a position vector
    sf::Vector2f convertValueToPoint(const sf::Vector2f& dataValue) const;

    /// @brief updates the given spline to work with the given data
    /// @param splineData should be empty
    void updateSpline(std::list<sw::Spline>& splineData, const GraphData& dataset);

    float roundTo(float value, uint8_t precision = 0) const;

protected:
    /// @returns the min value for the x and y separately 
    sf::Vector2f getMinValue(const GraphData& graphData) const;
    /// @returns the max value for the x and y separately
    sf::Vector2f getMaxValue(const GraphData& graphData) const;

public:

    static GraphType strToType(const std::string& str);
    static std::string typeToString(GraphType type);

    /// @brief used to update the graph if anything was changed
    /// @throws if the render texture is not able to resize to the stored resolution
    void Update();

    /// @brief draws the background lines for the graph
    void drawBackgroundLines(sf::RenderTarget& target) const;
    /// @brief draws the axis step lines
    void drawAxisStepIndicators(sf::RenderTarget& target) const;
    /// @brief draws the axis lines for x and y
    /// @returns the vertex array that can be used to draw to a renderer
    void drawAxisLines(sf::RenderTarget& target) const;
    /// @brief draws all the text elements for the graph
    void drawTextElements(sf::RenderTarget& target);

    /// @brief Creates a string representation of a float with 'precision' number of digits after comma
    /// @param d float to convert
    /// @param precision number of digits after comma
    /// @returns String representation
    std::string toString(float d, size_t precision = 2);

    /// @brief Constructor using essential values
    /// @param position top left position of graph
    /// @param size size of the graph (NOT RELATED TO RESOLUTION)
    /// @param margin distance of axis from the borders of the graph
    /// @param font sf::Font reference
    /// @param xLabel label of x-axis
    /// @param yLabel label of y-axis
    /// @param resolution the x and y resolution of the original image (before scaling)
    /// @param antialiasingLevel the level of antialiasing applied to the original image
    /// @note if the size is scaled equally in the x and y then the graph will be stretched
    Graph(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& xLabel = "X", const std::string& yLabel = "Y", 
            const sf::Color& backgroundColor = sf::Color::Black, unsigned int antialiasingLevel = 0, float margin = 75, 
            const sf::Vector2u& resolution = sf::Vector2u(1000, 1000), unsigned int decimalPrecision = 2);

    /// @brief Constructs the graph with default inputs 
    /// @warning you must still set a font
    inline Graph() = default;

    /// @brief does not effect texture
    void setPostion(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    /// @brief does not effect texture
    /// @note rotates the drawn graph
    void setRotation(sf::Angle angle);
    sf::Angle getRotation() const;
    /// @brief does not effect texture
    void setOrigin(const sf::Vector2f& origin);
    sf::Vector2f getOrigin() const;
    /// @brief does not effect texture
    void setSize(const sf::Vector2f& size);
    sf::Vector2f getSize() const;
    void setResolution(const sf::Vector2u& resolution = sf::Vector2u(1000, 1000));
    sf::Vector2u getResolution() const;
    void setXLable(const std::string& xLabel);
    std::string getXLable() const;
    void setYLable(const std::string& yLabel);
    std::string getYLable() const;
    /// @note the antialiasing is only on the original graph texture 
    void setAntialiasingLevel(unsigned int antialiasingLevel = 0);
    unsigned int getAntialiasingLevel() const;
    void setMargin(float margin = 50);
    float getMargin() const;
    void setBackgroundColor(const sf::Color& color);
    sf::Color getBackgroundColor() const;
    /// @note max decimal precision is 6
    void setDecimalPrecision(unsigned int decimalPrecision = 2);
    unsigned int getDecimalPrecision() const;
    void setAxesThickness(unsigned int thickness = 10);
    unsigned int getAxesThickness() const;
    /// @brief the thickness of the the background indicator lines
    void setBackgroundLinesThickness(unsigned int thickness = 3);
    unsigned int getBackgroundLinesThickness() const;
    void setBackgroundLinesColor(sf::Color color = sf::Color::White);
    sf::Color getBackgroundLinesColor() const;
    /// @brief sets the rotation of the x axis text indicators
    /// @note default = 45
    void setXTextRotation(sf::Angle angle);
    /// @returns the rotation of the x axis text indicators
    sf::Angle getXTextRotation() const;
    /// @brief sets the rotation of the y axis text indicators
    /// @note default = 45
    void setYTextRotation(sf::Angle rotation);
    /// @returns the rotation of the y axis text indicators
    sf::Angle getYTextRotation() const;
    /// @brief the size of characters on the graph
    /// @note if the graph has not been made yet returns some negative number
    float getCharSize() const;
    /// @brief sets the size of characters
    /// @note if not set the size will be 35% of the margin
    /// @note if set the char size will NOT change
    void setCharSize(float size);
    void setFont(const sf::Font& font);
    /// @note this includes the background lines color
    void setAxesColor(const sf::Color& color);
    sf::Color getAxesColor() const;
    const sf::Font& getFont() const;

    sf::Texture getTexture_copy() const;
    /// @note this is a reference to the stored texture and should not be stored
    sf::Texture& getTexture();

    /// @brief Function for automatically deducing axis labeling
    /// @note this will update when ever something is changed
    void setupAxes();

    /// @brief Function for setting axis labeling manually
    /// @param xSteps    x number of steps
    /// @param ySteps    y number of steps
    /// @warning this will force the axis to stay constant even if something is changed (to undo call the setupAxis without any inputs)
    void setupAxes(unsigned int xSteps, unsigned int ySteps);

    /// @brief Function for adding a dataset to the plot
    /// @param set the set to be added
    /// @returns the ID of the new data set
    size_t addDataSet(const GraphData& data_set);

    void clearDataSets();
    void removeDataSet(size_t ID);
    /// @param ID the id of the wanted data set
    /// @note dont edit the graph type
    /// @note if you call this the next time update is called the graph will be redrawn
    /// @returns nullptr if there was not dataSet with that ID 
    GraphData* getDataSet(size_t ID);
};

#endif
