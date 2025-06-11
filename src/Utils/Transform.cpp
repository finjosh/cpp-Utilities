#include "Utils/Transform.hpp"

Transform::Transform(const Vector2& position, Rotation rotation) : position(position), rotation(rotation) {}

void Transform::set(const Vector2& position, Rotation rotation)
{
    this->position = position;
    this->rotation = rotation;
}

void Transform::setZero()
{
    this->position.setZero();
    this->rotation.setZero();
}

Vector2 Transform::getLocalPoint(Vector2 point) const
{
    point -= this->position;
	return Vector2{rotation.cos * point.x + rotation.sin * point.y, -rotation.sin * point.x + rotation.cos * point.y};
}

Vector2 Transform::getGlobalPoint(Vector2 point) const
{
    float x = (rotation.cos * point.x - rotation.sin * point.y) + position.x;
	float y = (rotation.sin * point.x + rotation.cos * point.y) + position.y;
	return Vector2{x, y};
}

Vector2 Transform::getLocalVector(Vector2 vector) const
{
    return Vector2{rotation.cos * vector.x + rotation.sin * vector.y, -rotation.sin * vector.x + rotation.cos * vector.y};
}

Vector2 Transform::getGlobalVector(Vector2 vector) const
{
    return Vector2{rotation.cos * vector.x - rotation.sin * vector.y, rotation.sin * vector.x + rotation.cos * vector.y};
}

Transform Transform::operator + (const Transform& localTransform) const
{
    return Transform{position + Vector2::rotateAround(localTransform.position, {0,0}, rotation), rotation + localTransform.rotation};
}

void Transform::operator += (const Transform& localTransform)
{
    position += Vector2::rotateAround(localTransform.position, {0,0}, rotation);
    rotation += localTransform.rotation;
}
