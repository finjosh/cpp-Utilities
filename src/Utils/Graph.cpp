#include "Utils/Graph.hpp"

size_t Graph::m_lastID = 0;
std::map<std::string, GraphType> Graph::m_stringToGraphType = {{"scatter", GraphType::Scatter}, {"histogram", GraphType::Histogram}, {"line", GraphType::Line}, {"bar", GraphType::Bar}};
std::map<GraphType, std::string> Graph::m_graphTypeToString = {{GraphType::Scatter, "scatter"}, {GraphType::Histogram, "histogram"}, {GraphType::Line, "line"}, {GraphType::Bar, "bar"}};

void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::RectangleShape temp(m_size);
    temp.setOrigin(m_origin);
    temp.setPosition(m_position);
    temp.setRotation(m_rotation);
    temp.setTexture(&m_texture);
    target.draw(temp, states);
}

GraphType Graph::strToType(const std::string& str)
{
    auto temp = m_stringToGraphType.find(StringHelper::toLower_copy(str));
    if (temp == m_stringToGraphType.end())
        return GraphType::Line;
    return temp->second;
}

std::string Graph::typeToString(GraphType type)
{
    auto temp = m_graphTypeToString.find(type);
    if (temp == m_graphTypeToString.end())
        return "Not in dictionary";
    return temp->second;
}

void Graph::Update()
{
    if (m_wasChanged)
    {
        if (!m_axesSetup)
            setupAxes(); 
        sf::RenderTexture render;
        sf::ContextSettings settings;
        settings.antiAliasingLevel = m_antialiasingLevel;
        if (!render.resize(m_resolution, settings))
        {
            throw std::runtime_error("Unable to resize sf::RenderTexture");
        }
        sf::RectangleShape background(sf::Vector2f((int)m_resolution.x, (int)m_resolution.y));
        background.setFillColor(m_backgroundColor);
        render.draw(background);
        drawBackgroundLines(render);

        bool renderedAxis = false;
        std::list<sw::Spline> splineData;
        for (const GraphData& data: m_dataSets)
        {
            if (!renderedAxis && data.getGraphType() == GraphType::Scatter)
            {
                drawAxisLines(render);
                drawAxisStepIndicators(render);
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
            drawAxisLines(render);
            drawAxisStepIndicators(render);
        }
        drawTextElements(render);

        render.display();
        m_texture = render.getTexture();

        m_wasChanged = false;
    }
}

/// @warning assumes that vertexArr has a primitive type of triangles
/// @param topLeft x is left value y is top value
/// @param bottomRight x is right value and y is bottom value for rect bounds
static void drawRectangleViaTriangles(sf::VertexArray& vertexArr, sf::Vector2f topLeft, sf::Vector2f bottomRight, sf::Color color = sf::Color::White)
{
    // top left triangle
    vertexArr.append(sf::Vertex({topLeft.x, bottomRight.y})); // left bottom
    vertexArr.append(sf::Vertex({topLeft.x, topLeft.y})); // left top
    vertexArr.append(sf::Vertex({bottomRight.x, topLeft.y})); // right top
    // bottom right triangle
    vertexArr.append(sf::Vertex({topLeft.x, bottomRight.y})); // left bottom
    vertexArr.append(sf::Vertex({bottomRight.x, topLeft.y})); // right top
    vertexArr.append(sf::Vertex({bottomRight.x, bottomRight.y})); // right bottom
}

void Graph::drawBackgroundLines(sf::RenderTarget& target) const
{
    sf::VertexArray vertexArr;
    vertexArr.setPrimitiveType(sf::PrimitiveType::Triangles);

    float lineThickness = m_backgroundLinesThickness/2;
    // x axis
    for (float x = m_xBounds.first; roundTo(x, m_decimalPrecision) <= roundTo(m_xBounds.second, m_decimalPrecision); x += m_StepSize.x)
    {
        drawRectangleViaTriangles(vertexArr, convertValueToPoint(sf::Vector2f{x, m_yBounds.second}) - sf::Vector2f{lineThickness, 0}, 
                                             convertValueToPoint(sf::Vector2f{x, m_yBounds.first}) + sf::Vector2f{lineThickness, 0}, m_backgroundLinesColor);
        // // top left triangle
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.first)) - temp)); // bottom left
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.first)) + temp)); // top left
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.second)) + temp)); // top right
        // // bottom right triangle
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.first)) - temp)); // bottom left
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.second)) + temp)); // top right
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(x, m_yBounds.second)) - temp)); // bottom right
    }

    // y axis
    for (float y = m_yBounds.first; roundTo(y, m_decimalPrecision) <= roundTo(m_yBounds.second, m_decimalPrecision); y += m_StepSize.y)
    {
        drawRectangleViaTriangles(vertexArr, convertValueToPoint(sf::Vector2f{m_xBounds.first, y}) + sf::Vector2f{0, lineThickness}, 
                                             convertValueToPoint(sf::Vector2f{m_xBounds.second, y}) - sf::Vector2f{0, lineThickness}, m_backgroundLinesColor);
        // // top left triangle
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.first, y)) + temp)); // left bottom
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.first, y)) - temp)); // left top
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.second, y)) - temp)); // right top
        // // bottom right triangle
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.first, y)) + temp)); // left bottom
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.second, y)) - temp)); // right top
        // rtn.append(sf::Vertex(convertValueToPoint(sf::Vector2f(m_xBounds.second, y)) + temp)); // right bottom
    }

    target.draw(vertexArr);
}

