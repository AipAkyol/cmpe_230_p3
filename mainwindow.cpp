#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "seconwindow.h"
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
#include <QMessageBox>

using namespace std;
// Load icons

const int N = 10; // row count
const int M = 10; // column count
const int K = 8; // mine count
static int  countNumber = 0;//count

QGridLayout *gridLayout = new QGridLayout;
QHBoxLayout *buttonPart = new QHBoxLayout;

class Square{
private:
    /**/
public:
    bool haveMine;
    bool isOpened;
    bool isFlagged;
    bool isSafeFlagged; // for finding hints, indicates that this square is safe to open
    int neighbourMines; // number of neighbour mines
    bool isHinted;
    Square() {
        this->haveMine = false;
        this->isOpened = false;
        this->isFlagged = false;
        this->isSafeFlagged = false;
        this->neighbourMines = 0;
        this->isHinted = false;
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

void checkWinCondition(){
    if(unopenedCount(board) == K) { // if all unopened squares are mines and user opened all other squares its a win
        printEndBoard(board); // print the board with mines
        QMessageBox gameOverBox;
        gameOverBox.setWindowTitle("minesweeper");
        gameOverBox.setText("You win!");
        gameOverBox.setStandardButtons(QMessageBox::Ok);
        gameOverBox.setIcon(QMessageBox::Information);
        gameOverBox.exec();

    }
}
void printEndBoard(Square board[N][M]) {
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap buttonMine = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/mine.png").scaled(iconSize);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(board[i][j].haveMine) {
                qobject_cast<QPushButton*>(gridLayout->itemAtPosition(i,j)->widget())->setIcon(buttonMine);
            }
        }
    }
}
int unopenedCount(Square board[N][M]) {
    int count = 0;
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(!board[i][j].isOpened) {
                count++;
            }
        }
    }
    return count;
}

void gameOver(){
    printEndBoard(board);
    QMessageBox gameOverBox;
    gameOverBox.setWindowTitle("minesweeper");
    gameOverBox.setText("You lose!");
    gameOverBox.setStandardButtons(QMessageBox::Ok);
    gameOverBox.setIcon(QMessageBox::Information);
    gameOverBox.exec();
}
void buttonLeftClicked(int i , int j){
    if(board[i][j].haveMine){
        gameOver();
    }else{
       openSquare(board ,i ,j);
    }
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
void initializeBoard(QGridLayout* layout){
    QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
    QPixmap buttonIcon = QPixmap(iconPath);
    QSize iconSize = buttonIcon.size() * 2;
    QPixmap buttonEmpty = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/empty.png").scaled(iconSize);
    // Initialize the board
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            board[i][j] = Square();
            qobject_cast<QPushButton*>(layout->itemAtPosition(i,j)->widget())->setIcon(buttonEmpty);
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
    countNumber++;
    QLabel* label = qobject_cast<QLabel*>(buttonPart->itemAt(0)->widget());
    std::string text = "Score: ";
    int number = countNumber;
    std::string result = text + std::to_string(number);
    label->setText(QString("Score: %1").arg(countNumber));
    if(board[x][y].neighbourMines==0){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button0);
    }
    if(board[x][y].neighbourMines==1){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button1);
    }
    if(board[x][y].neighbourMines==2){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button2);
    }
    if(board[x][y].neighbourMines==3){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button3);
    }
    if(board[x][y].neighbourMines==4){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button4);
    }
    if(board[x][y].neighbourMines==5){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button5);
    }
    if(board[x][y].neighbourMines==6){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button6);
    }
    if(board[x][y].neighbourMines==7){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button7);
    }
    if(board[x][y].neighbourMines==8){
        qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(button8);
    }
    if(board[x][y].neighbourMines == 0) { // if there are no neighbour mines, use bfs to open neighbours
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                openSquare(board, x + dx, y + dy); // open neighbours
            }
        }
    }
    checkWinCondition();
}



