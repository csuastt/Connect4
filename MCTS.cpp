#include "MCTS.h"
#include <cstring>
#include <random>

Situation g_situs[MAX_SITU];

// calculate UCT
#define getUCT(cur_situ, x) \
    ((double)situs[cur_situ].num_win[x] / situs[cur_situ].num_expl[x] + \
        C * sqrt(2 * log(situs[cur_situ].tot_num_expl) / \
            situs[cur_situ].num_expl[x]))

MCTS::MCTS(MyBoard *start_board): M(start_board->getM()), N(start_board->getN()), 
    situs(g_situs),board(start_board),num_situs(0)
{
    std::memset(g_situs, 0, sizeof(g_situs));
}

// @brief the entry of MCTS
// @param situ_index start from which situ
// @param player which player to move on this situ (1 or 2)
// @param last_action  last action (-1 means no last action)
// @return win or lost (1-win, 0-lost)
int MCTS::run(const int &cur_situ, const int &player)
{
    // game over
    if (cur_situ && board->isEnd(last_action))
        return 0;
    
    // judge if someplace is to win
    int next_act = -1;
    player_win[1] = 0;
    player_win[2] = 0;
    checkWinPlaceExplore(player, cur_situ);
    if (player_win[player])
        next_act = player_win[player] % (N + 1) - 1;
    else if (player_win[3 - player] >= N + 1)
        return 0;
    else if (player_win[3 - player])
        next_act = player_win[3 - player] - 1;

    // select a place 
    if (next_act == -1)
        next_act = select(cur_situ);

    // case of tie
    if (next_act == -1)
        return rand() & 1;
    
    // update
    int res;
    board->place(next_act, player);
    last_action = next_act;
    ++situs[cur_situ].tot_num_expl;
    ++situs[cur_situ].num_expl[next_act];
    if (!situs[cur_situ].next[next_act])
    {
        ++num_situs;
        situs[cur_situ].next[next_act] = num_situs;
        // simulation
        res = simulate(3 - player);
    }
    else
    {
        // recurse
        res = run(situs[cur_situ].next[next_act], 3 - player);
    }

    if (!res)
        ++situs[cur_situ].num_win[next_act];
    
    // back track
    // board->remove(next_act, player);
    return 1 - res;
}

void MCTS::checkWinPlaceRandom(const int &player)
{
    n_choice = 0;

    for (register int i = 0; i < N; ++i)
    {
        // try every place
        if (!board->isFull(i))
        {
            // try player 1
            board->place(i, 1);
            if (board->isEnd(i))
                player_win[1] = i + 1 + player_win[1] * (N + 1);
            board->remove(i, 1);

            // try player 2
            board->place(i, 2);
            if (board->isEnd(i))
                player_win[2] = i + 1 + player_win[2] * (N + 1);
            board->remove(i, 2);
            // note down all available place
            choice[n_choice] = i;
            ++n_choice;
        }
    }
}

void MCTS::checkWinPlaceExplore(const int &player, const int &cur_situ)
{
    n_choice = 0;
    
    for (register int i = 0; i < N; ++i)
    {
        // try every place
        if (!board->isFull(i))
        {
            // try player 1
            board->place(i, 1);
            if (board->isEnd(i))
                player_win[1] = i + 1 + player_win[1] * (N + 1);
            board->remove(i, 1);

            // try player 2
            board->place(i, 2);
            if (board->isEnd(i))
                player_win[2] = i + 1 + player_win[2] * (N + 1);
            if (!situs[cur_situ].num_expl[i])
            {
                // note down all available place
                choice[n_choice] = i;
                ++n_choice;
            }
            board->remove(i, 2);
        }
    }
}

// selection
int MCTS::select(const int &cur_situ)
{
    // if not all explored, randomly choose
    if (n_choice)
        // randomly choose
        return choice[rand() % n_choice];

    // else use UCT
    int next_act = -1;
    double max_p = -1, cur_p;
    for (register int i = 0; i < N; ++i)
    {
        if (!board->isFull(i))
        {
            cur_p = getUCT(cur_situ, i);
            if (max_p < cur_p)
            {
                max_p = cur_p;
                next_act = i;
            }
        }
    }
    return next_act;
}

int MCTS::getAction()
{
    int next_act = -1;
    double max_p = -1, cur_p;
    for (int i = 0; i < N; ++i)
    {
        if (!board->isFull(i))
        {
            if (situs[0].num_expl[i])
                cur_p = (double)situs[0].num_win[i] / situs[0].num_expl[i];
            else
                cur_p = 0;
            if (max_p < cur_p)
            {
                max_p = cur_p;
                next_act = i;
            }
        }
    }
    return next_act;
}

// simulation
int MCTS::simulate(const int &player)
{
    register int tmp = player;
    register int next_act;

    while (true)
    {
        // game over
        if (board->isEnd(last_action))
            return (tmp ^ player) ? 1 : 0;
        
        // judge if someplace is to win
        next_act = -1;
        player_win[1] = 0;
        player_win[2] = 0;
        checkWinPlaceRandom(tmp);
        
        if (player_win[tmp])
            next_act = player_win[tmp] % (N + 1) - 1;
        else if (player_win[3 - tmp] >= N + 1)
            return (tmp ^ player) ? 1 : 0;
        else if (player_win[3 - tmp])
            next_act = player_win[3 - tmp] - 1;
        
        // randomly select a place 
        if (next_act == -1)
        {
            if (n_choice)
            {
                // randomly choose
                next_act = choice[rand() % n_choice];
            }
            else
            {
                // case of tie
                return rand() & 1;
            }
        }

        // make a move
        board->place(next_act, tmp);
        last_action = next_act;
        // in turns
        tmp = 3 - tmp;
    }
}
