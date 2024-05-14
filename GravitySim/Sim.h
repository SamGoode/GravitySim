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

    Sim& addBody(int radius, float x, float y);

    Sim& addBody(float mass, int radius, float x, float y);

    Sim& update();

    Sim& show(Screen& screen);
};