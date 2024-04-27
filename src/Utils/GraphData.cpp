#include "Utils/GraphData.hpp"

size_t GraphData::lastID = 0;

GraphData::GraphData()
{
    _color = sf::Color::White;
    _label = "Unlabeled";
    _graphType = GraphType::Scatter;
    this->_ID = ++lastID;
}

GraphData::GraphData(const sf::Color& color, const std::string& label, const GraphType& type) : _color(color), _label(label), _graphType(type)
{ this->_ID = ++lastID; }

GraphData::GraphData(const std::vector<float>& xValues, const std::vector<float>& yValues, const sf::Color& color, const std::string& label, const GraphType& type)
                    : _xValues(xValues), _yValues(yValues), _color(color), _label(label), _graphType(type)
{ 
    this->_ID = ++lastID;
}

bool GraphData::isValid() const
{ 
    return (_xValues.size() == _yValues.size()); 
}

size_t GraphData::getID() const
{
    return this->_ID;
}

const std::vector<float>& GraphData::GetXValues() const
{
    return this->_xValues;
}

void GraphData::setXValues(const std::vector<float>& xValues)
{ 
    this->_xValues.clear();
    this->_xValues = xValues;
}

const std::vector<float>& GraphData::GetYValues() const
{
    return this->_yValues;
}

void GraphData::setYValues(const std::vector<float>& yValues)
{
    this->_yValues.clear();
    this->_yValues = yValues;
}

size_t GraphData::GetDataLength() const
{
    return _xValues.size();
}

sf::Vector2f GraphData::GetDataValue(const size_t& i) const
{
    return sf::Vector2f{_xValues[i], _yValues[i]};
}

void GraphData::SetDataValue(const size_t& i, const sf::Vector2f& valuePair)
{
    this->_xValues[i] = valuePair.x;
    this->_yValues[i] = valuePair.y;
}

sf::Color GraphData::GetColor() const
{
    return this->_color;
}

void GraphData::SetColor(const sf::Color& color)
{
    this->_color = color;
}

const std::string& GraphData::GetLabel() const
{
    return this->_label;
}

void GraphData::SetLabel(const std::string& label)
{
    this->_label = label;
}

GraphType GraphData::GetGraphType() const
{
    return this->_graphType;
}

void GraphData::SetGraphType(const GraphType& type)
{
    this->_graphType = type;
}

void GraphData::PushPair_back(const sf::Vector2f& pair)
{
    _xValues.push_back(pair.x);
    _yValues.push_back(pair.y);
}

void GraphData::PopFront(const size_t n)
{
    size_t erase_count = std::min(n, _xValues.size());

    if (erase_count != 0)
    {
        _xValues.erase(_xValues.begin(), _xValues.begin() + erase_count);
        _yValues.erase(_yValues.begin(), _yValues.begin() + erase_count);
    }
}

void GraphData::ClearPairValues()
{
    this->_xValues.clear();
    this->_yValues.clear();
}
