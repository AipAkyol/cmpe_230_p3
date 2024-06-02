#include <iostream>
#include <string>
#include <tuple>

using namespace std;

const int N = 5; // row count
const int M = 5; // column count
const int K = 3; // mine count

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

int main() {
    
    // 2D array of squares
    Square board[N][M];
    
    // Initialize the board
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            board[i][j] = Square();
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

    printBoard(board); // print the board

    // Game loop
    while(true) {
        int x, y; // row and column input
        cout << "Enter row and column: ";
        cin >> x >> y; // get row and column from user
        if (x == -1 && y == -1) { // if user wants a hint, -1 -1 is the hint command
            tuple<int, int> hint = findHint(board); // call findHint function
            if (get<0>(hint) == -1 && get<1>(hint) == -1) { // if no hint found function returns -1 -1
                cout << "No hint found!" << endl;
            } else {
                cout << "Hint: " << get<0>(hint) << " " << get<1>(hint) << endl; // print the hint
            }
            continue;
        }
        if(board[x][y].haveMine) { // if user opened a square with mine
            cout << "Game over!" << endl; // print game over
            printEndBoard(board); // print the board with mines
            break;
        }
        openSquare(board, x, y); // open the square
        if(unopenedCount(board) == K) { // if all unopened squares are mines and user opened all other squares its a win
            cout << "You win!" << endl;
            printEndBoard(board); // print the board with mines
            break;
        }
        printBoard(board); // print the board after user move
    }



}

// open square function that uses bfs to open neighbours if there are no neighbour mines
void openSquare(Square board[N][M], int x, int y) {
    if(x < 0 || x >= N || y < 0 || y >= M) {
        return; // out of bounds, not necessary but just in case
    }
    if(board[x][y].isOpened) {
        return; // already opened, not necessary but just in case
    }
    board[x][y].isOpened = true; // open the square
    if(board[x][y].neighbourMines == 0) { // if there are no neighbour mines, use bfs to open neighbours
        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                openSquare(board, x + dx, y + dy); // open neighbours
            }
        }
    }
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

// finds the number of unopened squares
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


// print the board
void printBoard(Square board[N][M]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            if(board[i][j].isOpened) {
                if(board[i][j].haveMine) {
                    cout << "X ";
                } else {
                    cout << board[i][j].neighbourMines << " ";
                }
            } else {
                if(board[i][j].isFlagged) {
                    cout << "F ";
                } else {
                    cout << "= ";
                }
            }
        }
        cout << endl;
    }
}


