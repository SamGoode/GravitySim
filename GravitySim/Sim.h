#pragma once
#include "Vector2.h"
#include "Body.h"

class Sim {
public:
    float densitySetting;
    int count;
    Body* bodies;

public:
    Sim();

    ~Sim();

    Sim& addBody(float x, float y, int radius, Vector2 vel);

    Sim& addBody(float x, float y, int radius, float mass, Vector2 vel);

    Sim& update();

    Sim& show(Screen& screen);
};