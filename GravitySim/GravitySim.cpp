#include <iostream>
#include <windows.h>
#include "Screen.h"

float pi = acos(-1);

class Vector2 {
public:
    float x;
    float y;

public:
    Vector2() {
        x = 0;
        y = 0;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    Vector2(const Vector2& vec) {
        this->x = vec.x;
        this->y = vec.y;
    }

    Vector2& operator=(const Vector2& vec) {
        this->x = vec.x;
        this->y = vec.y;

        return *this;
    }

    float getMag() {
        return sqrt(pow(x, 2) + pow(y, 2));
    }

    float getAngle() {
        Vector2 unitVec = *this;
        unitVec.normalise();

        if (y >= 0) {
            return acos(unitVec.x);
        }
        else {
            return -acos(unitVec.x);
        }
    }

    Vector2& set(float x, float y) {
        this->x = x;
        this->y = y;

        return *this;
    }

    Vector2& setPolar(float magnitude, float theta) {
        this->x = cos(theta) * magnitude;
        this->y = sin(theta) * magnitude;

        return *this;
    }

    Vector2& add(const Vector2& vec) {
        this->x += vec.x;
        this->y += vec.y;

        return *this;
    }

    Vector2& sub(const Vector2& vec) {
        this->x -= vec.x;
        this->y -= vec.y;

        return *this;
    }

    Vector2& multiply(float scalar) {
        x = x * scalar;
        y = y * scalar;

        return *this;
    }

    Vector2& divide(float scalar) {
        if (scalar == 0) {
            return *this;
        }

        x = x / scalar;
        y = y / scalar;

        return *this;
    }

    Vector2& normalise() {
        divide(getMag());

        return *this;
    }

    Vector2& setMag(float magnitude) {
        normalise().multiply(magnitude);

        return *this;
    }
};

static Vector2 mapScreenToGrid(int x, int y) {
    return Vector2(x / 2, y);
}

void gridInput(Screen& screen, int gridX, int gridY) {
    screen.input(219, gridX * 2, gridY);
    screen.input(219, gridX * 2 + 1, gridY);
}

class Body {
public:
    int mass;

    Vector2 pos;
    Vector2 vel;
    Vector2 acc;

public:
    Body() {
        mass = 1;
    }

    Body(int mass, float x, float y) {
        this->mass = mass;

        pos = Vector2(x, y);
    }

    Body(const Body& body) {
        this->mass = mass;

        pos = body.pos;
        vel = body.vel;
        acc = body.acc;
    }

    Body& update() {
        vel.add(acc);
        pos.add(vel);

        return *this;
    }

    Body& show(Screen& screen) {
        //gridInput(screen, round(pos.x), round(pos.y));
        gridInput(screen, round(pos.x), round(pos.y) - 2);
        gridInput(screen, round(pos.x) + 1, round(pos.y) - 1);
        gridInput(screen, round(pos.x) + 2, round(pos.y));
        gridInput(screen, round(pos.x) + 1, round(pos.y) + 1);
        gridInput(screen, round(pos.x), round(pos.y) + 2);
        gridInput(screen, round(pos.x) - 1, round(pos.y) + 1);
        gridInput(screen, round(pos.x) - 2, round(pos.y));
        gridInput(screen, round(pos.x) - 1, round(pos.y) - 1);

        return *this;
    }
};

class Sim {
public:
    int count;
    Body bodies[2];

public:
    Sim() {
        count = 2;
        bodies[0] = Body(400, 200, 100);
        //bodies[0].vel.set(1, 0).setMag(1);
        //bodies[0].vel.setPolar(1, -pi/6);
        bodies[1] = Body(80, 225, 100);
        bodies[1].vel.set(0, -1).setMag(0.01);
    }

