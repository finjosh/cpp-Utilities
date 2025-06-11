#ifndef TRANSFORM_BOX2D_HPP
#define TRANSFORM_BOX2D_HPP

#pragma once

#include "Utils/Transform.hpp"
#include "Utils/Vector2_Box2d.hpp"
#include "Utils/Rotation_Box2d.hpp"
#include "box2d/math_functions.h"

inline Transform toTransform(b2Transform transform) 
{
    return Transform(toTransform(transform.p), toRotation(transform.q));
}
inline b2Transform tob2Transform(const Transform& transform) const
{
    return b2Transform{tob2Vec2(transform.position), tob2Rot(transform.rotation)};
}

#endif
