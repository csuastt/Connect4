// MCTS class
#if !defined(MCTS_H)
#define MCTS_H

#include "Parameters.h"
#include "MyBoard.h"


struct Situation
{
    // next situations after moving
    // content: index in the global 
    int next[MAX_SIZE];
    // exploring times of next situs
    int num_expl[MAX_SIZE];
    // winning times of next situs
    int num_win[MAX_SIZE];
    // exploring times of the situ
    int tot_num_expl;
};


class MCTS
{
private:
    // size
    int M, N;
    // last action
    int last_action;
    // situations
    Situation *situs;
    // board
    MyBoard* board;
    // player win
    int player_win[3];
    // candidate choices of place
    int choice[MAX_SIZE];
    // number of choice
    int n_choice;
public:
    int num_situs;
    MCTS(MyBoard *start_board);
    // Selection
    inline int select(const int &cur_situ);
    // Entry
    int run(const int &cur_situ, const int &player);
    // Get action
    int getAction();
    // Check if someplace to win
    inline void checkWinPlaceRandom(const int &player);
    inline void checkWinPlaceExplore(const int &player, const int &cur_situ);
    // Simulation
    inline int simulate(const int &player);
};

#endif // MCTS_H