void Graph::drawAxisStepIndicators(sf::RenderTarget& target) const
{
    sf::VertexArray vertexArr;
    vertexArr.setPrimitiveType(sf::PrimitiveType::Triangles);

    // x axis
    int AxisIndicatorIndex = 0;
    for (float x = m_xBounds.first; roundTo(x, m_decimalPrecision) <= roundTo(m_xBounds.second, m_decimalPrecision); x += m_StepSize.x)
    {
        sf::Vector2f LinePos(m_margin + (AxisIndicatorIndex * (m_resolution.x - m_margin*2) / m_numSteps.x), m_resolution.y - m_margin - m_axesThickness/2 /* sub 5 for the thickness of the bar*/);

        drawRectangleViaTriangles(vertexArr, sf::Vector2f{LinePos.x, LinePos.y - (m_margin * 0.15f)}, 
                                             sf::Vector2f{LinePos.x + m_axesThickness, LinePos.y + (m_margin * 0.15f)}, m_axesColor);
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y - (m_margin * 0.15f)), m_axesColor)); // top left
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x, LinePos.y + (m_margin * 0.15f)), m_axesColor)); // bottom left
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x + m_axesThickness, LinePos.y + (m_margin * 0.15f)), m_axesColor)); // bottom right
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x + m_axesThickness, LinePos.y - (m_margin * 0.15f)), m_axesColor)); // top right

        AxisIndicatorIndex++;
    }

    // y axis
    AxisIndicatorIndex = 0;
    for (float y = m_yBounds.first; roundTo(y, m_decimalPrecision) <= roundTo(m_yBounds.second, m_decimalPrecision); y += m_StepSize.y)
    {
        sf::Vector2f LinePos(m_margin + m_axesThickness/2, m_resolution.y - m_margin - (AxisIndicatorIndex * (m_resolution.y - m_margin*2) / m_numSteps.y) - m_axesThickness);

        drawRectangleViaTriangles(vertexArr, sf::Vector2f{LinePos.x - (m_margin * 0.15f), LinePos.y + m_axesThickness}, 
                                             sf::Vector2f{LinePos.x + (m_margin * 0.15f), LinePos.y}, m_axesColor);
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x - (m_margin * 0.15f), LinePos.y), m_axesColor)); // bottom left
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x + (m_margin * 0.15f), LinePos.y), m_axesColor)); // bottom right
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x + (m_margin * 0.15f), LinePos.y + m_axesThickness), m_axesColor)); // top right
        // vertexArr.append(sf::Vertex(sf::Vector2f(LinePos.x - (m_margin * 0.15f), LinePos.y + m_axesThickness), m_axesColor)); // top left

        AxisIndicatorIndex++;
    }

    target.draw(vertexArr);
}

