#include "Utils/Graph.hpp"

size_t Graph::_lastID = 0;
std::map<std::string, GraphType> Graph::_stringToGraphType = {{"scatter", GraphType::Scatter}, {"histogram", GraphType::Histogram}, {"line", GraphType::Line}, {"bar", GraphType::Bar}};
std::map<GraphType, std::string> Graph::_graphTypeToString = {{GraphType::Scatter, "scatter"}, {GraphType::Histogram, "histogram"}, {GraphType::Line, "line"}, {GraphType::Bar, "bar"}};

void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::RectangleShape temp(_size);
    temp.setOrigin(_origin);
    temp.setPosition(_position);
    temp.setRotation(_rotation);
    temp.setTexture(&_texture);
    target.draw(temp, states);
}

GraphType Graph::strToType(const std::string& str)
{
    auto temp = _stringToGraphType.find(StringHelper::toLower_copy(str));
    if (temp == _stringToGraphType.end())
        return GraphType::Line;
    return temp->second;
}

std::string Graph::typeToString(const GraphType& type)
{
    auto temp = _graphTypeToString.find(type);
    if (temp == _graphTypeToString.end())
        return "Not in dictionary";
    return temp->second;
}

void Graph::Update()
{
    if (_wasChanged)
    {
        if (!_axesSetup)
            setupAxes(); 
        sf::RenderTexture render;
        sf::ContextSettings settings;
        settings.antialiasingLevel = _antialiasingLevel;
        render.create(_resolution.x, _resolution.y, settings);
        sf::RectangleShape background(sf::Vector2f((int)_resolution.x, (int)_resolution.y));
        background.setFillColor(_backgroundColor);
        render.draw(background);
        render.draw(drawBackgroundLines());

        bool renderedAxis = false;
        std::list<sw::Spline> splineData;
        for (const GraphData& data: _dataSets)
        {
            if (!renderedAxis && data.getGraphType() == GraphType::Scatter)
            {
                render.draw(drawAxisLines());
                render.draw(drawAxisStepIndicators());
                renderedAxis = true;
            }
            updateSpline(splineData,data);
            for (auto& i: splineData)
            {
                render.draw(i);
            }
            splineData.clear();
        }
        if (!renderedAxis)
        {
            render.draw(drawAxisLines());
            render.draw(drawAxisStepIndicators());
        }
        auto text = drawTextElements();
        for (auto& i: text)
        {
            render.draw(i);
        }

        render.display();
        _texture = render.getTexture();

        _wasChanged = false;
    }
}

sf::VertexArray Graph::drawBackgroundLines()
{
    sf::VertexArray rtn;
    rtn.setPrimitiveType(sf::PrimitiveType::Quads);

    // x axis
    for (float x = _xBounds.first; roundTo(x, _decimalPrecision) <= roundTo(_xBounds.second, _decimalPrecision); x += _StepSize.x)
    {
        sf::Vector2f temp(_backgroundLinesThickness/2, 0);
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, _yBounds.first)) - temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, _yBounds.first)) + temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, _yBounds.second)) + temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, _yBounds.second)) - temp));
    }

    // y axis
    for (float y = _yBounds.first; roundTo(y, _decimalPrecision) <= roundTo(_yBounds.second, _decimalPrecision); y += _StepSize.y)
    {
        sf::Vector2f temp(0, _backgroundLinesThickness/2);
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(_xBounds.first, y)) + temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(_xBounds.first, y)) - temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(_xBounds.second, y)) - temp));
        rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(_xBounds.second, y)) + temp));
    }

    return rtn;
}

