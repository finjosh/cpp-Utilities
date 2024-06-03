#include "Utils/GraphData.hpp"

GraphData::GraphData()
{
    _color = sf::Color::White;
    _label = "Unlabeled";
    _graphType = GraphType::Scatter;
}

GraphData::GraphData(const sf::Color& color, const std::string& label, GraphType type) : _color(color), _label(label), _graphType(type)
{}

GraphData::GraphData(const std::vector<float>& xValues, const std::vector<float>& yValues, const sf::Color& color, const std::string& label, GraphType type)
                    : _xValues(xValues), _yValues(yValues), _color(color), _label(label), _graphType(type)
{}

bool GraphData::isValid() const
{ 
    return (_xValues.size() == _yValues.size()); 
}

size_t GraphData::getID() const
{
    return this->_ID;
}

const std::vector<float>& GraphData::getXValues() const
{
    return this->_xValues;
}

void GraphData::setXValues(const std::vector<float>& xValues)
{ 
    this->_xValues.clear();
    this->_xValues = xValues;
}

const std::vector<float>& GraphData::getYValues() const
{
    return this->_yValues;
}

void GraphData::setYValues(const std::vector<float>& yValues)
{
    this->_yValues.clear();
    this->_yValues = yValues;
}

void GraphData::setValues(const std::vector<sf::Vector2f>& values)
{
    this->_xValues.clear();
    this->_yValues.clear();

    for (auto point: values)
    {
        _xValues.push_back(point.x);
        _yValues.push_back(point.y);
    }
}

void GraphData::setValues(const std::vector<float>& xValues, const std::vector<float>& yValues)
{
    this->_xValues.clear();
    this->_xValues = xValues;
    this->_yValues.clear();
    this->_yValues = yValues;
}

size_t GraphData::getDataLength() const
{
    return _xValues.size();
}

sf::Vector2f GraphData::getDataValue(size_t i) const
{
    return sf::Vector2f{_xValues[i], _yValues[i]};
}

void GraphData::setDataValue(size_t i, const sf::Vector2f& valuePair)
{
    this->_xValues[i] = valuePair.x;
    this->_yValues[i] = valuePair.y;
}

sf::Color GraphData::getColor() const
{
    return this->_color;
}

void GraphData::setColor(const sf::Color& color)
{
    this->_color = color;
}

const std::string& GraphData::getLabel() const
{
    return this->_label;
}

void GraphData::setLabel(const std::string& label)
{
    this->_label = label;
}

GraphType GraphData::getGraphType() const
{
    return this->_graphType;
}

void GraphData::setGraphType(GraphType type)
{
    this->_graphType = type;
}

void GraphData::push_back(const sf::Vector2f& pair)
{
    _xValues.push_back(pair.x);
    _yValues.push_back(pair.y);
}

void GraphData::popFront(const size_t n)
{
    size_t erase_count = std::min(n, _xValues.size());

    if (erase_count != 0)
    {
        _xValues.erase(_xValues.begin(), _xValues.begin() + erase_count);
        _yValues.erase(_yValues.begin(), _yValues.begin() + erase_count);
    }
}

void GraphData::clearPairValues()
{
    this->_xValues.clear();
    this->_yValues.clear();
}

void GraphData::setThickness(float value)
{
    _thickness = value;
}

float GraphData::getThickness() const
{
    return _thickness;
}

void GraphData::setID(size_t id)
{
    _ID = id;
}

size_t GraphData::getID()
{
    return _ID;
}
