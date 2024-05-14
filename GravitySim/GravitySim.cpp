#include <iostream>
#include <windows.h>
#include "Screen.h"
#include "Sim.h"

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
    bool mouseClicked = false;
    bool mousePressed = false;
    bool mouseReleased = false;

    // 0:standby, 1:creation, 2:addingVelocity
    int editorState = 0;
    Coord centrePos = { -1, -1 };
    int radius = 0;
    // resulting vector magnitude / distance in pixels
    float velScaling = 1.0 / 10.0f;

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
                    if (!mousePressed) {
                        mouseClicked = true;
                    }
                    else {
                        mouseClicked = false;
                    }
                    
                    mousePressed = true;
                    mouseReleased = false;
                }
                else if(mousePressed && !mouseReleased) {
                    mouseClicked = false;
                    mousePressed = false;
                    mouseReleased = true;
                }
                else {
                    mouseClicked = false;
                    mousePressed = false;
                    mouseReleased = false;
                }
                break;
            }
        }

        screen.reset();
        
        //editor logic
        switch (editorState) {
        case 0:
            if (mouseClicked) {
                centrePos = { mouseX / 2, mouseY };
                editorState = 1;
            }
            break;
        case 1:
            radius = (int)round(sqrt(pow((mouseX / 2) - centrePos.x, 2) + pow(mouseY - centrePos.y, 2)));
            screen.circle(centrePos.x, centrePos.y, radius);
            screen.line(centrePos.x, centrePos.y, (mouseX / 2), mouseY);

            if (mouseReleased) {
                editorState = 2;
            }
            break;
        case 2:
            screen.circle(centrePos.x, centrePos.y, radius);
            screen.line(centrePos.x, centrePos.y, (mouseX / 2), mouseY);

            if (mouseReleased) {
                Vector2 vel = Vector2((mouseX / 2) - centrePos.x, mouseY - centrePos.y);
                vel.multiply(velScaling);

                sim.addBody(centrePos.x, centrePos.y, radius, vel);
                radius = 0;
                centrePos = { -1, -1 };

                editorState = 0;
            }
            break;
        }
        
        


        sim.update();

        sim.show(screen);

        //mouse cursor
        //gridInput(screen, mouseX / 2, mouseY);

        //circle(screen, 100, 100, 20);

        screen.print();

        Sleep(1000 / tickrate);
    }
}