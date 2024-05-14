#pragma once
#include <string>

struct Coord {
    int x;
    int y;
};

class Screen {
    private:    
        char* screenMatrix;
        int matrixSize;
    
    public:
        int width;
        int height;

        Screen();

        Screen(int width, int height);

        ~Screen();

        Screen& operator=(const Screen& screen);

        void reset();

        void input(char text, int x, int y);

        void rect(char text, int x, int y, int width, int height);

        void text(std::string text, int x, int y);

        static Coord mapScreenToGrid(int x, int y);

        void gridInput(int gridX, int gridY);

        void line(int x1, int y1, int x2, int y2);

        void circle(int x, int y, int r);

        void print();
};