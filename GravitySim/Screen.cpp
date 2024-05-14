#include "Screen.h"
#include <iostream>

Screen::Screen() {
    width = 0;
    height = 0;
    screenMatrix = nullptr;
    matrixSize = 0;
}

Screen::Screen(int width, int height) {
    this->width = width;
    this->height = height;
    matrixSize = (width + 1) * height;

    screenMatrix = new char[matrixSize];
    for (int i = 0; i < matrixSize; i++) {
        if (i % (this->width+1) == this->width) {
            screenMatrix[i] = '\n';
            continue;
        }

        screenMatrix[i] = ' ';
    }
    screenMatrix[matrixSize - 1] = 0;
}

Screen::~Screen() {
    delete[] screenMatrix;
}

Screen& Screen::operator=(const Screen& screen) {
    delete[] screenMatrix;

    width = screen.width;
    height = screen.height;
    matrixSize = screen.matrixSize;

    screenMatrix = new char[matrixSize];
    for (int i = 0; i < matrixSize; i++) {
        screenMatrix[i] = screen.screenMatrix[i];
    }
    screenMatrix[matrixSize - 1] = 0;

    return *this;
}

void Screen::reset() {
    for (int i = 0; i < matrixSize; i++) {
        if (i % (width + 1) == width) {
            screenMatrix[i] = '\n';
            continue;
        }

        screenMatrix[i] = ' ';
    }
    screenMatrix[matrixSize - 1] = 0;
}

void Screen::input(char text, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    screenMatrix[x + y * (width+1)] = text;
}

void Screen::rect(char text, int x, int y, int width, int height) {
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            input(text, i, j);
        }
    }
}

void Screen::text(std::string text, int x, int y) {
    for (int i = 0, j = 0, k = 0; i < text.length(); i++, j++) {
        if (text[i] == '\n') {
            j = -1;
            k++;
            continue;
        }
        input(text[i], x + j, y + k);
    }
}

Coord Screen::mapScreenToGrid(int x, int y) {
    return { x / 2, y };
}

void Screen::gridInput(int gridX, int gridY) {
    input(-37, gridX * 2, gridY);
    input(-37, gridX * 2 + 1, gridY);
}

void Screen::line(int x1, int y1, int x2, int y2) {
    float gradient = (float)(y2 - y1) / (x2 - x1);

    int lengthX = abs(x2 - x1);
    int lengthY = abs(y2 - y1);

    Coord leftPos;
    if (x1 < x2) {
        leftPos = { x1, y1 };
    }
    else {
        leftPos = { x2, y2 };
    }

    Coord topPos;
    if (y1 < y2) {
        topPos = { x1, y1 };
    }
    else {
        topPos = { x2, y2 };
    }

    int minX;
    if (x1 < x2) {
        minX = x1;
    }
    else {
        minX = x2;
    }

    int minY;
    if (y1 < y2) {
        minY = y1;
    }
    else {
        minY = y2;
    }

    if (lengthX == 0) {
        for (int i = 0; i < lengthY + 1; i++) {
            //screen.input(chr, x1, minY + i);
            gridInput(x1, minY + i);
        }
        return;
    }

    if (lengthY == 0) {
        for (int i = 0; i < lengthX; i++) {
            //screen.input(chr, minX + i, y1);
            gridInput(minX + i, y1);
        }
        return;
    }

    if (abs(gradient) < 1) {
        for (int i = 0; i < lengthX + 1; i++) {
            //screen.input(chr, leftPos.x + i, leftPos.y + round(i * gradient));
            gridInput(leftPos.x + i, leftPos.y + round(i * gradient));
        }
        return;
    }

    for (int i = 0; i < lengthY + 1; i++) {
        //screen.input(chr, topPos.x + round(i * (1 / gradient)), topPos.y + i);
        gridInput(topPos.x + round(i * (1 / gradient)), topPos.y + i);
    }
}

void Screen::circle(int x, int y, int r) {
    // x^2 + y^2 = r^2
    // y = sqrt(r^2 - x^2)

    for (int i = -r; i < r; i++) {
        gridInput(x + i, y + round(sqrt(pow(r, 2) - pow(i, 2))));
    }
    for (int i = -r; i < r; i++) {
        gridInput(x + round(sqrt(pow(r, 2) - pow(i, 2))), y + i);
    }
    for (int i = -r; i < r; i++) {
        gridInput(x + i, y - round(sqrt(pow(r, 2) - pow(i, 2))));
    }
    for (int i = -r; i < r; i++) {
        gridInput(x - round(sqrt(pow(r, 2) - pow(i, 2))), y + i);
    }
}

void Screen::print() {
    std::cout << "\x1b[?25l\x1b[1;1H";
    //std::cout << "\x1b[38;2;255;255;0m";

    std::cout << screenMatrix;
}