sf::VertexArray Graph::drawAxisStepIndicators()
{
    sf::VertexArray rtn;
    rtn.setPrimitiveType(sf::PrimitiveType::Quads);

    // x axis
    int AxisIndicatorIndex = 0;
    for (float x = _xBounds.first; roundTo(x, _decimalPrecision) <= roundTo(_xBounds.second, _decimalPrecision); x += _StepSize.x)
    {
        sf::Vector2f LinePos(_margin + (AxisIndicatorIndex * (_resolution.x - _margin*2) / _numSteps.x), _resolution.y - _margin - _axesThickness/2 /* sub 5 for the thickness of the bar*/);

        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y - (_margin * 0.15f)), _axesColor)); // top left
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y + (_margin * 0.15f)), _axesColor)); // bottom left
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x + _axesThickness, LinePos.y + (_margin * 0.15f)), _axesColor)); // bottom right
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x + _axesThickness, LinePos.y - (_margin * 0.15f)), _axesColor)); // top right

        AxisIndicatorIndex++;
    }

    // y axis
    AxisIndicatorIndex = 0;
    for (float y = _yBounds.first; roundTo(y, _decimalPrecision) <= roundTo(_yBounds.second, _decimalPrecision); y += _StepSize.y)
    {
        sf::Vector2f LinePos(_margin + _axesThickness/2, _resolution.y - _margin - (AxisIndicatorIndex * (_resolution.y - _margin*2) / _numSteps.y) - _axesThickness);

        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x - (_margin * 0.15f), LinePos.y), _axesColor)); // bottom left
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x + (_margin * 0.15f), LinePos.y), _axesColor)); // bottom right
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x + (_margin * 0.15f), LinePos.y + _axesThickness), _axesColor)); // top right
        rtn.append(sf::Vertex(sf::Vector2f(LinePos.x - (_margin * 0.15f), LinePos.y + _axesThickness), _axesColor)); // top left

        AxisIndicatorIndex++;
    }

    return rtn;
}

sf::VertexArray Graph::drawAxisLines()
{
    sf::VertexArray rtn;
    rtn.setPrimitiveType(sf::PrimitiveType::Quads);

    // Y-axis
    rtn.append(sf::Vertex(sf::Vector2f(_margin, _margin), _axesColor)); // top left
    rtn.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin), _axesColor)); // bottom left
    rtn.append(sf::Vertex(sf::Vector2f(_margin + _axesThickness, _resolution.y - _margin), _axesColor)); // bottom right
    rtn.append(sf::Vertex(sf::Vector2f(_margin + _axesThickness, _margin), _axesColor)); // top right
    
    //BottomRight
    rtn.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin), _axesColor)); // bottom left
    rtn.append(sf::Vertex(sf::Vector2f(_resolution.x - _margin, _resolution.y - _margin), _axesColor)); // bottom right
    rtn.append(sf::Vertex(sf::Vector2f(_resolution.x - _margin, _resolution.y - _margin - _axesThickness), _axesColor)); // top right
    rtn.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin - _axesThickness), _axesColor)); // top left

    return rtn;
}

