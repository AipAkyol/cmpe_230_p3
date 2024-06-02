#include <iostream>
#include <string>

using namespace std;

class Square{
    private:
        /**/
    public:
        bool haveMine;
        bool isOpened;
        bool isFlagged;
        /*bool isQuestioned;*/
        int neighbourMines;
        Square(bool haveMine) {
            this->haveMine = haveMine;
            this->isOpened = false;
            this->isFlagged = false;
            /*this->isQuestioned = false;*/
            this->neighbourMines = 0;
        }
};