// find hint function that finds a safe square to open
tuple<int, int> findHint(Square board[N][M]) {
    Square boardCopy[N][M];

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            // copy the board, but don't copy the flags since user may have flagged some squares wrongly
            boardCopy[i][j] = Square();
            boardCopy[i][j].haveMine = board[i][j].haveMine;
            boardCopy[i][j].isOpened = board[i][j].isOpened; // current state is opened
            boardCopy[i][j].neighbourMines = board[i][j].neighbourMines;
        }
    }
    bool newInfo = false; // boolean to check if new information is found; new information is a square that is safe or flagged
    bool foundHint = false; // boolean to check if a hint is found
    tuple<int, int> hint = make_tuple(-1, -1); // default hint for no hint
    tuple<int, int> hintneigbour; // hint neighbour coordinates
    do {
        newInfo = false; // reset new information
        for(int i = 0; i < N; i++) {
            for(int j = 0; j < M; j++) {
                if(boardCopy[i][j].isOpened && boardCopy[i][j].neighbourMines > 0) {
                    int flaggedNeighboursCount = countFlaggedNeighbours(boardCopy, i, j); // count flagged neighbours
                    // if flagged neighbours count is equal to neighbour mines, all other neighbours are safe
                    if(flaggedNeighboursCount == boardCopy[i][j].neighbourMines) {
                        bool safeflagInfo = safeFlagNeighbours(boardCopy, i, j); // flag other neighbours as safe, and if any safe flag is found, set safeflagInfo to true
                        if (safeflagInfo) { // if any safe flag is found, set foundHint to true and set hintneigbour
                            foundHint = true;
                            hintneigbour = make_tuple(i, j);
                        }
                        if (!newInfo && safeflagInfo) { // if new information is found, set newInfo to true
                            newInfo = true;
                        }
                    }
                    int noInfoNeighboursCount = countNoInfoNeighbours(boardCopy, i, j); // count no info neighbours
                    // if no info neighbours count is equal to neighbour mines - flagged neighbours, all other neighbours are flagged
                    if(noInfoNeighboursCount == boardCopy[i][j].neighbourMines - flaggedNeighboursCount) {
                        bool flagInfo = flagNeighbours(boardCopy, i, j); // flag other neighbours, and if any flag is found, set flagInfo to true
                        if (!newInfo && flagInfo) { // if new information is found, set newInfo to true
                            newInfo = true;
                        }
                    }
                }
            }
        }
    } while(newInfo); // repeat until no new information is found

    if (foundHint) {
        //loop through the neighbours of the hintneigbour to find a hint
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                // bounds check
                if (get<0>(hintneigbour) + dx >= 0 && get<0>(hintneigbour) + dx < N && get<1>(hintneigbour) + dy >= 0 && get<1>(hintneigbour) + dy < M) {
                    // if a neighbour is not opened, not flagged and safe flagged, it is a hint
                    if (!boardCopy[get<0>(hintneigbour) + dx][get<1>(hintneigbour) + dy].isOpened
                        && !boardCopy[get<0>(hintneigbour) + dx][get<1>(hintneigbour) + dy].isFlagged
                        && boardCopy[get<0>(hintneigbour) + dx][get<1>(hintneigbour) + dy].isSafeFlagged) {
                        hint = make_tuple(get<0>(hintneigbour) + dx, get<1>(hintneigbour) + dy);
                        break;
                    }
                }
            }
        }
    }
    return hint;
}

// count flagged neighbours of a square
int countFlaggedNeighbours(Square board[N][M], int x, int y) {
    int count = 0;
    // loop through neighbours
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            // bounds check
            if(x + dx >= 0 && x + dx < N && y + dy >= 0 && y + dy < M) {
                // if neighbour is flagged and not opened, increment count
                if(board[x + dx][y + dy].isFlagged
                    && !board[x + dx][y + dy].isOpened) {
                    count++;
                }
            }
        }
    }
    return count;
}

// count neighbours that are not opened, not flagged and not safe flagged
int countNoInfoNeighbours(Square board[N][M], int x, int y) {
    int count = 0;
    // loop through neighbours
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            // bounds check
            if(x + dx >= 0 && x + dx < N && y + dy >= 0 && y + dy < M) {
                // if neighbour is not opened, not flagged and not safe flagged, increment count
                if(!board[x + dx][y + dy].isOpened
                    && !board[x + dx][y + dy].isFlagged
                    && !board[x + dx][y + dy].isSafeFlagged) {
                    count++;
                }
            }
        }
    }
    return count;
}

