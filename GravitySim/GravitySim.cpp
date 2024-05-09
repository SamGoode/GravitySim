#include <iostream>
#include <windows.h>
#include "Screen.h"

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
        gridInput(screen, round(pos.x), round(pos.y) - 1);
        gridInput(screen, round(pos.x) + 1, round(pos.y));
        gridInput(screen, round(pos.x), round(pos.y) + 1);
        gridInput(screen, round(pos.x) - 1, round(pos.y));

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
        bodies[0] = Body(10, 50, 50);
        bodies[0].vel.set(0, 1).setMag(0.1);
        bodies[1] = Body(10, 100, 50);
        bodies[1].vel.set(0, -1).setMag(0.1);
    }

    Sim& update() {
        float gConstant = 1;
        float distance = Vector2(bodies[1].pos).sub(bodies[0].pos).getMag();
        float gForceMag = gConstant * (bodies[1].mass * bodies[0].mass)/pow(distance, 2);
        
        Vector2 gForce;
        gForce.setPolar(gForceMag, Vector2(bodies[1].pos).sub(bodies[0].pos).getAngle());
        gForce.divide(2);

        bodies[0].acc = Vector2(gForce).divide(bodies[0].mass);
        bodies[1].acc = Vector2(gForce).multiply(-1).divide(bodies[1].mass);

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

    SHORT width = 300;
    SHORT height = 100;

    SetConsoleScreenBufferSize(hout, { width, height });

    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

    GetCurrentConsoleFontEx(hout, false, &cfi);

    cfi.dwFontSize.Y = 10;
    cfi.dwFontSize.X = 5;

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
        //screen.text("Swarm target: x:" + std::to_string(swarm.target.x) + ", y:" + std::to_string(swarm.target.y), 2, 4);
        //screen.text("Drone vel mag: " + std::to_string(drone.vel.getMag()), 2, 5);

        screen.print();

        Sleep(1000 / tickrate);
    }
}