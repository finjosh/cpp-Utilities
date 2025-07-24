#ifndef ROTATION_HPP
#define ROTATION_HPP

#pragma once

#ifndef PI
#define PI 3.14159265358979323846f
#endif

class Vector2;

/// @note if you want the to string function to be defined include this the string include
class Rotation
{
public:
    inline Rotation() = default;
    /// @brief uses radians as the rotation
    /// @note for setting as degrees use setDegrees() 
    Rotation(float radians);
    Rotation(const Rotation& rotation);
    Rotation(float cos, float sin);

    /// @brief sets the stored sin and cos values with and angle in radians
    void set(float radians);
    inline void setDegrees(float degrees) { this->set(degrees/180.f*PI); }
    /// @returns the rotation/angle in radians
    float getAngle() const;
    /// @returns the rotation/angle in degrees
    float getAngleInDegrees() const;
    /// @brief same as "getAngleInDegrees()"
    inline float getDegrees() const { return getAngleInDegrees(); }
    /// @brief same as getDegrees()
    inline float asDegrees() const { return this->getDegrees(); }
    /// @brief same as "getAngle()"
    inline float getRadians() const { return getAngle(); }
    /// @brief same as getRadians()
    inline float asRadians() const { return this->getRadians(); }
    /// @brief sets the stored rotation to 0
    void setZero();
	/// @returns the rotated x axis (1,0) in terms of this rotation
	Vector2 GetXAxis() const;
    /// @returns the rotated y axis (0,1) in terms of this rotation
	Vector2 GetYAxis() const;
    /// @note this is clamped so the relative rotation is guarantied to be in the range [0,1]
    /// @param current the current rotation
    /// @param wanted the wanted rotation
    /// @param relativeRotation the relative rotation to travel
    /// @returns the rotation from current to target
    static Rotation lerp(const Rotation& current, const Rotation& target, float relativeRotation);
    /// @note this is not clamped so the relative rotation can be outside of the range [0,1]
    /// @param current the current rotation
    /// @param wanted the wanted rotation
    /// @param relativeRotation the relative rotation to travel
    /// @returns the rotation from current to target
    static Rotation lerpUnclamped(const Rotation& current, const Rotation& target, float relativeRotation);

    Rotation operator + (Rotation rotation) const;
    void operator += (Rotation rotation);
    Rotation operator * (float scaler) const;
    void operator *= (float scaler);
    Rotation operator - (Rotation rotation) const;
    void operator -= (Rotation rotation);
    Rotation operator - () const;
    void operator = (Rotation rotation);

    float sin = 0.f, cos = 1.f;
protected:

private:

};

#endif
