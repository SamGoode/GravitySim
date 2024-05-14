#pragma once
#include "Vector2.h"
#include "Body.h"

class Sim {
public:
    int count;
    Body bodies[3];

public:
    Sim();

    Sim& update();

    Sim& show(Screen& screen);
};