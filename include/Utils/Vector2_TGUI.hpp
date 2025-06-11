#ifndef VECTOR2_TGUI_HPP
#define VECTOR2_TGUI_HPP

#pragma once

#include "Vector2.hpp"
#include "TGUI/Vector2.hpp"

/// @note you MUST include the tgui vector header BEFORE including this Vector2 header
/// @note converts from the given typename to a float   
template <typename T>
inline Vector2 toVector2(const tgui::Vector2<T>& TGUIVector) 
{
    return Vector2(static_cast<float>(TGUIVector.x), static_cast<float>(TGUIVector.y));
}

/// @note converts to the given typename from a float
/// @returns the equivelent tgui Vector2
template <typename T = float>
inline tgui::Vector2<T> toTGUIVector2(const Vector2& vector)
{
    return tgui::Vector2<T>{static_cast<T>(vector.x), static_cast<T>(vector.y)};
}

#endif