void Graph::drawAxisLines(sf::RenderTarget& target) const
{
    sf::VertexArray vertexArr;
    vertexArr.setPrimitiveType(sf::PrimitiveType::Triangles);

    // Y - axis
    drawRectangleViaTriangles(vertexArr, sf::Vector2f{(float)m_margin, (float)m_margin}, 
                                         sf::Vector2f{(float)(m_margin + m_axesThickness), (float)(m_resolution.y - m_margin)}, m_axesColor);
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin, m_margin), m_axesColor)); // top left
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin, m_resolution.y - m_margin), m_axesColor)); // bottom left
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin + m_axesThickness, m_resolution.y - m_margin), m_axesColor)); // bottom right
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin + m_axesThickness, m_margin), m_axesColor)); // top right
    
    // X - axis
    drawRectangleViaTriangles(vertexArr, sf::Vector2f{(float)m_margin, (float)(m_resolution.y - m_margin - m_axesThickness)}, 
                                         sf::Vector2f{(float)(m_resolution.x - m_margin), (float)(m_resolution.y - m_margin)}, m_axesColor);
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin, m_resolution.y - m_margin), m_axesColor)); // bottom left
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_resolution.x - m_margin, m_resolution.y - m_margin), m_axesColor)); // bottom right
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_resolution.x - m_margin, m_resolution.y - m_margin - m_axesThickness), m_axesColor)); // top right
    // vertexArr.append(sf::Vertex(sf::Vector2f(m_margin, m_resolution.y - m_margin - m_axesThickness), m_axesColor)); // top left

    target.draw(vertexArr);
}

