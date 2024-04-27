#include "Utils/Graph.hpp"

void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(_graphImage, states);
}

void Graph::Update()
{
    if (_wasChanged)
    {
        this->GenerateVertices();

        sf::RenderTexture render;
        sf::ContextSettings settings;
        settings.antialiasingLevel = _antialiasingLevel;
        render.create(_resolution.x, _resolution.y, settings);
        sf::RectangleShape background(sf::Vector2f((int)_resolution.x, (int)_resolution.y));
        background.setFillColor(_backgroundColor);
        render.draw(background);
        render.draw(_backgroundGraphLines);

        bool renderedAxes = false;
        for (size_t i = 0; i < _graphedDataLines.size(); i++)
        {
            if (!renderedAxes && _graphedDataLines[i].first == GraphType::Scatter)
            {
                render.draw(_axesVertexArray);
                render.draw(_axesStepIndicators);
                renderedAxes = true;
            }
            render.draw(_graphedDataLines[i].second);
        }
        if (!renderedAxes)
        {
            render.draw(_axesVertexArray);
            render.draw(_axesStepIndicators);
        }
        for (size_t i = 0; i < _textElements.size(); i++)
        {
            render.draw(_textElements[i]);
        }

        render.display();
        _graphTexture = render.getTexture();

        _graphImage.setTexture(_graphTexture);
        _graphImage.setScale(_size.x / _resolution.x, _size.y / _resolution.y);
        _graphImage.setPosition(_graphImage.getPosition().x, _graphImage.getPosition().y);    

        _wasChanged = false;
    }
}

