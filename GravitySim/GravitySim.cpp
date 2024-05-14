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
    Coord clickedPos = { -1, -1 };
    Coord draggedPos = { -1, -1 };

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
        
        if (mouseClicked) {
            clickedPos = { mouseX / 2, mouseY };
        }
        else if (mousePressed) {
            draggedPos = { mouseX / 2, mouseY };
            
            //screen.gridInput(clickedPos.x, clickedPos.y);
            screen.line(clickedPos.x, clickedPos.y, draggedPos.x, draggedPos.y);
            screen.circle(clickedPos.x, clickedPos.y, (int)round(sqrt(pow(draggedPos.x - clickedPos.x, 2) + pow(draggedPos.y - clickedPos.y, 2))));
        }
        else if (mouseReleased) {
            sim.addBody(50, (int)round(sqrt(pow(draggedPos.x - clickedPos.x, 2) + pow(draggedPos.y - clickedPos.y, 2))), clickedPos.x, clickedPos.y);
            clickedPos = { -1, -1 };
            draggedPos = { -1, -1 };
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