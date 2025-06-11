#ifndef VECTOR2_SFML_HPP
#define VECTOR2_SFML_HPP

#pragma once

#include "Vector2.hpp"
#include "SFML/System/Vector2.hpp"

/// @note converts from the given typename to a float
template <typename T>
inline Vector2 toVector2(const sf::Vector2<T>& SFMLVector) 
{
    return Vector2(static_cast<float>(SFMLVector.x), static_cast<float>(SFMLVector.y));
}
/// @note converts to the given typename from a float
/// @returns the equivelent sf Vector2
template <typename T = float>
inline sf::Vector2<T> toSFMLVector2(const Vector2& vector)
{
    return sf::Vector2<T>{static_cast<T>(vector.x), static_cast<T>(vector.y)};
}

#endif
