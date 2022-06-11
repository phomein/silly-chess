#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utf8_codepoint.h"
#include "chess_pieces.h"
#include "chess_board.h"

using std::endl;
using std::istream;
using std::map;
using std::ostream;
using std::out_of_range;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::vector;
using std::streampos;

const char* team_name(Team team)
{
    switch (team)
    {
    case WHITE:
        return "White";
    case BLACK:
        return "Black";
    case NONE:
        return "None";
    }
    return "UNKNOWN";
}

bool Cell::operator==(Cell other) const
{
    return x == other.x && y == other.y;
}

bool Cell::operator!=(Cell other) const
{
    return !(*this == other);
}

bool Move::operator==(Move other) const
{
    return to == other.to && from == other.from;
}

bool Move::operator!=(Move other) const
{
    return !(*this == other);
}

ostream& operator<<(ostream& os, const Cell& cell)
{
    return os << static_cast<char>(cell.x + 'a') << cell.y + 1;
}
istream& operator>>(istream& is, Cell& cell)
{
    char x_plus_a;
    int y_plus_1;
    is >> x_plus_a >> y_plus_1;
    cell.x = x_plus_a - 'a';
    cell.y = y_plus_1 - 1;
    return is;
}

ostream& operator<<(ostream& os, const Move& move)
{
    return os << move.from << move.to;
}
istream& operator>>(istream& is, Move& move)
{
    return is >> move.from >> move.to;
}

Board::Board()
{
    board.resize(rows, vector<const ChessPiece* >(cols));
    reset_board();
}

const ChessPiece& Board::operator[](Cell cell) const
{
    return *board[cell.y][cell.x];
}

void Board::reset_board()
{
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            board[y][x] = &EMPTY_SPACE;
        }
    }

    for (int x = 0; x < cols; ++x)
    {
        board[1][x] = &WHITE_PAWN;
        board[6][x] = &BLACK_PAWN;
    }

    board[0][0] = &WHITE_ROOK;
    board[0][1] = &WHITE_KNIGHT;
    board[0][2] = &WHITE_BISHOP;
    board[0][3] = &WHITE_QUEEN;
    board[0][4] = &WHITE_KING;
    board[0][5] = &WHITE_BISHOP;
    board[0][6] = &WHITE_KNIGHT;
    board[0][7] = &WHITE_ROOK;

    board[7][0] = &BLACK_ROOK;
    board[7][1] = &BLACK_KNIGHT;
    board[7][2] = &BLACK_BISHOP;
    board[7][3] = &BLACK_QUEEN;
    board[7][4] = &BLACK_KING;
    board[7][5] = &BLACK_BISHOP;
    board[7][6] = &BLACK_KNIGHT;
    board[7][7] = &BLACK_ROOK;

    current_teams_turn = WHITE;
}

vector<Move> Board::get_moves() const
{
    vector<Move> moves;
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (board[y][x]->team == current_teams_turn)
            {
                board[y][x]->get_moves(*this, Cell(x, y), moves);
            }
        }
    }
    for (Move move : moves)
    {
        if (!contains(move.to) || !contains(move.from))
        {
            stringstream err_msg;
            err_msg << "Board::get_moves got a move that moves to or from a cell that is not on the board: " << move;
            throw out_of_range(err_msg.str());
        }
    }
    return moves;
}

// This function represents how most classical chess ALL_CHESS_PIECES would move.
// This also allows us to add support for more complex "moves", like a pawn
// getting to the end of the board and turning into a queen or some other type
// of piece.
// If we allow the chess piece that's moving to define the move, then we can
// add really interesting custom ALL_CHESS_PIECES that are nothing like normal ALL_CHESS_PIECES!
void Board::make_classical_chess_move(Move move)
{
    board[move.to.y][move.to.x] = board[move.from.y][move.from.x];
    board[move.from.y][move.from.x] = &EMPTY_SPACE;
    current_teams_turn = current_teams_turn == WHITE ? BLACK : WHITE;
}

void Board::make_move(Move move)
{
    if (!contains(move.to) || !contains(move.from))
    {
        stringstream err_msg;
        err_msg << "Board::make_move called with a move that moves to or from a cell that is not on the board: " << move;
        throw out_of_range(err_msg.str());
    }
    board[move.from.y][move.from.x]->make_move(*this, move);
}

bool Board::contains(Cell cell) const
{
    return cell.x >= 0 && cell.x < 8 && cell.y >= 0 && cell.y < 8;
}

Team Board::winner() const
{
    bool found_white_king = false, found_black_king = false;
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            if (board[y][x] == &WHITE_KING)
            {
                found_white_king = true;
            }
            else if (board[y][x] == &BLACK_KING)
            {
                found_black_king = true;
            }
        }
    }
    if (!found_white_king)
    {
        return BLACK;
    }
    if (!found_black_king)
    {
        return WHITE;
    }
    return NONE;
}

ostream& operator<<(ostream& os, const Board& board)
{
    os << "   ";
    for (int i = 0; i < board.cols; ++i)
    {
        os << static_cast<char>(i + 'a');
    }
    os << endl;
    for (int y = board.rows - 1; y >= 0; --y)
    {
        if (y < 10 - 1)
        {
            os << ' ';
        }
        os << (y + 1) << ' ';
        for (int x = 0; x < board.cols; ++x)
        {
            os << board[Cell(x, y)];
        }
        os << ' ' << (y + 1) << endl;
    }
    os << "   ";
    for (int i = 0; i < board.cols; ++i)
    {
        os << static_cast<char>(i + 'a');
    }
    os << endl;
    return os;
}

istream& operator>>(istream& is, Board& board)
{
    string begline;
    getline(is, begline);
    if (begline.find("   ab") == string::npos)
    {
        throw runtime_error("First row of chess board input does not match expected headers!");
    }
    board.cols = begline.size() - 3;  // 3 spaces at beginning 
    
    streampos cur = is.tellg();  // save cur pos at beginning of line
    char num_char_tens = is.get();
    char num_char_ones = is.get();
    string rows_str;
    rows_str.push_back(num_char_tens);
    if (rows_str == " ")
    {
        rows_str.pop_back();
    }
    rows_str.push_back(num_char_ones);
    board.rows = std::stoi(rows_str);
    board.board.clear();
    board.board.resize(board.rows, vector<const ChessPiece* >(board.cols));
    is.seekg(cur, is.beg);

    for (int y = board.rows - 1; y >= 0; --y)
    {
        char beg_buf[3]{};
        is.read(beg_buf, sizeof(beg_buf));
        if (is.bad())
        {
            throw runtime_error("Failed to read beginning of chess board row!");
        }
        for (int x = 0; x < board.cols; ++x)
        {
            UTF8CodePoint piece;
            is >> piece;
            board.board[y][x] = ALL_CHESS_PIECES.at(piece);
        }
        string endofline;
        getline(is, endofline);
    }
    string lastline;
    getline(is, lastline);
    return is;
}