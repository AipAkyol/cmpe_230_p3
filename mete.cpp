#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "custompushbutton.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QObject>
#include <QContextMenuEvent>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;
// Load icons

const int N = 10; // row count
const int M = 10; // column count
const int K = 8; // mine count


QGridLayout *gridLayout = new QGridLayout;

class Square{
private:
    /**/
public:
    bool haveMine;
    bool isOpened;
    bool isFlagged;
    bool isSafeFlagged; // for finding hints, indicates that this square is safe to open
    int neighbourMines; // number of neighbour mines
    Square() {
        this->haveMine = false;
        this->isOpened = false;
        this->isFlagged = false;
        this->isSafeFlagged = false;
        this->neighbourMines = 0;
    }
};
// 2D array of squares
Square board[N][M];
// implicitly declared functions, they are defined below main function
void printEndBoard(Square board[N][M]); // this is for open board
void printBoard(Square board[N][M]); // this is for closed board
void openSquare(Square board[N][M], int x, int y); // this is for opening squares
tuple<int, int> findHint(Square board[N][M]); // this is for finding a hint
int countFlaggedNeighbours(Square board[N][M], int x, int y); // this is for counting flagged neighbours
int countNoInfoNeighbours(Square board[N][M], int x, int y); // this is for counting no info neighbours
bool flagNeighbours(Square board[N][M], int x, int y); // this is for flagging neighbours
bool safeFlagNeighbours(Square board[N][M], int x, int y); // this is for safe flagging neighbours
int unopenedCount(Square board[N][M]); // this is for counting unopened squares
void buttonLeftClicked(int i , int j){
    ;
}
void buttonRightClicked(QPushButton* button,int i , int j){
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap buttonEmpty = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/empty.png").scaled(iconSize);
    QPixmap buttonFlag = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/flag.png").scaled(iconSize);
    if(board[i][j].isFlagged){
        button->setIcon(buttonEmpty);
        board[i][j].isFlagged = false;
    }else{
        button->setIcon(buttonFlag);
        board[i][j].isFlagged = true;
    }
}
void initializeBoard(){
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap buttonEmpty = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/empty.png").scaled(iconSize);
    // Initialize the board
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            board[i][j] = Square();
            QWidget* button = gridLayout->itemAtPosition(i,j)->widget();
            ((QPushButton*)button)->setIcon(buttonEmpty);
        }
    }
    srand(time(0));
    // Place mines randomly, don't overlap mines
    for(int i = 0; i < K; i++) {
        int x, y;
        do {
            //randomly select a square using computers time
            x = rand() % N;
            y = rand() % M;
        } while(board[x][y].haveMine); // check if there is already a mine
        board[x][y].haveMine = true; // place mine
    }

    // Calculate neighbour mines
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(board[i][j].haveMine) {
                continue; // skip if it is a mine
            }
            for(int dx = -1; dx <= 1; dx++) {
                for(int dy = -1; dy <= 1; dy++) {
                    // bounds check, note that dx = 0 and dy = 0 is the current square and it is not a mine
                    // so neighbour mines are not incremented for the current square
                    if(i + dx >= 0 && i + dx < N && j + dy >= 0 && j + dy < M) {
                        if(board[i + dx][j + dy].haveMine) { // if neighbour has mine
                            board[i][j].neighbourMines++; // increment neighbour mine count for this square
                        }
                    }
                }
            }
        }
    }
}
void openSquare(Square board[N][M], int x, int y) {
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap button0 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/0.png").scaled(iconSize);
    QPixmap button1 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/1.png").scaled(iconSize);
    QPixmap button2 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/2.png").scaled(iconSize);
    QPixmap button3 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/3.png").scaled(iconSize);
    QPixmap button4 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/4.png").scaled(iconSize);
    QPixmap button5 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/5.png").scaled(iconSize);
    QPixmap button6 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/6.png").scaled(iconSize);
    QPixmap button7 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/7.png").scaled(iconSize);
    QPixmap button8 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/8.png").scaled(iconSize);
    if(x < 0 || x >= N || y < 0 || y >= M) {
        return; // base case for bounds check
    }
    if(board[x][y].isOpened) {
        return; // base case for bfs
    }
    board[x][y].isOpened = true; // open the square
    //if(board[x][y].neighbourMines==0){
    //    button->setIcon(buttonEmpty);
    //}
    if(board[x][y].neighbourMines == 0) { // if there are no neighbour mines, use bfs to open neighbours
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                openSquare(board, x + dx, y + dy); // open neighbours
            }
        }
    }
}

// print the board with mines
void printEndBoard(Square board[N][M]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(board[i][j].haveMine) {
                cout << "X ";
            } else {
                cout << board[i][j].neighbourMines << " ";
            }
        }
        cout << endl;
    }
}

void hintMaker(){
    printEndBoard(board);
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeBoard();

    // Create a central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *buttonPart = new QHBoxLayout;


    // Create and add label to the buttonPart layout
    QLabel *label = new QLabel("Score: 0", centralWidget);
    buttonPart->addWidget(label);

    // Create and add buttons to the buttonPart layout
    QPushButton *restartButton = new QPushButton("Restart", centralWidget);
    connect(restartButton,&QPushButton::clicked,this,[=]() {
        initializeBoard();
    });
    buttonPart->addWidget(restartButton);

    QPushButton *hintButton = new QPushButton("Hint", centralWidget);
    connect(hintButton,&QPushButton::clicked,this,hintMaker);
    buttonPart->addWidget(hintButton);

    // Add buttonPart layout to the main layout
    mainLayout->addLayout(buttonPart);
    mainLayout->addStretch();
    mainLayout->addLayout(gridLayout);
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap button0 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/0.png").scaled(iconSize);
    QPixmap button1 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/1.png").scaled(iconSize);
    QPixmap button2 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/2.png").scaled(iconSize);
    QPixmap button3 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/3.png").scaled(iconSize);
    QPixmap button4 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/4.png").scaled(iconSize);
    QPixmap button5 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/5.png").scaled(iconSize);
    QPixmap button6 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/6.png").scaled(iconSize);
    QPixmap button7 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/7.png").scaled(iconSize);
    QPixmap button8 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/8.png").scaled(iconSize);
    QPixmap buttonEmpty = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/empty.png").scaled(iconSize);
    QPixmap buttonMine = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/mine.png").scaled(iconSize);
    QPixmap buttonFlag = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/flag.png").scaled(iconSize);
    QPixmap buttonWrongFlag = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/wrong-flag.png").scaled(iconSize);


    int buttonSize = 30; // Button size

    // Create and add push buttons to the grid layout
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            QPushButton *button = new QPushButton(centralWidget);
            button->setFixedSize(buttonSize, buttonSize); // Ensure the button is square

            if (!buttonIcon.isNull()) {
                button->setIcon(buttonEmpty);
                button->setIconSize(iconSize);
            } else {
                qWarning() << "Icon not found:" << iconPath;
                button->setText(QString("Button %1,%2").arg(i + 1).arg(j + 1)); // Fallback text
            }

            // Connect the button's clicked signal to a slot
            connect(button, &QPushButton::clicked, this, [=]() {
                buttonLeftClicked(i,j);
            });
            button->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(button, &QPushButton::customContextMenuRequested, this, [=]() {
                buttonRightClicked(button , i, j);
            });

            // Add the button to the grid layout
            gridLayout->addWidget(button, i, j);
        }
    }

}


MainWindow::~MainWindow()
{
    delete ui;
}