std::list<sf::Text> Graph::drawTextElements()
{
    _charSize = _charSize < 0 ? static_cast<unsigned int>(_margin * 0.3) : _charSize;
    std::list<sf::Text> rtn;

    /*
     * Axis Labels
     */
    sf::Text xAxisLabel;
    xAxisLabel.setFont(_font);
    xAxisLabel.setCharacterSize(_charSize);
    xAxisLabel.setFillColor(_axesColor);
    xAxisLabel.setString(_xAxisLabel);

    xAxisLabel.setPosition(_resolution.x - _margin/2 + xAxisLabel.getLocalBounds().height, _resolution.y - _margin - xAxisLabel.getLocalBounds().width);

    xAxisLabel.rotate(90);

    rtn.push_back(xAxisLabel);

    sf::Text yAxisLabel;
    yAxisLabel.setFont(_font);
    yAxisLabel.setCharacterSize(_charSize);
    yAxisLabel.setFillColor(_axesColor);
    yAxisLabel.setString(_yAxisLabel);

    yAxisLabel.setPosition(_margin, _margin/2 - yAxisLabel.getLocalBounds().height);

    rtn.push_back(yAxisLabel);

    /*
     * Axis indicator text
     */
    // x axis
    int AxisIndicatorIndex = 0;
    for (float x = _xBounds.first; roundTo(x, _decimalPrecision) <= roundTo(_xBounds.second, _decimalPrecision); x += _StepSize.x)
    {
        sf::Vector2f LinePos(_margin + (AxisIndicatorIndex * (_resolution.x - _margin*2) / _numSteps.x), _resolution.y - _margin - _axesThickness/2 /* sub 5 for the thickness of the bar*/);

        //text
        sf::Text indicatorText;
        indicatorText.setCharacterSize(_charSize);
        indicatorText.setFont(_font);
        indicatorText.setString(toString(roundTo(x, _decimalPrecision), _decimalPrecision));
        indicatorText.setFillColor(_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin(tDimension.left + tDimension.width / 2, tDimension.top + tDimension.height / 2);
        indicatorText.setPosition(LinePos.x, LinePos.y + (_margin/2) + (_margin * 0.15f));
        indicatorText.setRotation(_xTextRotation);

        rtn.emplace_back(indicatorText);
        AxisIndicatorIndex++;
    }

    // y axis
    AxisIndicatorIndex = 0;
    for (float y = _yBounds.first; roundTo(y, _decimalPrecision) <= roundTo(_yBounds.second, _decimalPrecision); y += _StepSize.y)
    {
        sf::Vector2f LinePos(_margin + _axesThickness/2, _resolution.y - _margin - (AxisIndicatorIndex * (_resolution.y - _margin*2) / _numSteps.y) - _axesThickness);

        //text
        sf::Text indicatorText;
        indicatorText.setCharacterSize(_charSize);
        indicatorText.setFont(_font);
        indicatorText.setString(toString(roundTo(y, _decimalPrecision), _decimalPrecision));
        indicatorText.setFillColor(_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin(tDimension.width/2, tDimension.height/2);
        indicatorText.rotate(_yTextRotation);
        indicatorText.setPosition(indicatorText.getGlobalBounds().width/2 + (_margin * 0.15f), LinePos.y);

        rtn.emplace_back(indicatorText);
        AxisIndicatorIndex++;
    }

    //* legend 
    sf::Vector2f legendPos(_resolution.x - (_margin * 0.2), 0);
    sf::Text segmentLegend;
    segmentLegend.setFont(_font);
    segmentLegend.setCharacterSize(_charSize);
    for (auto& dataset : _dataSets)
    {
        segmentLegend.setPosition(legendPos);
        segmentLegend.setFillColor(dataset.getColor());
        segmentLegend.setString(dataset.getLabel());

        sf::FloatRect dim = segmentLegend.getLocalBounds();
        segmentLegend.move(-dim.width, 0);

        legendPos += sf::Vector2f(0, segmentLegend.getGlobalBounds().height + (_margin * 0.1));

        rtn.push_back(segmentLegend);
    }

    return rtn;
}

void Graph::updateSpline(std::list<sw::Spline>& splineData, const GraphData& dataset)
{
    switch (dataset.getGraphType())
    {
    case GraphType::Scatter:
        for (size_t i = 0; i < dataset.getDataLength(); i++)
        {
            sf::Vector2f temp = convertValueToPoint(dataset.getDataValue(i));
            splineData.push_back({{temp + sf::Vector2f(1,0)}, {temp + sf::Vector2f(0,1)}, 
                                     {temp + sf::Vector2f(-1,0)}, {temp + sf::Vector2f(0,-1)}});
            auto& spline = splineData.back();
            spline.setColor(dataset.getColor());
            spline.setThickness(dataset.getThickness());
            spline.setThickCornerType(sw::Spline::ThickCornerType::Round);
            spline.setThickEndCapType(sw::Spline::ThickCapType::Round);
            spline.setThickStartCapType(sw::Spline::ThickCapType::Round);
            spline.update();
        }
        break;
    
    case GraphType::Line:
        {
        splineData.push_back({});
        auto& spline = splineData.back();
        spline.setColor(dataset.getColor());
        spline.setThickness(dataset.getThickness());
        spline.setThickCornerType(sw::Spline::ThickCornerType::Round);
        spline.setThickEndCapType(sw::Spline::ThickCapType::Round);
        spline.setThickStartCapType(sw::Spline::ThickCapType::Round);
        for (size_t i = 0; i < dataset.getDataLength(); i++)
        {
            spline.addVertex(convertValueToPoint(dataset.getDataValue(i)));
        } 
        spline.update();
        }
        break;

    case GraphType::Histogram:
        {
        float currentValue = dataset.getXValues().front();

        float yValue = 0.f;
        for (size_t i = 0; i < dataset.getDataLength(); i++)
        {
            sf::Vector2f value = dataset.getDataValue(i);
            if (value.x >= currentValue + _StepSize.x)
            {
                currentValue += _StepSize.x;

                float barWidth = convertValueToPoint({currentValue,0}).x - convertValueToPoint({currentValue - _StepSize.x,0}).x;
                auto point = convertValueToPoint({currentValue, yValue});
                splineData.push_back({{sf::Vector2f(point.x - barWidth/2, _resolution.y - _margin - 5)}, 
                                        {sf::Vector2f(point.x - barWidth/2, point.y)}});
                auto& spline = splineData.back();
                spline.setColor(dataset.getColor());
                spline.setThickness(barWidth);
                spline.update();
                yValue = 0.f;
            }
            yValue += value.y;
        }
        }
        break;

    case GraphType::Bar:
        for (size_t i = 0; i < dataset.getDataLength(); i++)
        {
            //Generate bars
            sf::Vector2f ValuePosition(convertValueToPoint(dataset.getDataValue(i)));

            splineData.push_back({{sf::Vector2f(ValuePosition.x, _resolution.y - _margin)}, {ValuePosition}});
            
            auto& spline = splineData.back();
            spline.setColor(dataset.getColor());
            spline.setThickness(dataset.getThickness());
            spline.update();
        }
        break;

    default: // this should never happen
        break;
    }
}

sf::Vector2f Graph::convertValueToPoint(sf::Vector2f dataValue)
{
   return sf::Vector2f(_margin + _axesThickness/2 + ((dataValue.x - _xBounds.first) / (_xBounds.second - _xBounds.first)) * (_resolution.x - _margin*2),
                        _resolution.y - _margin - _axesThickness/2 - ((dataValue.y - _yBounds.first) / (_yBounds.second - _yBounds.first)) * (_resolution.y - _margin*2)); 
}

std::string Graph::toString(const float& d, const size_t& precision)
{
    return StringHelper::FloatToStringRound(d, precision);
}

Graph::Graph(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& xLabel, const std::string& yLabel, 
                const sf::Color& backgroundColor, const unsigned int& antialiasingLevel, const float& margin, const sf::Vector2u& resolution, const unsigned int& decimalPrecision)
: _font(font), _xAxisLabel(xLabel), _yAxisLabel(yLabel), _antialiasingLevel(antialiasingLevel), _margin(margin), _resolution(resolution), _backgroundColor(backgroundColor), 
_decimalPrecision(decimalPrecision), _position(position), _size(size)
{}

void Graph::setPostion(const sf::Vector2f& pos)
{
    _position = pos;
}

void Graph::setRotation(const float& degree)
{
    _rotation = degree;
}

void Graph::setOrigin(const sf::Vector2f origin)
{
    _origin = origin;
}

void Graph::setSize(const sf::Vector2f& size)
{
    _size = size;
}

void Graph::setResolution(const sf::Vector2u& resolution)
{
    this->_resolution = resolution;
    this->_wasChanged = true;
}

void Graph::setXLable(const std::string& xLabel)
{
    this->_xAxisLabel = xLabel;
    this->_wasChanged = true;
}

void Graph::setYLable(const std::string& yLabel)
{
    this->_yAxisLabel = yLabel;
    this->_wasChanged = true;
}

void Graph::setAntialiasingLevel(const unsigned int& antialiasingLevel)
{
    this->_antialiasingLevel = antialiasingLevel;
    this->_wasChanged = true;
}

void Graph::setMargin(const float& margin)
{
    this->_margin = margin;
    this->_wasChanged = true;
}

void Graph::setBackgroundColor(const sf::Color& color)
{
    this->_backgroundColor = color;
    this->_wasChanged = true;
}

void Graph::setDecimalPrecision(const unsigned int& decimalPrecision)
{
    _decimalPrecision = decimalPrecision < 6 ? decimalPrecision : 6;
    this->_wasChanged = true;
}

void Graph::setAxesThickness(const unsigned int& thickness)
{
    _axesThickness = thickness;
    this->_wasChanged = true;
}

void Graph::setBackgroundLinesThickness(const unsigned int& thickness)
{
    _backgroundLinesThickness = thickness;
    this->_wasChanged = true;
}

sf::Vector2f Graph::getMinValue(const GraphData& graphData) const
{
    if (!graphData.isValid()) return {std::numeric_limits<float>().max(), std::numeric_limits<float>().max()};
    
    if (graphData.getGraphType() != GraphType::Histogram)
    {
        std::vector<float> setXAxis = graphData.getXValues();
        std::vector<float> setYAxis = graphData.getYValues();

        float setMinX = *std::min_element(setXAxis.begin(), setXAxis.end());
        float setMinY = *std::min_element(setYAxis.begin(), setYAxis.end());
        return {setMinX, setMinY};
    }
    else
    {
        float xMin = *std::min_element(graphData.getXValues().begin(), graphData.getXValues().end());
        float xMax = *std::max_element(graphData.getXValues().begin(), graphData.getXValues().end());

        float stepSize = (xMax - xMin) / (float)_numSteps.x;

        float yMin = INFINITY;

        float yValue = 0.f;
        float xValue = graphData.getXValues().front();
        for (size_t i = 0; i < graphData.getYValues().size(); i++)
        {
            if (graphData.getXValues()[i] >= xValue + stepSize)
            {
                xValue += stepSize;
                yMin = yValue < yMin ? yValue : yMin;
                yValue = 0.f;
            }
            yValue += graphData.getYValues()[i];
        }
        return {xMin, yMin};
    }
}

sf::Vector2f Graph::getMaxValue(const GraphData& graphData) const
{
    if (!graphData.isValid()) return {std::numeric_limits<float>().min(), std::numeric_limits<float>().min()};

    if (graphData.getGraphType() != GraphType::Histogram)
    {
        std::vector<float> setXAxis = graphData.getXValues();
        std::vector<float> setYAxis = graphData.getYValues();

        float setMaxX = *std::max_element(setXAxis.begin(), setXAxis.end());
        float setMaxY = *std::max_element(setYAxis.begin(), setYAxis.end());
        return {setMaxX, setMaxY};
    }
    else
    {
        float xMin = *std::min_element(graphData.getXValues().begin(), graphData.getXValues().end());
        float xMax = *std::max_element(graphData.getXValues().begin(), graphData.getXValues().end());

        float stepSize = (xMax - xMin) / (float)_numSteps.x;

        float yMax = -INFINITY;

        float yValue = 0.f;
        float xValue = graphData.getXValues().front();
        for (size_t i = 0; i < graphData.getYValues().size(); i++)
        {
            if (graphData.getXValues()[i] >= xValue + stepSize)
            {
                xValue += stepSize;
                yMax = yValue > yMax ? yValue : yMax;
                yValue = 0.f;
            }
            yValue += graphData.getYValues()[i];
        }
        return {xMax, yMax};
    }
}

void Graph::setupAxes()
{
    //Auto deducing step size and boundaries
    float xMin = INFINITY;
    float yMin = INFINITY;

    for (const auto& dataset : this->_dataSets)
    {
        if (!dataset.isValid()) continue;
        auto min = getMinValue(dataset);

        if (min.x < xMin)
        {
            xMin = min.x;
        }
        if (min.y < yMin)
        {
            yMin = min.y;
        }
    }

    float xMax = -INFINITY;
    float yMax = -INFINITY;

    for (const auto& dataset : this->_dataSets)
    {
        if (!dataset.isValid()) continue;
        auto max = getMaxValue(dataset);

        if (max.x > xMax)
        {
            xMax = max.x;
        }
        if (max.y > yMax)
        {
            yMax = max.y;
        }
    }

    _xBounds.first = xMin;
    _xBounds.second = xMax;

    _yBounds.first = yMin;
    _yBounds.second = yMax;

    _StepSize.x = (xMax - xMin) / (float)_numSteps.x;
    _StepSize.y = (yMax - yMin) / (float)_numSteps.y;

    this->_wasChanged = true;
    this->_axesSetup = false;
}

void Graph::setupAxes(const unsigned int& xSteps, const unsigned int& ySteps)
{
    _numSteps.x = xSteps;
    _numSteps.y = ySteps;

    this->setupAxes(); // using the setupAxis to find bounds automatically

    this->_wasChanged = true;
    this->_axesSetup = true;
}

size_t Graph::addDataSet(const GraphData& data_set)
{
    this->_dataSets.push_back(data_set);
    _dataSets.back().setID(++_lastID);
    _dataSets.sort([](GraphData& i, GraphData& j){ return (i.getGraphType() < j.getGraphType()); });
    _wasChanged = true;
    return _lastID;
}

void Graph::clearDataSets()
{
    this->_dataSets.clear();
    _wasChanged = true;
}

void Graph::removeDataSet(const size_t& ID)
{
    auto temp = std::find_if(this->_dataSets.begin(), this->_dataSets.end(), [ID](const GraphData& temp){ return temp.getID() == ID; });
    if (temp == this->_dataSets.end()) return;
    this->_dataSets.erase(temp);
    _wasChanged = true;
}

GraphData* Graph::getDataSet(const size_t& ID)
{
    auto temp = std::find_if(this->_dataSets.begin(), this->_dataSets.end(), [ID](const GraphData& temp){ return temp.getID() == ID; }).operator->();
    if (temp == this->_dataSets.end().operator->()) return nullptr;
    this->_wasChanged = true;
    return temp;
}

sf::Vector2f Graph::getPosition() const
{
    return _position;
}

float Graph::getRotation() const
{
    return _rotation;
}

sf::Vector2f Graph::getOrigin() const
{
     return _origin;
}

sf::Vector2f Graph::getSize() const
{
    return _size;
}

sf::Vector2u Graph::getResolution() const
{
    return _resolution;
}

std::string Graph::getXLable() const
{
    return _xAxisLabel;
}

std::string Graph::getYLable() const
{
    return _yAxisLabel;
}

unsigned int Graph::getAntialiasingLevel() const
{
    return _antialiasingLevel;
}

float Graph::getMargin() const
{
    return _margin;
}

sf::Color Graph::getBackgroundColor() const
{
    return _backgroundColor;
}

unsigned int Graph::getDecimalPrecision() const
{
    return _decimalPrecision;
}

unsigned int Graph::getAxesThickness() const
{
    return _axesThickness;
}

unsigned int Graph::getBackgroundLinesThickness() const
{
    return _backgroundLinesThickness;
}

sf::Texture Graph::getTexture_copy() const
{
    return _texture;
}

sf::Texture& Graph::getTexture()
{
    return _texture;
}

float Graph::roundTo(const float& value, unsigned int precision) const
{
    if (precision == 0) return round(value);
    return roundTo(value*10,precision-1)/10;
}

void Graph::setXTextRotation(const float& rotation)
{
    _xTextRotation = rotation;
    _wasChanged = true;
}

float Graph::getXTextRotation() const
{
    return _xTextRotation;
}

void Graph::setYTextRotation(const float& rotation)
{
    _yTextRotation = rotation;
    _wasChanged = true;
}

float Graph::getYTextRotation() const
{
    return _yTextRotation;
}

float Graph::getCharSize() const
{
    return (_charSize < 0 ? _margin*0.35 : _charSize);
}

void Graph::setCharSize(const float& size)
{
    _charSize = size;
    _wasChanged = true;
}

void Graph::setFont(const sf::Font& font)
{
    _font = font;
    _wasChanged = true;
}

const sf::Font& Graph::getFont() const
{
    return _font;
}

void Graph::setAxesColor(const sf::Color& color)
{
    _axesColor = color;
    _wasChanged = true;
}

sf::Color Graph::getAxesColor() const
{
    return _axesColor;
}
