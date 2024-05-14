#include "Sim.h"

float pi = (float)acos(-1);

Sim::Sim() {
    count = 3;
    bodies = new Body[count];

    bodies[0] = Body(400, 10, 200, 100);
    //bodies[0].vel.set(1, 0).setMag(1);
    //bodies[0].vel.setPolar(1, -pi/6);
    bodies[1] = Body(80, 4, 225, 100);
    //bodies[1].vel.set(0, -1).setMag(0.2);
    bodies[2] = Body(100, 5, 200, 120);
}

Sim::~Sim() {
    delete[] bodies;
}

Sim& Sim::addBody(float mass, int radius, float x, float y) {
    Body* oldPtr = bodies;

    count++;
    bodies = new Body[count];
    for (int i = 0; i < count - 1; i++) {
        bodies[i] = Body(oldPtr[i]);
    }
    bodies[count - 1] = Body(mass, radius, x, y);
    
    delete[] oldPtr;

    return *this;
}

Sim& Sim::update() {
    float gConstant = 0.01f;

    //last loop should be unnecessary
    for (int i = 0; i < count - 1; i++) {
        for (int n = i + 1; n < count; n++) {
            //A bodies[i]
            //B bodies[n]

            Vector2 AtoB = Vector2(bodies[n].pos).sub(bodies[i].pos);
            float distance = AtoB.getMag();

            float gForceMag = gConstant * (bodies[1].mass * bodies[0].mass) / (float)pow(distance, 2);

            Vector2 gForce;
            gForce.setPolar(gForceMag, AtoB.getAngle());
            //gForce.divide(2);

            bodies[i].applyForce(gForce);
            bodies[n].applyForce(gForce.multiply(-1));
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int n = i + 1; n < count; n++) {
            //A bodies[i]
            //B bodies[n]

            Vector2 AtoB = Vector2(bodies[n].pos).sub(bodies[i].pos);
            float distance = AtoB.getMag();

            if (distance <= bodies[i].radius + bodies[n].radius) {
                float thetaA = bodies[i].vel.getAngle() - AtoB.getAngle();
                Vector2 impactVecA;
                impactVecA.setPolar((float)cos(thetaA) * bodies[i].vel.getMag(), AtoB.getAngle());
                float impactSpeedA = impactVecA.getMag();

                float thetaB = bodies[n].vel.getAngle() - AtoB.getAngle() + pi;
                Vector2 impactVecB;
                impactVecB.setPolar((float)cos(thetaB) * bodies[n].vel.getMag(), AtoB.getAngle() + pi);
                float impactSpeedB = -impactVecB.getMag();

                float m1 = bodies[i].mass;
                float v1 = impactSpeedA;
                float m2 = bodies[n].mass;
                float v2 = impactSpeedB;

                float newSpeedA = ((m2 * ((2 * v2) - v1)) + (m1 * v1)) / (m1 + m2);
                float newSpeedB = ((m1 * ((2 * v1) - v2)) + (m2 * v2)) / (m1 + m2);

                Vector2 a;
                a.setPolar(newSpeedA, impactVecA.getAngle());
                Vector2 b;
                b.setPolar(-newSpeedB, impactVecB.getAngle());
                bodies[i].vel.sub(impactVecA).add(a);
                bodies[n].vel.sub(impactVecB).add(b);

                //temp fix
                bodies[i].sumForce.set(0, 0);
                bodies[n].sumForce.set(0, 0);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        bodies[i].update();
    }

    return *this;
}

Sim& Sim::show(Screen& screen) {
    for (int i = 0; i < count; i++) {
        bodies[i].show(screen);
    }

    return *this;
}