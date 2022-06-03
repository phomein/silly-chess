#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>

#include "chess_pieces.h"
#include "chess_board.h"
#include "chess_player.h"

// algorithm
using std::find;

// iostream
using std::cout;
using std::cerr;
using std::endl;

// stdexcept
using std::logic_error;
using std::out_of_range;

// string
using std::string;

class UnitTestException : public logic_error
{
public:
    UnitTestException(const string &err) : logic_error(err) {}
};

template <typename T, typename U>
void assert_equals(const T &expected, const U &actual, const string &error_msg)
{
    if (expected != actual)
    {
        std::stringstream msg;
        msg << "Expected " << expected << " but got " << actual << ". " << error_msg;
        throw UnitTestException(msg.str());
    }
}

// chess pieces
void test_is_opposite_team()
{
    assert_equals(true, WHITE_KNIGHT.is_opposite_team(BLACK_KNIGHT), "WHITE/BLACK KNIGHT in test_is_opposite_team");
    assert_equals(false, WHITE_KNIGHT.is_opposite_team(WHITE_QUEEN), "WHITE KNIGHT/QUEEN in test_is_opposite_team");
    assert_equals(true, BLACK_PAWN.is_opposite_team(WHITE_QUEEN), "BLACK PAWN/WHITE QUEEN in test_is_opposite_team");
    assert_equals(false, BLACK_KING.is_opposite_team(BLACK_QUEEN), "BLACK KING/QUEEN in test_is_opposite_team");
}

void test_piece_identity()
{
    assert_equals(true, WHITE_KNIGHT == WHITE_KNIGHT, "WHITE KNIGHT in test_piece_identity");
    assert_equals(false, WHITE_KNIGHT != WHITE_KNIGHT, "WHITE KNIGHT in test_piece_identity");
    assert_equals(true, BLACK_PAWN != WHITE_QUEEN, "BLACK PAWN/WHITE QUEEN in test_piece_identity");
}

void test_get_and_make_moves()
{
    Board board;
    vector<Move> moves;
    // test initial conditions and individual pieces of a real chess game
    WHITE_PAWN.get_moves(board, Cell(0, 1), moves);
    WHITE_PAWN.make_move(board, moves[0]);
    WHITE_PAWN.get_moves(board, Cell(0, 2), moves);
    WHITE_PAWN.make_move(board, moves[1]);
    WHITE_ROOK.get_moves(board, Cell(0, 0), moves);
    WHITE_ROOK.make_move(board, moves[3]);
    WHITE_ROOK.get_moves(board, Cell(0, 2), moves);
    WHITE_KNIGHT.get_moves(board, Cell(1, 0), moves);
    BLACK_KNIGHT.get_moves(board, Cell(1, 7), moves); 
    assert_equals(16, moves.size(), "test_get_and_make_moves: size of moves vector");
    vector<Move> expected_moves = {
        Move(Cell(0,1), Cell(0,2)), 
        Move(Cell(0,2), Cell(0,3)),
        Move(Cell(0,0), Cell(0,1)),
        Move(Cell(0,0), Cell(0,2)),
        Move(Cell(0,2), Cell(1,2)),
        Move(Cell(0,2), Cell(2,2)),
        Move(Cell(0,2), Cell(3,2)),
        Move(Cell(0,2), Cell(4,2)),
        Move(Cell(0,2), Cell(5,2)),
        Move(Cell(0,2), Cell(6,2)),
        Move(Cell(0,2), Cell(7,2)),
        Move(Cell(0,2), Cell(0,1)),
        Move(Cell(0,2), Cell(0,0)),
        Move(Cell(1,0), Cell(2,2)),
        Move(Cell(1,7), Cell(0,5)),
        Move(Cell(1,7), Cell(2,5))
    };
    for (int i = 0; i < moves.size(); ++i)
    {
        assert_equals(expected_moves[i], moves[i], "test_get_and_make_moves: initial moves");
    }

    // test other individual pieces in "floating space". non-real game flow conditions
    board.reset_board();
    moves.clear();
    BLACK_QUEEN.get_moves(board, Cell(3,3), moves);
    assert_equals(19, moves.size(), "test_get_and_make_moves: BLACK_QUEEN");
    moves.clear();
    WHITE_KING.get_moves(board, Cell(3,3), moves);
    assert_equals(8, moves.size(), "test_get_and_make_moves: WHITE_KING");
    moves.clear();
    BLACK_BISHOP.get_moves(board, Cell(3,3), moves);
    assert_equals(8, moves.size(), "test_get_and_make_moves: BLACK_BISHOP");
}



