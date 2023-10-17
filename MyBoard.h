// Board Class for MCTS
#if !defined(MYBOARD_H)
#define MYBOARD_H

#include "Parameters.h"
#include <cstring>

// if the place is banned
#define isBanned(x, y, noX, noY) (x == noX && y == noY)
// pos index conversion
#define toRightPos(x, y) (x + y)
#define toLeftPos(x, y, M) (x - y + M) 
// judge if the connections occur
#define isConnected(arr) ((arr & (arr >> 2) & (arr >> 4) & (arr >> 6)) ? 1 : 0)
#define isConnected3(arr) ((arr & (arr >> 2) & (arr >> 4)) ? 1 : 0)
#define isConnected2(arr) ((arr & (arr >> 2)) ? 1 : 0)
#define clr(data, val, off1, off2) ((data & ((1 << ((val << 1) + off1)) - 1)) >> (std::max(0, ((val << 1) - off2))))


class MyBoard
{
private:
    // some parameters
    int M, N;
    int top[MAX_SIZE];
    int noX, noY;
    // the connections in:
    // horizontal, vertical
    // left-up-right-down
    // right-up-left-down
    int hor[MAX_SIZE] = {}, ver[MAX_SIZE] = {};
    int rig[MAX_SIZE * 3] = {}, lef[MAX_SIZE * 3] = {};

public:
    MyBoard(const int M, const int N, const int *_board, 
            const int noX, const int noY);
    ~MyBoard() {}
    // user functions
    // make a move
    inline void place(const register int next_x, const register int val)
    {
        register int next_y = top[next_x] - 1;
        register int delta_x = (1 << ((next_x << 1) + val));
        // update connections array
        hor[next_x] |= (1 << ((next_y << 1) + val));
        ver[next_y] |= delta_x;
        rig[toRightPos(next_x, next_y)] |= delta_x;
        lef[toLeftPos(next_x, next_y, M)] |= delta_x;
        // update top
        --top[next_x];
        // skip the banned area
        if (isBanned(next_x, (top[next_x] - 1), noX, noY))
            --top[next_x];
    }
    // retreat last move
    inline void remove(const int register x, const int register val)
    {
        if (isBanned(x, top[x], noX, noY))
            ++top[x];
        register int preY = top[x];
        register int delta_x = (1 << ((x << 1) + val));
        hor[x] -= (1 << ((preY << 1) + val));
        ver[preY] -= delta_x;
        lef[toLeftPos(x, preY, M)] -= delta_x;
        rig[toRightPos(x, preY)] -= delta_x;
        ++top[x];
    }
    // judgement
    // If A makes a move, call isEnd() first:
    //  if the game is over, A wins;
    //  else call isFull();
    // The game isn't over if and only if both return 0
    // After placing at x, if the game is over 
    inline int isEnd(const int register x)
    {
        // if the connections are fulfilled
        register int bound = (isBanned(x, top[x], noX, noY)) ? 
                                (top[x] + 1) : top[x];
        if (bound == M)
            // the line is empty
            return 0;
        return isConnected(hor[x]) || isConnected(ver[bound]) ||
            isConnected(rig[toRightPos(x, bound)]) ||
            isConnected(lef[toLeftPos(x, bound, M)]);
    }
    // After placing at x, if the line is full
    inline int isFull(const int register x) {return top[x] == 0;};
    // if the line is empty
    inline int isEmpty(const int &x) {return top[x] == M;}
    // if is ok to make a move at the pos
    inline int isOk2Place(const int &x);
    // Init board
    inline void initBoard(const MyBoard *copy_board)
    {
        for (register int i = 0; i < MAX_SIZE; ++i)
        {
            top[i] = copy_board->top[i];
            hor[i] = copy_board->hor[i];
            ver[i] = copy_board->ver[i];
        }
        for (register int i = 0; i < MAX_SIZE * 3; ++i)
        {
            rig[i] = copy_board->rig[i];
            lef[i] = copy_board->lef[i];
        }
    }
    // get estimated connections
    int estimateConnection(const int &x);
    // getters
    inline int getM() {return M;};
    inline int getN() {return N;};
    inline int getNoX() {return noX;};
    inline int getNoY() {return noY;};
    inline int getTop(const int &x) {return top[x];};
};

#endif // MYBOARD_H
