#pragma once
#include <math.h>

class Vector2 {
public:
    float x;
    float y;

public:
    Vector2();

    Vector2(float x, float y);

    Vector2(const Vector2& vec);

    Vector2& operator=(const Vector2& vec);

    float getMag();

    float getAngle();

    Vector2& set(float x, float y);

    Vector2& setPolar(float magnitude, float theta);

    Vector2& add(const Vector2& vec);

    Vector2& sub(const Vector2& vec);

    Vector2& multiply(float scalar);

    Vector2& divide(float scalar);

    Vector2& normalise();

    Vector2& setMag(float magnitude);
};