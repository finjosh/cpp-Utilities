#ifndef ROTATION_BOX2D_HPP
#define ROTATION_BOX2D_HPP

#pragma once

#include "Utils/Rotation.hpp"
#include "box2d/math_functions.h"

inline Rotation toRotation(b2Rot rot) 
{
    return Rotation(rot.c, rot.s);
}

inline b2Rot tob2Rot(const Rotation& rot) const
{
    return b2MakeRot(rot.getRadians());
}

#endif