// chess board
void test_board()
{
    // test board.get_moves(), which calls each piece's get_move() for the player turn, which is white by default
    Board board;
    vector<Move> moves = board.get_moves();
    assert_equals(12, moves.size(), "test_board: size of moves vector");
    vector<Move> expected_moves = {
        Move(Cell(1,0), Cell(0,2)), 
        Move(Cell(1,0), Cell(2,2)),
        Move(Cell(6,0), Cell(5,2)),
        Move(Cell(6,0), Cell(7,2)),
        Move(Cell(0,1), Cell(0,2)),
        Move(Cell(1,1), Cell(1,2)),
        Move(Cell(2,1), Cell(2,2)),
        Move(Cell(3,1), Cell(3,2)),
        Move(Cell(4,1), Cell(4,2)),
        Move(Cell(5,1), Cell(5,2)),
        Move(Cell(6,1), Cell(6,2)),
        Move(Cell(7,1), Cell(7,2))
    };
    for (int i = 0; i < moves.size(); ++i)
    {
        assert_equals(expected_moves[i], moves[i], "test_board: initial white moves");
    }

    // test out of bounds
    try
    {
        board.make_move(Move(Cell(0,0), Cell(8,8)));
    }
    catch (const out_of_range& e)
    {
        assert_equals(0, strcmp("Board::make_move called with a move that moves to or from a cell that is not on the board: a1i9", e.what()), "test_board: out of bounds");
    }
}


// chess player
void test_players()
{
    Board board;
    RandomPlayer rando(WHITE);
    CapturePlayer cap(BLACK);
    CheckMateCapturePlayer check(WHITE);
    vector<Move> white_moves = {
        Move(Cell(1,0), Cell(4,7)),  // killer move straight for the jugular
        Move(Cell(0,2), Cell(3,4)),  // random move
        Move(Cell(0,7), Cell(4,3)),  // random move
        Move(Cell(0,1), Cell(1,6))   // take a pawn
    };
    vector<Move> black_moves = {
        Move(Cell(0,2), Cell(3,4)),  // random move
        Move(Cell(0,7), Cell(4,3)),  // random move
        Move(Cell(1,6), Cell(0,1))   // take a pawn
    };
    assert_equals(Move(Cell(1,0), Cell(4,7)), check.get_move(board, white_moves), "test_players: checkmate_player");
    assert_equals(Move(Cell(1,6), Cell(0,1)), cap.get_move(board, black_moves), "test_players: capture_player");
    Move rando_move = rando.get_move(board, white_moves);
    if (find(white_moves.begin(), white_moves.end(), rando_move) == white_moves.end())
    {
        std::stringstream msg;
        msg << "Expected " << rando_move << " to be part of white_moves vector! test_players: random_player";
        throw UnitTestException(msg.str());
    }
    assert_equals(Move(Cell(1,6), Cell(0,1)), cap.get_move(board, black_moves), "test_players: capture_player");
}


// int main()
// {
//     try
//     {
//         test_is_opposite_team();
//         test_piece_identity();
//         test_get_and_make_moves();
//         test_board();
//         test_players();
//     }
//     catch (UnitTestException& e)
//     {
//         cerr << "Unit test(s) failed!\n" << e.what() << endl;
//     }
//     return 0;
// }
