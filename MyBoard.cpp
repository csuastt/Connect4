#include "MyBoard.h"
#include <iostream>

// init
MyBoard::MyBoard(const int _M, const int _N, const int *_board,
                const int _noX, const int _noY) : M(_M), N(_N),
                    noX(_noX), noY(_noY)
{
    // init
    for (register int i = 0; i < N; ++i)
        top[i] = M;
    if (noY == M - 1)
        top[noX] = M - 1;
    // place points
    for (register int i = 0; i < N; ++i)
        for (register int j = M - 1; j >= 0; --j)
            if (_board[j * N + i])
                place(i, _board[j * N + i]);
}

int MyBoard::estimateConnection(const int &x)
{
    // the number of connections 2 or 3 
    register int y = (isBanned(x, top[x], noX, noY)) ? 
                            (top[x] + 1) : top[x];
    if (y == M)
        // the line is empty
        return 0;
    
    register int clear, res = 0;
    clear = clr(hor[x], y, 7, 3);
    res += (isConnected3(clear));
    clear = clr(hor[x], y, 5, 1);
    res += isConnected2(clear);
    
    clear = clr(ver[y], x, 7, 3);
    res += (isConnected3(clear));
    clear =  clr(ver[y], x, 5, 1);
    res += isConnected2(clear);

    clear = clr(rig[toRightPos(x, y)], x, 7, 3);
    res += (isConnected3(clear)); 
    clear = clr(rig[toRightPos(x, y)], x, 5, 1);
    res += isConnected2(clear);

    clear = clr(lef[toLeftPos(x, y, M)], x, 7, 3);
    res += (isConnected3(clear)); 
    clear = clr(lef[toLeftPos(x, y, M)], x, 5, 1);
    res += isConnected2(clear);

    return res;
}

int MyBoard::isOk2Place(const int &x)
{
    return (0 <= x && x < N) && (!isFull(x));
}

