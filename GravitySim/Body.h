#pragma once
#include "Screen.h"
#include "Vector2.h"

class Body {
public:
    float mass;
    int radius;

    Vector2 pos;
    Vector2 vel;
    Vector2 acc;

    Vector2 sumForce;

public:
    Body();

    Body(float mass, int radius, float x, float y);

    Body(const Body& body);

    Body& applyForce(Vector2 force);

    Body& update();

    Body& show(Screen& screen);
};