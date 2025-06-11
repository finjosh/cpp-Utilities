#ifndef VECTOR2_BOX2D_HPP
#define VECTOR2_BOX2D_HPP

#pragma once

#include "Vector2.hpp"
#include "box2d/types.h"

inline Vector2 toVector2(const b2Vec2& box2DVector) 
{
    return Vector2(box2DVector.x, box2DVector.y);
}
/// @returns the equivalent b2Vec2
inline b2Vec2 tob2Vec2(const Vector2& vector) const
{
    return b2Vec2{vector.x,vector.y};
}

#endif