void Graph::drawTextElements(sf::RenderTarget& target)
{
    m_charSize = m_charSize < 0 ? static_cast<unsigned int>(m_margin * 0.3) : m_charSize;
    /*
     * Axis Labels
     */
    sf::Text xAxisLabel(m_font, m_xAxisLabel, m_charSize);
    xAxisLabel.setFillColor(m_axesColor);

    xAxisLabel.setPosition({m_resolution.x - m_margin/2 + xAxisLabel.getLocalBounds().size.y, m_resolution.y - m_margin - xAxisLabel.getLocalBounds().size.x});

    xAxisLabel.rotate(sf::degrees(90));

    target.draw(xAxisLabel);

    sf::Text yAxisLabel(m_font, m_yAxisLabel, m_charSize);
    yAxisLabel.setFillColor(m_axesColor);

    yAxisLabel.setPosition({(float)m_margin, (float)m_margin/2 - yAxisLabel.getLocalBounds().size.y});

    target.draw(yAxisLabel);

    /*
     * Axis indicator text
     */
    // x axis
    int AxisIndicatorIndex = 0;
    for (float x = m_xBounds.first; roundTo(x, m_decimalPrecision) <= roundTo(m_xBounds.second, m_decimalPrecision); x += m_StepSize.x)
    {
        sf::Vector2f LinePos(m_margin + (AxisIndicatorIndex * (m_resolution.x - m_margin*2) / m_numSteps.x), m_resolution.y - m_margin - m_axesThickness/2 /* sub 5 for the thickness of the bar*/);

        //text
        sf::Text indicatorText(m_font, StringHelper::FloatToStringRound(roundTo(x, m_decimalPrecision), m_decimalPrecision), m_charSize);
        indicatorText.setFillColor(m_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin({tDimension.size.x / 2, tDimension.size.y / 2});
        indicatorText.setPosition({LinePos.x, LinePos.y + (m_margin/2) + (m_margin * 0.15f)});
        indicatorText.setRotation(m_xTextRotation);

        target.draw(indicatorText);
        AxisIndicatorIndex++;
    }

    // y axis
    AxisIndicatorIndex = 0;
    for (float y = m_yBounds.first; roundTo(y, m_decimalPrecision) <= roundTo(m_yBounds.second, m_decimalPrecision); y += m_StepSize.y)
    {
        sf::Vector2f LinePos(m_margin + m_axesThickness/2, m_resolution.y - m_margin - (AxisIndicatorIndex * (m_resolution.y - m_margin*2) / m_numSteps.y) - m_axesThickness);

        //text
        sf::Text indicatorText(m_font, StringHelper::FloatToStringRound(roundTo(y, m_decimalPrecision), m_decimalPrecision), m_charSize);
        indicatorText.setFillColor(m_axesColor);

        sf::FloatRect tDimension = indicatorText.getLocalBounds();
        indicatorText.setOrigin({tDimension.size.x/2, tDimension.size.y/2});
        indicatorText.rotate(m_yTextRotation);
        indicatorText.setPosition({indicatorText.getGlobalBounds().size.x/2 + (m_margin * 0.15f), LinePos.y});

        target.draw(indicatorText);
        AxisIndicatorIndex++;
    }

    //* legend 
    sf::Vector2f legendPos(m_resolution.x - (m_margin * 0.2), 0);
    sf::Text segmentLegend(m_font, "", m_charSize);
    for (auto& dataset : m_dataSets)
    {
        segmentLegend.setPosition(legendPos);
        segmentLegend.setFillColor(dataset.getColor());
        segmentLegend.setString(dataset.getLabel());

        sf::FloatRect dim = segmentLegend.getLocalBounds();
        segmentLegend.move({-dim.size.x, 0});

        legendPos += sf::Vector2f(0, segmentLegend.getGlobalBounds().size.y + (m_margin * 0.1));

        target.draw(segmentLegend);
    }
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
            if (value.x >= currentValue + m_StepSize.x)
            {
                currentValue += m_StepSize.x;

                float barWidth = convertValueToPoint({currentValue,0}).x - convertValueToPoint({currentValue - m_StepSize.x,0}).x;
                auto point = convertValueToPoint({currentValue, yValue});
                splineData.push_back({{sf::Vector2f(point.x - barWidth/2, m_resolution.y - m_margin - 5)}, 
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

            splineData.push_back({{sf::Vector2f(ValuePosition.x, m_resolution.y - m_margin)}, {ValuePosition}});
            
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

sf::Vector2f Graph::convertValueToPoint(const sf::Vector2f& dataValue) const
{
   return sf::Vector2f(m_margin + m_axesThickness/2 + ((dataValue.x - m_xBounds.first) / (m_xBounds.second - m_xBounds.first)) * (m_resolution.x - m_margin*2),
                        m_resolution.y - m_margin - m_axesThickness/2 - ((dataValue.y - m_yBounds.first) / (m_yBounds.second - m_yBounds.first)) * (m_resolution.y - m_margin*2)); 
}

std::string Graph::toString(float d, size_t precision)
{
    return StringHelper::FloatToStringRound(d, precision);
}

Graph::Graph(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, const std::string& xLabel, const std::string& yLabel, 
                const sf::Color& backgroundColor, unsigned int antialiasingLevel, float margin, const sf::Vector2u& resolution, unsigned int decimalPrecision)
: m_font(font), m_xAxisLabel(xLabel), m_yAxisLabel(yLabel), m_antialiasingLevel(antialiasingLevel), m_resolution(resolution), m_backgroundColor(backgroundColor), 
m_decimalPrecision(decimalPrecision), m_position(position), m_size(size)
{
    setMargin(margin); // setting here so that char size is auto set
}

void Graph::setPostion(const sf::Vector2f& pos)
{
    m_position = pos;
}

void Graph::setRotation(sf::Angle angle)
{
    m_rotation = angle;
}

void Graph::setOrigin(const sf::Vector2f& origin)
{
    m_origin = origin;
}

void Graph::setSize(const sf::Vector2f& size)
{
    m_size = size;
}

void Graph::setResolution(const sf::Vector2u& resolution)
{
    this->m_resolution = resolution;
    this->m_wasChanged = true;
}

void Graph::setXLable(const std::string& xLabel)
{
    this->m_xAxisLabel = xLabel;
    this->m_wasChanged = true;
}

void Graph::setYLable(const std::string& yLabel)
{
    this->m_yAxisLabel = yLabel;
    this->m_wasChanged = true;
}

void Graph::setAntialiasingLevel(unsigned int antialiasingLevel)
{
    this->m_antialiasingLevel = antialiasingLevel;
    this->m_wasChanged = true;
}

void Graph::setMargin(float margin)
{
    this->m_margin = margin;
    this->m_wasChanged = true;
}

void Graph::setBackgroundColor(const sf::Color& color)
{
    this->m_backgroundColor = color;
    this->m_wasChanged = true;
}

void Graph::setDecimalPrecision(unsigned int decimalPrecision)
{
    m_decimalPrecision = decimalPrecision < 6 ? decimalPrecision : 6;
    this->m_wasChanged = true;
}

void Graph::setAxesThickness(unsigned int thickness)
{
    m_axesThickness = thickness;
    this->m_wasChanged = true;
}

void Graph::setBackgroundLinesThickness(unsigned int thickness)
{
    m_backgroundLinesThickness = thickness;
    this->m_wasChanged = true;
}

void Graph::setBackgroundLinesColor(sf::Color color)
{
    m_backgroundLinesColor = color;
}

sf::Color Graph::getBackgroundLinesColor() const
{
    return m_backgroundLinesColor;
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

        float stepSize = (xMax - xMin) / (float)m_numSteps.x;

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

        float stepSize = (xMax - xMin) / (float)m_numSteps.x;

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

    for (const auto& dataset : this->m_dataSets)
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

    for (const auto& dataset : this->m_dataSets)
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

    m_xBounds.first = xMin;
    m_xBounds.second = xMax;

    m_yBounds.first = yMin;
    m_yBounds.second = yMax;

    m_StepSize.x = (xMax - xMin) / (float)m_numSteps.x;
    m_StepSize.y = (yMax - yMin) / (float)m_numSteps.y;

    this->m_wasChanged = true;
    this->m_axesSetup = false;
}

void Graph::setupAxes(unsigned int xSteps, unsigned int ySteps)
{
    m_numSteps.x = xSteps;
    m_numSteps.y = ySteps;

    this->setupAxes(); // using the setupAxis to find bounds automatically

    this->m_wasChanged = true;
    this->m_axesSetup = true;
}

size_t Graph::addDataSet(const GraphData& data_set)
{
    this->m_dataSets.push_back(data_set);
    m_dataSets.back().setID(++m_lastID);
    m_dataSets.sort([](GraphData& i, GraphData& j){ return (i.getGraphType() < j.getGraphType()); });
    m_wasChanged = true;
    return m_lastID;
}

void Graph::clearDataSets()
{
    this->m_dataSets.clear();
    m_wasChanged = true;
}

void Graph::removeDataSet(size_t ID)
{
    auto temp = std::find_if(this->m_dataSets.begin(), this->m_dataSets.end(), [ID](const GraphData& temp){ return temp.getID() == ID; });
    if (temp == this->m_dataSets.end()) return;
    this->m_dataSets.erase(temp);
    m_wasChanged = true;
}

GraphData* Graph::getDataSet(size_t ID)
{
    auto temp = std::find_if(this->m_dataSets.begin(), this->m_dataSets.end(), [ID](const GraphData& temp){ return temp.getID() == ID; }).operator->();
    if (temp == this->m_dataSets.end().operator->()) return nullptr;
    this->m_wasChanged = true;
    return temp;
}

sf::Vector2f Graph::getPosition() const
{
    return m_position;
}

sf::Angle Graph::getRotation() const
{
    return m_rotation;
}

sf::Vector2f Graph::getOrigin() const
{
     return m_origin;
}

sf::Vector2f Graph::getSize() const
{
    return m_size;
}

sf::Vector2u Graph::getResolution() const
{
    return m_resolution;
}

std::string Graph::getXLable() const
{
    return m_xAxisLabel;
}

std::string Graph::getYLable() const
{
    return m_yAxisLabel;
}

unsigned int Graph::getAntialiasingLevel() const
{
    return m_antialiasingLevel;
}

float Graph::getMargin() const
{
    return m_margin;
}

sf::Color Graph::getBackgroundColor() const
{
    return m_backgroundColor;
}

unsigned int Graph::getDecimalPrecision() const
{
    return m_decimalPrecision;
}

unsigned int Graph::getAxesThickness() const
{
    return m_axesThickness;
}

unsigned int Graph::getBackgroundLinesThickness() const
{
    return m_backgroundLinesThickness;
}

sf::Texture Graph::getTexture_copy() const
{
    return m_texture;
}

sf::Texture& Graph::getTexture()
{
    return m_texture;
}

float Graph::roundTo(float value, uint8_t precision) const
{
    if (precision == 0) return round(value);
    return roundTo(value*10,precision-1)/10;
}

void Graph::setXTextRotation(sf::Angle rotation)
{
    m_xTextRotation = rotation;
    m_wasChanged = true;
}

sf::Angle Graph::getXTextRotation() const
{
    return m_xTextRotation;
}

void Graph::setYTextRotation(sf::Angle rotation)
{
    m_yTextRotation = rotation;
    m_wasChanged = true;
}

sf::Angle Graph::getYTextRotation() const
{
    return m_yTextRotation;
}

float Graph::getCharSize() const
{
    return m_charSize < 0 ? static_cast<unsigned int>(m_margin * 0.3) : m_charSize;
}

void Graph::setCharSize(float size)
{
    m_charSize = size;
    m_wasChanged = true;
}

void Graph::setFont(const sf::Font& font)
{
    m_font = font;
    m_wasChanged = true;
}

const sf::Font& Graph::getFont() const
{
    return m_font;
}

void Graph::setAxesColor(const sf::Color& color)
{
    m_axesColor = color;
    m_wasChanged = true;
}

sf::Color Graph::getAxesColor() const
{
    return m_axesColor;
}
