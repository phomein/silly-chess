#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>

#include "chess_pieces.h"
#include "chess_board.h"
#include "chess_player.h"

using namespace std;

void play_chess_one_turn(Board &board, Player &player)
{
    cout << board << endl;
    cout << player.name() << "'s turn." << endl;
    vector<Move> moves = board.get_moves();
    Move move;
    while (true)
    {
        move = player.get_move(board, moves);
        if (find(moves.begin(), moves.end(), move) != moves.end())
        {
            break;
        }
    }
    cout
        << player.name() << " chose to move " << board[move.from]
        << " from " << move.from << " to " << move.to << " ("
        << board[move.to] << ")\n\n";
    board.make_move(move);
}

Team play_one_chess_game(Player &white_player, Player &black_player)
{
    Board board;
    vector<Move> moves;
    while (true)
    {
        play_chess_one_turn(board, white_player);
        if (board.winner() != NONE)
        {
            break;
        }
        play_chess_one_turn(board, black_player);
        if (board.winner() != NONE)
        {
            break;
        }
    }
    Team winner = board.winner();
    cout << team_name(winner) << " won!\n";
    return winner;
}

// int main(int argc, const char *argv[])
// {
//     CapturePlayer white_player(WHITE);
//     CheckMateCapturePlayer black_player(BLACK);
//     int b{};
//     int w{};
//     int g{};
//     for (int x = 0; x < 100; ++x)
//     {
//         Team winner = play_one_chess_game(white_player, black_player);
//         if (winner == BLACK)
//         {
//             ++b;
//         }
//         else if (winner == WHITE)
//         {
//             ++w;
//         }
//         else
//         {
//             cout << "ruhroh!";
//         }
//         ++g;
//     }
//     cout << "black won " << b << " games out of " << g << endl;
//     cout << "white won " << w << " games out of " << g << endl;
//     return 0;
// }
