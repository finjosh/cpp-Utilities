#ifndef ROTATION_SFML_HPP
#define ROTATION_SFML_HPP

#pragma once

#include "Utils/Rotation.hpp"
#include "SFML/System/Angle.hpp"

inline Rotation toRotation(sf::Angle angle) 
{ return Rotation(angle.asRadians()); }
inline sf::Angle toSFMLRotation(const Rotation& rot)
{ return sf::radians(rot.getAngle()); }

#endif
