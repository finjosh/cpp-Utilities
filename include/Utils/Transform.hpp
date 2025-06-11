#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#pragma once

#include "Vector2.hpp"
#include "Rotation.hpp"

/// @note if you want helper functions to work with box2d transforms make sure to include them before including "Transform"
class Transform
{
public:
    /// @brief position = (0,0) rotation = 0
    inline Transform() = default;
    Transform(const Vector2& position, Rotation rotation);
	void set(const Vector2& position, Rotation rotation);
    /// @brief sets position to (0,0) and rotation to 0
    void setZero();
    Vector2 getLocalPoint(Vector2 point) const;
    Vector2 getGlobalPoint(Vector2 point) const;
    Vector2 getLocalVector(Vector2 vector) const;
    Vector2 getGlobalVector(Vector2 vector) const;

    /// @brief adds a local transform to a global transform
    /// @note left Transform will be the global and right will be the local
    Transform operator + (const Transform& localTransform) const;
    /// @brief adds a local transform to a global transform
    /// @note left Transform will be the global and right will be the local
    void operator += (const Transform& localTransform);

    Vector2 position = Vector2(0,0);
    Rotation rotation; // defaults to 0 dont init with 0 as that uses trig functions
protected:

private:

};

#endif
