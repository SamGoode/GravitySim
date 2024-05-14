#include "Sim.h"

float pi = (float)acos(-1);

Sim::Sim() {
    // kg/m^2
    densitySetting = 4.0 / 1.0f;
    count = 0;
    bodies = new Body[count];

    //bodies[0] = Body(400, 10, 200, 100);
    //bodies[1] = Body(80, 4, 225, 100);
    //bodies[2] = Body(100, 5, 200, 120);
    
    //bodies[0].vel.set(1, 0).setMag(1);
    //bodies[0].vel.setPolar(1, -pi/6);
    //bodies[1].vel.set(0, -1).setMag(0.2);
}

Sim::~Sim() {
    delete[] bodies;
}

Sim& Sim::addBody(float x, float y, int radius, Vector2 vel) {
    Body* oldPtr = bodies;

    count++;
    bodies = new Body[count];
    for (int i = 0; i < count - 1; i++) {
        bodies[i] = Body(oldPtr[i]);
    }
    delete[] oldPtr;
    
    bodies[count - 1] = Body(densitySetting*(pi*pow(radius, 2)), radius, x, y);
    bodies[count - 1].vel = vel;

    return *this;
}

Sim& Sim::addBody(float x, float y, int radius, float mass, Vector2 vel) {
    Body* oldPtr = bodies;

    count++;
    bodies = new Body[count];
    for (int i = 0; i < count - 1; i++) {
        bodies[i] = Body(oldPtr[i]);
    }
    delete[] oldPtr;

    bodies[count - 1] = Body(mass, radius, x, y);
    bodies[count - 1].vel = vel;

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
                //bodies[i].pos.sub(Vector2(AtoB).setMag((bodies[i].radius - (distance / 2)) * (1 * bodies[i].mass) / (bodies[i].mass + bodies[n].mass)));
                //bodies[n].pos.add(Vector2(AtoB).setMag((bodies[n].radius - (distance / 2)) * (1 * bodies[n].mass) / (bodies[i].mass + bodies[n].mass)));
                
                //new temp fix, I wanna incorporate the ratio of mass in distribution of distance clipped for each body, but I'll figure out the math for that later.
                if (abs(bodies[i].mass - bodies[n].mass) < 100) {
                    bodies[i].pos.sub(Vector2(AtoB).setMag(bodies[i].radius - (distance / 2)));
                    bodies[n].pos.add(Vector2(AtoB).setMag(bodies[n].radius - (distance / 2)));
                }
                else if (bodies[i].mass < bodies[n].mass) {
                    bodies[i].pos.sub(Vector2(AtoB).setMag(bodies[i].radius + bodies[n].radius - distance));
                }
                else {
                    bodies[n].pos.add(Vector2(AtoB).setMag(bodies[i].radius + bodies[n].radius - distance));
                }

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