void Graph::GenerateVertices()
{
    this->ClearVertices();
    const auto characterSize = static_cast<unsigned int>(_margin * 0.35);
    _backgroundGraphLines.setPrimitiveType(sf::PrimitiveType::Quads);

    /*
     * Calculating axes themselves
     */
    _axesVertexArray.setPrimitiveType(sf::PrimitiveType::Quads);
    // Y-axis
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin, _margin), _axesColor)); // top left
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin), _axesColor)); // bottom left
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin + _axesThickness, _resolution.y - _margin), _axesColor)); // bottom right
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin + _axesThickness, _margin), _axesColor)); // top right
    
    //BottomRight
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin), _axesColor)); // bottom left
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_resolution.x - _margin, _resolution.y - _margin), _axesColor)); // bottom right
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_resolution.x - _margin, _resolution.y - _margin - _axesThickness), _axesColor)); // top right
    _axesVertexArray.append(sf::Vertex(sf::Vector2f(_margin, _resolution.y - _margin - _axesThickness), _axesColor)); // top left

    /*
     * Axis Labels
     */
    sf::Text xAxisLabel;
    xAxisLabel.setFont(_font);
    xAxisLabel.setCharacterSize(characterSize);
    xAxisLabel.setFillColor(_axesColor);
    xAxisLabel.setString(_xAxisLabel);

    xAxisLabel.setPosition(_resolution.x - _margin/2 + xAxisLabel.getLocalBounds().height, _resolution.y - _margin - xAxisLabel.getLocalBounds().width);

    xAxisLabel.rotate(90);

    _textElements.push_back(xAxisLabel);

    //------
    sf::Text yAxisLabel;
    yAxisLabel.setFont(_font);
    yAxisLabel.setCharacterSize(characterSize);
    yAxisLabel.setFillColor(_axesColor);
    yAxisLabel.setString(_yAxisLabel);

    yAxisLabel.setPosition(_margin, _margin/2 - yAxisLabel.getLocalBounds().height);

    _textElements.push_back(yAxisLabel);

    /*
     * Calculating axes indicators and adding text
     */
    _axesStepIndicators.setPrimitiveType(sf::PrimitiveType::Quads);
    // x axis
    int AxesIndicatorIndex = 0;
    for (float x = _xBounds.first; x <= _xBounds.second; x += _StepSize.x)
    {
        sf::Vector2f LinePos(_margin + (AxesIndicatorIndex * (_resolution.x - _margin*2) / _numSteps.x), _resolution.y - _margin - _axesThickness/2 /* sub 5 for the thickness of the bar*/);

        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y - (_margin * 0.25f)), _axesColor)); // top left
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y + (_margin * 0.25f)), _axesColor)); // bottom left
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x + _axesThickness, LinePos.y + (_margin * 0.25f)), _axesColor)); // bottom right
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x + _axesThickness, LinePos.y - (_margin * 0.25f)), _axesColor)); // top right

        sf::Vector2f temp(_backgroundLinesThickness/2, 0);
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(x, 0)) - temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(x, 0)) + temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(x, _yBounds.second)) + temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(x, _yBounds.second)) - temp));

        //text
        sf::Text indicatorText;
        indicatorText.setCharacterSize(characterSize);
        indicatorText.setFont(_font);
        indicatorText.setString(ToString(x, _decimalPrecision));
        indicatorText.setFillColor(_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin(tDimension.left + tDimension.width / 2, tDimension.top + tDimension.height / 2);
        indicatorText.setPosition(LinePos.x, LinePos.y + (_margin/2) + (_margin * 0.15f));
        indicatorText.setRotation(45);

        _textElements.push_back(indicatorText);
        AxesIndicatorIndex++;
    }

    // y axis
    AxesIndicatorIndex = 0;
    for (float y = _yBounds.first; y <= _yBounds.second; y += _StepSize.y)
    {
        sf::Vector2f LinePos(_margin + _axesThickness/2, _resolution.y - _margin - (AxesIndicatorIndex * (_resolution.y - _margin*2) / _numSteps.y) - _axesThickness);

        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x - (_margin * 0.25f), LinePos.y), _axesColor)); // bottom left
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x + (_margin * 0.25f), LinePos.y), _axesColor)); // bottom right
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x + (_margin * 0.25f), LinePos.y + _axesThickness), _axesColor)); // top right
        _axesStepIndicators.append(sf::Vertex(sf::Vector2f(LinePos.x - (_margin * 0.25f), LinePos.y + _axesThickness), _axesColor)); // top left

        sf::Vector2f temp(0, _backgroundLinesThickness/2);
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(0, y)) + temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(0, y)) - temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(_xBounds.second, y)) - temp));
        _backgroundGraphLines.append(sf::Vertex(ConvertValueToPoint(sf::Vector2f(_xBounds.second, y)) + temp));

        //text
        sf::Text indicatorText;
        indicatorText.setCharacterSize(characterSize);
        indicatorText.setFont(_font);
        indicatorText.setString(ToString(y, _decimalPrecision));
        indicatorText.setFillColor(_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin(tDimension.width/2, tDimension.height/2);
        indicatorText.rotate(45);
        indicatorText.setPosition(indicatorText.getGlobalBounds().width/2 + (_margin * 0.15f), LinePos.y);

        _textElements.push_back(indicatorText);
        AxesIndicatorIndex++;
    }

    /*
     * Calculate graphs
     */
    sf::Vector2f legendPos(_resolution.x - (_margin * 0.2), 0);
    for (auto& dataset : _dataSets)
    {
        //Generate legend
        sf::Text segmentLegend;
        segmentLegend.setPosition(legendPos);
        segmentLegend.setFont(_font);
        segmentLegend.setCharacterSize(static_cast<unsigned int>(_margin * 0.35));
        segmentLegend.setString(dataset.GetLabel());
        segmentLegend.setFillColor(dataset.GetColor());

        sf::FloatRect dim = segmentLegend.getLocalBounds();
        segmentLegend.move(-dim.width, 0);

        legendPos += sf::Vector2f(0, segmentLegend.getGlobalBounds().height + (_margin * 0.1));

        _textElements.push_back(segmentLegend);

        //Generate actual data lines
        GraphType type = dataset.GetGraphType();

        sf::Color lineColor = dataset.GetColor();

        if (type == GraphType::Scatter)
        {
            for (size_t i = 0; i < dataset.GetDataLength(); i++)
            {
                sf::Vector2f temp = ConvertValueToPoint(dataset.GetDataValue(i));
                _graphedDataLines.push_back(std::pair(GraphType::Scatter, sw::Spline({{temp + sf::Vector2f(1,0)}, {temp + sf::Vector2f(0,1)}, 
                                                        {temp + sf::Vector2f(-1,0)}, {temp + sf::Vector2f(0,-1)}})));
                sw::Spline& point = _graphedDataLines.back().second;
                point.setColor(lineColor);
                point.setThickness(_dotSize);
                point.setThickCornerType(sw::Spline::ThickCornerType::Round);
                point.setThickEndCapType(sw::Spline::ThickCapType::Round);
                point.setThickStartCapType(sw::Spline::ThickCapType::Round);
                point.update();
            }
        }
        else 
        if (type == GraphType::Line)
        {
            _graphedDataLines.push_back(std::pair(GraphType::Line, sw::Spline()));
            sw::Spline& lineData = _graphedDataLines.back().second;

            for (size_t i = 0; i < dataset.GetDataLength(); i++)
            {
                lineData.setColor(lineColor);
                lineData.setThickness(_lineThickness);
                lineData.setThickCornerType(sw::Spline::ThickCornerType::Round);
                lineData.setThickEndCapType(sw::Spline::ThickCapType::Round);
                lineData.setThickStartCapType(sw::Spline::ThickCapType::Round);

                // origin as in the graphs origin
                lineData.addVertex(ConvertValueToPoint(dataset.GetDataValue(i)));
            } 
            lineData.update();
        }
        else if (type == GraphType::Bar)
        {
            for (size_t i = 0; i < dataset.GetDataLength(); i++)
            {
                //Generate bars
                sf::Vector2f ValuePosition(ConvertValueToPoint(dataset.GetDataValue(i)));

                _graphedDataLines.push_back(std::pair(GraphType::Bar, sw::Spline({{sf::Vector2f(ValuePosition.x, _resolution.y - _margin - _barThickness)}, {ValuePosition}})));
                
                sw::Spline& line = _graphedDataLines.back().second;
                line.setColor(lineColor);
                line.setThickness(_margin/2);
                line.update();
            }
        }
        else if (type == GraphType::Histogram)
        {
            float range = _StepSize.x;
            float currentValue = _xBounds.first;

            std::vector<float> XTempDataSet;
            std::vector<float> YTempDataSet;
            while (currentValue <= _xBounds.second)
            {
                XTempDataSet.push_back(currentValue);
                YTempDataSet.push_back(0);
                currentValue += range;
            }

            currentValue = _xBounds.first;
            int CurrentIndex = 0;

            for (size_t i = 0; i < dataset.GetDataLength(); i++)
            {
                sf::Vector2f dataValue(dataset.GetDataValue(i));
                while (dataValue.x >= (currentValue + range))
                {
                    currentValue += range;
                    CurrentIndex++;
                }
                if (dataValue.x >= currentValue)
                    YTempDataSet[CurrentIndex] += dataValue.y;
            }

            XTempDataSet.push_back(_xBounds.second);
            YTempDataSet.push_back(0);
            dataset.setXValues(XTempDataSet);
            dataset.setYValues(YTempDataSet);

            this->SetupAxes(_axesColor, _numSteps.x, _numSteps.y);

            sf::Vector2f nextValuePosition(ConvertValueToPoint(dataset.GetDataValue(0)));
            for (size_t i = 1; i < dataset.GetDataLength() - 1; i++)
            {
                sf::Vector2f valuePosition(nextValuePosition);
                nextValuePosition = ConvertValueToPoint(dataset.GetDataValue(i));
                float barWidth(nextValuePosition.x - valuePosition.x);

                _graphedDataLines.push_back(std::pair(GraphType::Histogram, sw::Spline({{sf::Vector2f(valuePosition.x + barWidth/2 - 5, _resolution.y - _margin - 5)}, 
                                                        {sf::Vector2f(valuePosition.x + barWidth/2 - 5, valuePosition.y)}})));
                
                sw::Spline& bar = _graphedDataLines.back().second;
                bar.setColor(lineColor);
                bar.setThickness(barWidth);
                bar.update();
            }
        }
        else
        {
            // Should never happen
            exit(1);
        }
    }
}