    Sim& update() {
        Vector2 AtoB = Vector2(bodies[1].pos).sub(bodies[0].pos);
        float distance = AtoB.getMag();
        
        if (distance < 6) {
            float thetaA = bodies[0].vel.getAngle() - AtoB.getAngle();
            Vector2 collisionA;
            collisionA.setPolar(cos(thetaA) * bodies[0].vel.getMag(), AtoB.getAngle());
            float impactSpeedA = collisionA.getMag();

            float thetaB = bodies[1].vel.getAngle() - AtoB.getAngle()+acos(-1);
            Vector2 collisionB;
            collisionB.setPolar(cos(thetaB) * bodies[1].vel.getMag(), AtoB.getAngle() + pi);
            float impactSpeedB = -collisionB.getMag();

            float m1 = bodies[0].mass;
            float v1 = impactSpeedA;
            float m2 = bodies[1].mass;
            float v2 = impactSpeedB;

            float newSpeedA = ((m2 * ((2 * v2) - v1)) + (m1 * v1)) / (m1 + m2);
            float newSpeedB = ((m1 * ((2 * v1) - v2)) + (m2 * v2)) / (m1 + m2);

            Vector2 a; 
            a.setPolar(newSpeedA, collisionA.getAngle());
            Vector2 b;
            b.setPolar(-newSpeedB, collisionB.getAngle());
            Vector2 newVelA = Vector2(bodies[0].vel).sub(collisionA).add(a);
            Vector2 newVelB = Vector2(bodies[1].vel).sub(collisionB).add(b);

            bodies[0].vel = newVelA;
            bodies[1].vel = newVelB;
        }
        else {
            float gConstant = 0.01;
            float gForceMag = gConstant * (bodies[1].mass * bodies[0].mass) / pow(distance, 2);

            Vector2 gForce;
            gForce.setPolar(gForceMag, AtoB.getAngle());
            //gForce.divide(2);

            bodies[0].acc = Vector2(gForce).divide(bodies[0].mass);
            bodies[1].acc = Vector2(gForce).multiply(-1).divide(bodies[1].mass);
        }

        for (int i = 0; i < count; i++) {
            bodies[i].update();
        }

        return *this;
    }

    Sim& show(Screen& screen) {
        for (int i = 0; i < count; i++) {
            bodies[i].show(screen);
        }

        return *this;
    }
};

int main() {
    SetConsoleOutputCP(437);

    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);

    DWORD consoleMode;
    GetConsoleMode(hout, &consoleMode);
    SetConsoleMode(hout, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleMode(hin, ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);

    SHORT width = 800;
    SHORT height = 200;

    SetConsoleScreenBufferSize(hout, { width, height });

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

    GetCurrentConsoleFontEx(hout, false, &cfi);

    cfi.dwFontSize.Y = 4;
    cfi.dwFontSize.X = 2;

    SetCurrentConsoleFontEx(hout, false, &cfi);

    SMALL_RECT dim = { 0, 0, width - 1, height - 1 };

    if (!SetConsoleWindowInfo(hout, true, &dim)) {
        cfi.dwFontSize.Y = 16;
        cfi.dwFontSize.X = 9;

        SetCurrentConsoleFontEx(hout, false, &cfi);

        SetConsoleWindowInfo(hout, true, &dim);
    }

    INPUT_RECORD inputRecord;
    DWORD events;

    Screen screen = Screen(width, height);

    //game ticks per second
    int tickrate = 100;

    int mouseX = 0;
    int mouseY = 0;
    bool mousePressed = false;

    Sim sim;

    while (true) {
        GetNumberOfConsoleInputEvents(hin, &events);

        if (events > 0) {
            ReadConsoleInput(hin, &inputRecord, 1, &events);
            switch (inputRecord.EventType) {
            case MOUSE_EVENT:
                mouseX = inputRecord.Event.MouseEvent.dwMousePosition.X;
                mouseY = inputRecord.Event.MouseEvent.dwMousePosition.Y;

                if (inputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                    mousePressed = true;
                }
                else {
                    mousePressed = false;
                }
                break;
            }
        }

        screen.reset();

        //if (mousePressed) {
        //}
        sim.update();

        sim.show(screen);

        //mouse cursor
        //gridInput(screen, mouseX / 2, mouseY);

        screen.text("Mouse screen pos: x:" + std::to_string(mouseX) + ", y:" + std::to_string(mouseY), 2, 1);
        screen.text("Mouse grid pos: x:" + std::to_string(mouseX / 2) + ", y:" + std::to_string(mouseY), 2, 2);
        screen.text("Bodies: " + std::to_string(sim.count), 2, 3);
        Vector2 collisionA;
        //collisionA.setPolar(cos(thetaA) * bodies[0].vel.getMag(), AtoB.getAngle());
        collisionA = sim.bodies[0].vel;
        screen.text("angle: " + std::to_string(collisionA.getAngle()), 2, 5);
        //screen.text("Swarm target: x:" + std::to_string(swarm.target.x) + ", y:" + std::to_string(swarm.target.y), 2, 4);
        //screen.text("Drone vel mag: " + std::to_string(drone.vel.getMag()), 2, 5);

        screen.print();

        Sleep(1000 / tickrate);
    }
}