#include "Utils/Vector2.hpp"
#include <math.h>
#include <algorithm>

Vector2::Vector2(float x, float y) : x(x), y(y) {}
Vector2::Vector2(const Vector2& vector) : x(vector.x), y(vector.y) {}

void Vector2::operator+=(const Vector2& vector)
{
    x += vector.x;
    y += vector.y;
}

Vector2 Vector2::operator+(const Vector2& vector) const
{
    return Vector2{x+vector.x, y+vector.y};
}

void Vector2::operator-=(const Vector2& vector)
{
    x -= vector.x;
    y -= vector.y;
}

Vector2 Vector2::operator-(const Vector2& vector) const
{
    return Vector2{x-vector.x, y-vector.y};
}

Vector2 Vector2::operator-() const
{
    return Vector2{-x,-y};
}

void Vector2::operator*=(float scaler)
{
    x *= scaler;
    y *= scaler;
}

Vector2 Vector2::operator*(float scaler) const
{
    return Vector2{x*scaler, y*scaler};
}

void Vector2::operator/=(float scaler)
{
    x /= scaler;
    y /= scaler;
}

Vector2 Vector2::operator/(float scaler) const
{
    return Vector2{x/scaler, y/scaler};
}

bool Vector2::operator==(const Vector2& vector) const
{
    return (x == vector.x && y == vector.y);
}

bool Vector2::operator!=(const Vector2& vector) const
{
    return (x != vector.x || y != vector.y);
}

void Vector2::operator=(const Vector2& vector)
{
    x = vector.x;
    y = vector.y;
}

bool Vector2::isValid() const
{
    return std::isfinite(x) && std::isfinite(y);
}

float Vector2::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const
{
    return x * x + y * y;
}

float Vector2::normalize()
{
    float length = this->length();
    if (length < __FLT_EPSILON__)
    {
        return 0.0f;
    }
    float invLength = 1.0f / length;
    x *= invLength;
    y *= invLength;

    return length;
}

Vector2 Vector2::normalize(const Vector2& vector)
{
    float length = vector.length();
    if (length < __FLT_EPSILON__)
    {
        return {0,0};
    }
    float invLength = 1.0f / length;

    return Vector2{vector.x * invLength, vector.y * invLength};
}

Vector2 Vector2::rotateAround(const Vector2& point, const Vector2& center, Rotation rot)
{
    Vector2 temp(point);
    temp -= center;
    float tempX = temp.x * rot.cos - temp.y * rot.sin;
    temp.y = temp.y * rot.cos + temp.x * rot.sin;
    temp.x = tempX;
    temp += center;
    return temp;
}

float Vector2::dot(const Vector2& a, const Vector2& b)
{
    return a.x*b.x + a.y*b.y;
}

float Vector2::distance(const Vector2& a, const Vector2& b)
{
    return (a-b).length();
}

float Vector2::distance(const Vector2& vector) const
{
    return (vector-(*this)).length();
}

Rotation Vector2::angle(Vector2 a, Vector2 b)
{
    a.normalize();
    b.normalize();
    return Rotation{b.x, b.y} - Rotation{a.x, a.y};
}

Rotation Vector2::angle(Vector2 b) const
{
    Vector2 a = Vector2::normalize(*this);
    b.normalize();
    return Rotation{b.x, b.y} - Rotation{a.x, a.y};
}

Rotation Vector2::rotation(Vector2 vector)
{
    vector.normalize();
    return Rotation{vector.x, vector.y};
}

Rotation Vector2::rotation() const
{
    Vector2 temp = Vector2::normalize(*this);
    return Rotation{temp.x, temp.y};
}

Vector2 Vector2::lerp(const Vector2& current, const Vector2& target, float relativeDistance)
{
    Vector2 rtn(target-current); // finding the vector from a to b
    rtn *= std::clamp(relativeDistance, 0.f, 1.f); // only getting the part we want
    rtn += current; // moving origin back to current
    return rtn;
}

Vector2 Vector2::lerpUnclamped(const Vector2& current, const Vector2& target, float relativeDistance)
{
    Vector2 rtn(target-current); // finding the vector from a to b
    rtn *= relativeDistance; // only getting the part we want
    rtn += current; // moving origin back to current
    return rtn;
}

Vector2 Vector2::max(const Vector2& a, const Vector2& b)
{ return Vector2{std::max(a.x, b.x), std::max(a.y, b.y)}; }

Vector2 Vector2::min(const Vector2& a, const Vector2& b)
{ return Vector2{std::min(a.x, b.x), std::min(a.y, b.y)}; }

Vector2 Vector2::moveTowards(const Vector2& current, const Vector2& target, float maxDistance)
{
    Vector2 rtn(target-current);
    float length = rtn.normalize();
    if (length > maxDistance)
    {
        rtn *= maxDistance;
    }
    else
        rtn *= length;
    rtn += current;
    return rtn;
}

Vector2 Vector2::rotateTowards(const Vector2& current, const Vector2& target, float maxRadians, float maxMagnitude)
{
    Vector2 rtn(current);
    float length = rtn.normalize();
    float targetLength = target.length();
    if (length > maxMagnitude)
    {
        rtn *= length + (length < targetLength ? maxMagnitude : -maxMagnitude);
    }
    else
    {
        rtn *= target.length();
    }

    float angle = Vector2::angle(current, target).getAngle();
    angle = angle > maxRadians ? maxRadians : (angle < -maxRadians ? -maxRadians : angle);

    return Vector2::rotate(rtn, angle);
}

Vector2 Vector2::scale(const Vector2& a, const Vector2& b)
{ return Vector2{a.x*b.x, a.y*b.y}; }

void Vector2::scale(const Vector2& vector)
{ x *= vector.x; y *= vector.y; }

Vector2 Vector2::rotate(const Vector2& vector, Rotation rot)
{ return Vector2{vector.x * rot.cos - vector.y * rot.sin, vector.y * rot.cos + vector.x * rot.sin}; }

void Vector2::rotate(Rotation rot)
{
    float temp = this->x;
    this->x = temp * rot.cos - this->y * rot.sin;
    this->y = this->y * rot.cos + temp * rot.sin;
}

void Vector2::setZero()
{ x = 0; y = 0; }

Vector2 Vector2::round() const
{ return Vector2{std::round(x), std::round(y)}; }

Vector2::Vector2(std::string str)
{
    if (str.empty())
    {
        // std::cerr << "Failed to parse Vector2. String was empty." << std::endl;
        return;
    }

    // Remove the brackets around the value
    if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
        str = str.substr(1, str.length() - 2);

    if (str.empty())
    {
        x = 0;
        y = 0;
        return;
    }

    auto commaPos = str.find(',');
    if (commaPos == std::string::npos)
    {
        // std::cerr << "Failed to parse Vector2 '" + str + "'. Expected numbers separated with a comma." << std::endl;
        return;
    }

    std::string temp = str.substr(0, commaPos);
    StringHelper::trim(temp);
    x = StringHelper::toFloat(temp);
    temp = str.substr(commaPos + 1);
    StringHelper::trim(temp);
    y = StringHelper::toFloat(temp);
}

std::string Vector2::toString() const
{
    return "(" + StringHelper::fromNumber(x) + ", " + StringHelper::fromNumber(y) + ")";
}