void Graph::ClearVertices()
{
    _axesVertexArray.clear();
    _axesStepIndicators.clear();
    _backgroundGraphLines.clear();
    _graphedDataLines.clear();
    _textElements.clear();
}

sf::Vector2f Graph::ConvertValueToPoint(sf::Vector2f dataValue)
{
   return sf::Vector2f(_margin + _axesThickness/2 + (dataValue.x / _xBounds.second) * (_resolution.x - _margin*2),
                        _resolution.y - _margin - _axesThickness/2 - (dataValue.y / _yBounds.second) * (_resolution.y - _margin*2)); 
}

std::string Graph::ToString(const float& d, const size_t& precision)
{
    return StringHelper::FloatToStringRound(d, precision);
}

Graph::Graph(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& xLabel, const std::string& yLabel, 
                const sf::Color& backgroundColor, const unsigned int& antialiasingLevel, const float& margin, const sf::Vector2u& resolution, const unsigned int& decimalPrecision)
: _font(font), _xAxisLabel(xLabel), _yAxisLabel(yLabel), _antialiasingLevel(antialiasingLevel), _margin(margin), _resolution(resolution), _backgroundColor(backgroundColor), 
_decimalPrecision(decimalPrecision)
{
    _graphImage.setPosition(position);
    _size = size;
}

void Graph::setPostion(const sf::Vector2f& pos)
{
    _graphImage.setPosition(pos);
}

