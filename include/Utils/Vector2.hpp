#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#pragma once

#include <string>
#include "Utils/StringHelper.hpp"

#include "Rotation.hpp"

/// @note if you want helper functions to work with tgui vectors make sure to include them before including "Vector2"
class Vector2
{
public:
    inline Vector2() = default;
    Vector2(float x, float y);
    Vector2(const Vector2& vector);
    /// @brief attempts to convert the given string to a vector2 and initializes this
    Vector2(std::string str);
    std::string toString() const;

    /// @brief adds given vector to this
    void operator+=(const Vector2& vector);
    /// @note if you call this a lot do think about using the += operator
    Vector2 operator+(const Vector2& vector) const;
    /// @brief subtracts given vector from this
    void operator-=(const Vector2& vector);
    /// @note if you call this a lot do think about using the -= operator
    Vector2 operator-(const Vector2& vector) const;
    /// @returns a vector with this vectors components negated
    Vector2 operator-() const;
    /// @brief multiplies this vector by a scalar
    void operator*=(float scaler);
    /// @note if you call this a lot do think about using the *= operator
    Vector2 operator*(float scaler) const;
    void operator/=(float scaler);
    /// @note if you call this a lot do think about using the /= operator
    Vector2 operator/(float scaler) const;
    void operator=(const Vector2& vector);
    /// @returns true if both x and y are equal
    bool operator==(const Vector2& vector) const;
    /// @returns true if both x and y are equal
    bool operator!=(const Vector2& vector) const;

	/// @returns true if this vector contains finite coordinates
	bool isValid() const;
    float length() const;
    float lengthSquared() const;
	/// @brief Convert this vector into a unit vector
    /// @note returns the original length
    float normalize();
    static Vector2 normalize(const Vector2& vector);
    /// @brief rotates the given Vector2 around the given center
    /// @param point the point that will be rotated
    /// @param center the point to rotate around
    /// @param rot rotation in radians
    /// @returns the point rotated
    static Vector2 rotateAround(const Vector2& point, const Vector2& center, Rotation rot);
    /// @returns the dot product of the two vectors
    static float dot(const Vector2& a, const Vector2& b);
    /// @returns the distance between the two vectors
    static float distance(const Vector2& a, const Vector2& b);
    /// @returns the distance between this and the given vector
    float distance(const Vector2& vector) const;
    /// @note angle is [-PI,PI]
    /// @note this is treating the vectors as directions not positions
    /// @returns the smallest absolute angle from a to b in radians
    static Rotation angle(Vector2 a, Vector2 b);
    /// @note angle is [-PI,PI]
    /// @note this is treating the vectors as directions not positions
    /// @returns the smallest absolute angle from a (this) to b in radians
    Rotation angle(Vector2 b) const;
    /// @note the returned angle is from 0 to PI
    /// @note treats the vector as a direction not position
    /// @returns the rotation of the vector from the positive x axis
    static Rotation rotation(Vector2 vector);
    /// @note the returned angle is from 0 to PI
    /// @note treats this vector as a direction not position
    /// @returns the rotation of this vector from the positive x axis
    Rotation rotation() const;
    /// @note a relativeDistance of 0 will return current, 1 will return target, and 0.5 will return the point midway between current and target
    /// @note relativeDistance is clamped between 0 and 1 (use lerpUnclamped if you dont want this functionality)
    /// @param relativeDistance the relative distance the returned vector will be (0-1)
    /// @returns a point between the two points
    static Vector2 lerp(const Vector2& current, const Vector2& target, float relativeDistance);
    /// @note a relativeDistance of 0 will return current, 1 will return target, and 0.5 will return the point midway between current and target
    /// @note relativeDistance can be outside of the range [0,1]
    /// @param relativeDistance the relative distance the returned vector will be
    /// @returns a point based on the distance between the two points and relative distance
    static Vector2 lerpUnclamped(const Vector2& current, const Vector2& target, float relativeDistance);
    /// @returns a vector with the max components from a and b
    /// @note i.e. (1,2) and (2,1) will return (2,2)
    static Vector2 max(const Vector2& a, const Vector2& b);
    /// @returns a vector with the min components from a and b
    /// @note i.e. (1,2) and (2,1) will return (1,1)
    static Vector2 min(const Vector2& a, const Vector2& b);
    /// @param maxDistance that max distance that the returned vector will be from current
    /// @returns a vector between current and target moving no further than maxDistance
    static Vector2 moveTowards(const Vector2& current, const Vector2& target, float maxDistance);
    /// @brief rotates the vector a towards b
    /// @param maxRadians the max change in radians
    /// @param maxMagnitude the max change in magnitude
    /// @returns the new vector
    static Vector2 rotateTowards(const Vector2& current, const Vector2& target, float maxRadians, float maxMagnitude);
    /// @returns a vector where the two vectors are multiplied component wise
    static Vector2 scale(const Vector2& a, const Vector2& b);
    /// @brief multiplies this and the given vector component wise and applies result to this vector
    void scale(const Vector2& vector);
    /// @param rot rotation in radians
    /// @returns the rotated vector
    static Vector2 rotate(const Vector2& vector, Rotation rot);
    /// @param rot rotation in radians
    void rotate(Rotation rot);
    /// @brief sets this vector to (0,0)
    void setZero();
    /// @returns a Vector with each component of this vector rounded to the nearest whole value
    Vector2 round() const;

    float x = 0;
    float y = 0;
};

#endif