// flag neighbours of a square
bool flagNeighbours(Square board[N][M], int x, int y) {
    bool newInfo = false;
    // loop through neighbours
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            // bounds check
            if(x + dx >= 0 && x + dx < N && y + dy >= 0 && y + dy < M) {
                // if neighbour is not opened, not safe flagged and not flagged, flag the neighbour
                if(!board[x + dx][y + dy].isOpened
                    && !board[x + dx][y + dy].isSafeFlagged
                    && !board[x + dx][y + dy].isFlagged) {
                    board[x + dx][y + dy].isFlagged = true; // flag the neighbour
                    newInfo = true; // set new information to true, since new flag is found
                }
            }
        }
    }
    return newInfo;
}

// safe flag neighbours of a square
bool safeFlagNeighbours(Square board[N][M], int x, int y) {
    bool newInfo = false;
    // loop through neighbours
    for(int dx = -1; dx <= 1; dx++) {
        for(int dy = -1; dy <= 1; dy++) {
            //  bounds check
            if(x + dx >= 0 && x + dx < N && y + dy >= 0 && y + dy < M) {
                // if neighbour is not opened, not flagged and safe flagged, safe flag the neighbour
                if(!board[x + dx][y + dy].isOpened
                    && !board[x + dx][y + dy].isFlagged
                    && !board[x + dx][y + dy].isSafeFlagged) {
                    board[x + dx][y + dy].isSafeFlagged = true; // safe flag the neighbour
                    newInfo = true; // set new information to true, since new safe flag is found
                }
            }
        }
    }
    return newInfo;
}
void hintMaker(){
    tuple<int, int> hint = findHint(board); // call findHint function
    if (get<0>(hint) == -1 && get<1>(hint) == -1) { // if no hint found function returns -1 -1
        return;
    }
    else {
        int x = get<0>(hint);
        int y = get<1>(hint);
        if (board[x][y].isHinted){
            openSquare(board,x,y);
            board[x][y].isHinted = false;
        }else{
            QString iconPath = QString("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png");
            QPixmap buttonIcon = QPixmap(iconPath);
            QSize iconSize = buttonIcon.size() * 2;
            QPixmap buttonHint = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/hint.png").scaled(iconSize);
            qobject_cast<QPushButton*>(gridLayout->itemAtPosition(x,y)->widget())->setIcon(buttonHint);
            board[x][y].isHinted = true;
        }

    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Create a central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);



    // Create and add label to the buttonPart layout
    QLabel *label = new QLabel("Score: 0", centralWidget);
    buttonPart->addWidget(label);

    // Create and add buttons to the buttonPart layout
    QPushButton *restartButton = new QPushButton("Restart", centralWidget);
    connect(restartButton,&QPushButton::clicked,this,[=]() {
        initializeBoard(gridLayout);
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
    QPixmap buttonEmpty = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/empty.png").scaled(iconSize);
    /*
    QPixmap button0 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/0.png").scaled(iconSize);
    QPixmap button1 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/1.png").scaled(iconSize);
    QPixmap button2 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/2.png").scaled(iconSize);
    QPixmap button3 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/3.png").scaled(iconSize);
    QPixmap button4 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/4.png").scaled(iconSize);
    QPixmap button5 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/5.png").scaled(iconSize);
    QPixmap button6 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/6.png").scaled(iconSize);
    QPixmap button7 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/7.png").scaled(iconSize);
    QPixmap button8 = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/8.png").scaled(iconSize);

    QPixmap buttonMine = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/mine.png").scaled(iconSize);
    QPixmap buttonFlag = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/flag.png").scaled(iconSize);
    QPixmap buttonWrongFlag = QPixmap("C:/Users/Mete Damar/Qt_Projects/deneme/assets/wrong-flag.png").scaled(iconSize);
*/

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
    initializeBoard(gridLayout);

}


MainWindow::~MainWindow()
{
    delete ui;
}
