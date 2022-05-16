#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>

#include "chess_pieces.h"
#include "chess_board.h"
#include "chess_player.h"

using namespace std;

void play_chess_one_turn(Board& board, Player& player) {
  cout << board << endl;
  cout << player.name() << "'s turn." << endl;
  vector<Move> moves = board.get_moves();
  Move move;
  while (true) {
    move = player.get_move(board, moves);
    if (find(moves.begin(), moves.end(), move) != moves.end()) {
      break;
    }
  }
  cout
    << player.name() << " chose to move " << board[move.from]
    << " from " << move.from << " to " << move.to << " ("
    << board[move.to] << ")\n\n";
  board.make_move(move);
}

Team play_one_chess_game(Player& white_player, Player& black_player) {
  Board board;
  vector<Move> moves;
  while (true) {
    play_chess_one_turn(board, white_player);
    if (board.winner() != NONE) {
      break;
    }
    play_chess_one_turn(board, black_player);
    if (board.winner() != NONE) {
      break;
    }
  }
  Team winner = board.winner();
  cout << team_name(winner) << " won!\n";
  return winner;
}

int main(int argc, const char *argv[]) {
  CheckMateCapturePlayer white_player(WHITE);
  CapturePlayer black_player(BLACK);
  int white_wins = 0, black_wins = 0;
  while (true) {
    Team winner = play_one_chess_game(white_player, black_player);
    if (winner == WHITE) ++white_wins;
    else ++black_wins;
    if ((white_wins + black_wins) % 1000 == 0)
      cerr << static_cast<double>(white_wins) / (white_wins + black_wins) << "  " << white_wins + black_wins << endl;
  }
  return 0;
}