void Graph::setRotation(const float& degree)
{
    this->_graphImage.setRotation(degree);
}

void Graph::setOrigin(const sf::Vector2f origin)
{
    this->_graphImage.setOrigin(sf::Vector2f(origin.x / _graphImage.getScale().x, origin.y / _graphImage.getScale().y));
}

void Graph::setSize(const sf::Vector2f& size)
{
    _size = size;
    _graphImage.setScale(_size.x / _resolution.x, _size.y / _resolution.y);
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

void Graph::setFont(const sf::Font& font)
{
    this->_font = font;
    this->_wasChanged = true;
}

void Graph::setBackgroundColor(const sf::Color& color)
{
    this->_backgroundColor = color;
    this->_wasChanged = true;
}

void Graph::setDecimalPrecision(const unsigned int& decimalPrecision)
{
    _decimalPrecision = decimalPrecision;
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

void Graph::setLineThickness(const unsigned int& thickness)
{
    _lineThickness = thickness;
    this->_wasChanged = true;
}

void Graph::setScatterPlotSize(const unsigned int& size)
{
    _dotSize = size;
    this->_wasChanged = true;
}

void Graph::setBarThickness(const unsigned int& thickness)
{
    _barThickness = thickness;
    this->_wasChanged = true;
}

void Graph::SetupAxes()
{
    //Auto deducing step size and boundaries
    float xMin = INFINITY;
    float yMin = INFINITY;

    for (const auto& dataset : this->_dataSets)
    {
        if (!dataset.isValid()) continue;
        std::vector<float> setXAxis = dataset.GetXValues();
        std::vector<float> setYAxis = dataset.GetYValues();

        float setMinX = *std::min_element(setXAxis.begin(), setXAxis.end());
        float setMinY = *std::min_element(setYAxis.begin(), setYAxis.end());

        if (setMinX < xMin)
        {
            xMin = setMinX;
        }
        if (setMinY < yMin)
        {
            yMin = setMinY;
        }
    }

    float xMax = -INFINITY;
    float yMax = -INFINITY;

    for (const auto& dataset : this->_dataSets)
    {
        if (!dataset.isValid()) continue;
        std::vector<float> setXAxis = dataset.GetXValues();
        std::vector<float> setYAxis = dataset.GetYValues();

        float setMaxX = *std::max_element(setXAxis.begin(), setXAxis.end());
        float setMaxY = *std::max_element(setYAxis.begin(), setYAxis.end());

        if (setMaxX > xMax)
        {
            xMax = setMaxX;
        }
        if (setMaxY > yMax)
        {
            yMax = setMaxY;
        }
    }

    _xBounds.first = xMin;
    _xBounds.second = xMax;

    _yBounds.first = yMin;
    _yBounds.second = yMax;

    _StepSize.x = (xMax - xMin) / (float)_numSteps.x;
    _StepSize.y = (yMax - yMin) / (float)_numSteps.y;

    _axesColor = sf::Color::White;
    this->_wasChanged = true;
}

void Graph::SetupAxes(const sf::Color& axesColor, const unsigned int& xSteps, const unsigned int& ySteps)
{

    this->SetupAxes(); // using the setupAxes to find bounds automatically

    _numSteps.x = xSteps;
    _numSteps.y = ySteps;

    _StepSize.x = (_xBounds.second - _xBounds.first) / (float)_numSteps.x;
    _StepSize.y = (_yBounds.second - _yBounds.first) / (float)_numSteps.y;

    _axesColor = axesColor;

    this->_wasChanged = true;
}

void Graph::AddDataSet(const GraphData& data_set)
{
    this->_dataSets.push_back(data_set);
    std::sort(this->_dataSets.begin(), this->_dataSets.end(), [](GraphData& i, GraphData& j){ return (i.GetGraphType() < j.GetGraphType()); });
    _wasChanged = true;
}

void Graph::ClearDataSets()
{
    this->_dataSets.clear();
    _wasChanged = true;
}

void Graph::RemoveDataSet(const size_t& ID)
{
    auto temp = std::find_if(this->_dataSets.begin(), this->_dataSets.end(), [ID](GraphData& temp){ return temp.getID() == ID; });
    if (temp == this->_dataSets.end()) return;
    this->_dataSets.erase(temp);
    _wasChanged = true;
}

GraphData* Graph::getDataSet(const size_t& ID)
{
    auto temp = std::find_if(this->_dataSets.begin(), this->_dataSets.end(), [ID](GraphData& temp){ return temp.getID() == ID; }).operator->();
    if (temp == this->_dataSets.end().operator->()) return nullptr;
    return temp;
}
