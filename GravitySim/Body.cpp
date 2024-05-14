#include "Body.h"

Body::Body() {
    mass = 1;
    radius = 1;
}

Body::Body(float mass, int radius, float x, float y) {
    this->mass = mass;
    this->radius = radius;

    pos = Vector2(x, y);
}

Body::Body(const Body& body) {
    this->mass = body.mass;
    this->radius = body.radius;

    pos = body.pos;
    vel = body.vel;
    acc = body.acc;
}

Body& Body::applyForce(Vector2 force) {
    sumForce.add(force);

    return *this;
}

Body& Body::update() {
    acc = Vector2(sumForce).divide(mass);
    sumForce.set(0, 0);

    vel.add(acc);
    pos.add(vel);

    return *this;
}

Body& Body::show(Screen& screen) {
    screen.gridInput((int)round(pos.x), (int)round(pos.y));

    screen.circle((int)round(pos.x), (int)round(pos.y), radius);

    return *this;
}