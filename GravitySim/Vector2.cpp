#include "Vector2.h"

Vector2::Vector2() {
    x = 0;
    y = 0;
}

Vector2::Vector2(float x, float y) {
    this->x = x;
    this->y = y;
}

Vector2::Vector2(const Vector2& vec) {
    this->x = vec.x;
    this->y = vec.y;
}

Vector2& Vector2::operator=(const Vector2& vec) {
    this->x = vec.x;
    this->y = vec.y;

    return *this;
}

float Vector2::getMag() {
    return (float)sqrt(pow(x, 2) + pow(y, 2));
}

float Vector2::getAngle() {
    Vector2 unitVec = *this;
    unitVec.normalise();

    if (y >= 0) {
        return (float)acos(unitVec.x);
    }
    else {
        return (float)-acos(unitVec.x);
    }
}

Vector2& Vector2::set(float x, float y) {
    this->x = x;
    this->y = y;

    return *this;
}

Vector2& Vector2::setPolar(float magnitude, float theta) {
    this->x = (float)cos(theta) * magnitude;
    this->y = (float)sin(theta) * magnitude;

    return *this;
}

Vector2& Vector2::add(const Vector2& vec) {
    this->x += vec.x;
    this->y += vec.y;

    return *this;
}

Vector2& Vector2::sub(const Vector2& vec) {
    this->x -= vec.x;
    this->y -= vec.y;

    return *this;
}

Vector2& Vector2::multiply(float scalar) {
    x = x * scalar;
    y = y * scalar;

    return *this;
}

Vector2& Vector2::divide(float scalar) {
    if (scalar == 0) {
        return *this;
    }

    x = x / scalar;
    y = y / scalar;

    return *this;
}

Vector2& Vector2::normalise() {
    divide(getMag());

    return *this;
}

Vector2& Vector2::setMag(float magnitude) {
    normalise().multiply(magnitude);

    return *this;
}
