#include <iostream>
#include <vector>

#include "bitop.hpp"
///////////////////////////////////header/////////////////////////////////////
// using u64 = unsigned long long int;

struct vertex { int type, now, nxt; };

enum {black, white};
enum {pawn, bishop, knight, rook, queen, king};

namespace move {
    const int x[64] = {0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
    const int y[64] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7};

    const std::vector<std::pair<int,int>> compass = {{0,-1},{1,0},{0,1},{-1,0}};
    const std::vector<std::pair<int,int>> diagonal = {{-1,-1},{1,-1},{1,1},{-1,1}};
    const std::vector<std::pair<int,int>> complete = {{0,-1},{1,0},{0,1},{-1,0},{-1,-1},{1,-1},{1,1},{-1,1}};
    const std::vector<std::pair<int,int>> knight_m = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };
    const std::vector<std::vector<std::pair<int,int>>> pawn_move = { {{0,1}},{{0,-1}} };
    const std::vector<std::vector<std::pair<int,int>>> pawn_attack = { {{-1,1},{1,1}},{{-1,-1},{1,-1}} };


    const int index(int sx, int sy) { return sx + sy * 8; }

    const std::pair<int, std::vector<std::pair<int,int>>> identify (int piece) {

        switch (piece) {
            case pawn : break;
            case rook : return { 8, move::compass } ;
            case bishop : return { 8, move::diagonal } ;
            case knight : return { 1, move::knight_m } ;
            case queen : return { 8, move::complete } ;
            case king : return { 1, move::complete } ;
        }

        return {-1,{}};
    }
    const std::vector<int> get (int dist, const std::vector<std::pair<int,int>> &direction, int cell) {
        std::vector<int> place;

        for (auto &[dx,dy] : direction) {
            for (int i = 1; i <= dist; i++) {
                int sx = x[cell] + dx * i, sy = y[cell] + dy * i;

                if (sx >= 0 && sy >= 0 && sx < 8 && sy < 8) {
                    place.push_back(sx + sy * 8);
                }
            }
        }

        return place;
    }
    const std::vector<int> get (const std::pair<int, std::vector<std::pair<int,int>>> &mm, int cell) {
        const auto &[dist, direction] = mm;
        std::vector<int> place;

        for (auto &[dx,dy] : direction) {
            for (int i = 1; i <= dist; i++) {
                int sx = x[cell] + dx * i, sy = y[cell] + dy * i;

                if (sx >= 0 && sy >= 0 && sx < 8 && sy < 8) {
                    place.push_back(sx + sy * 8);
                }
            }
        }

        return place;
    }
}

class Board {
    private :
        // u64 grid[2][6] = {{0,0,0,0,0,0},{0,0,0,0,0,0}};
        std::vector<std::vector<u64>> bitboard = {{0,0,0,0,0,0},{0,0,0,0,0,0}};
    public :
        std::vector<u64> & operator [] (int);

        const int player_id(int, int) const;
        void create();
        void place (const std::string &) ;
        const bool is_inside(int, int);
        const int count ();
};
class Display {
    private :
        inline static const std::string utblack[6] = {"♙","♗","♘","♖","♕","♔"};
        inline static const std::string utwhite[6] = {"♟","♝","♞","♜","♛","♚"};

    public :
        static void board (const Board &board) {
            std::cout << "\n";

            for (unsigned long i = 0; i < 64; i++) {
                const int x = move::x[i], y = move::y[i];
                int pla = board.player_id(white, i), opp = board.player_id(black, i);

                if (pla >= 0) {
                    std::cout << utwhite[pla];
                } else if (opp >= 0) {
                    std::cout << utblack[opp];
                } else {
                    std::cout << ".";
                }

                std::cout << " ";
                if (x == 7) std::cout << "\n";
            }
        }
        static void large (Board &board) {
            const std::string cadran = "     A   B   C   D   E   F   G   H  \n";
            const std::string header = "   ╔═══╤═══╤═══╤═══╤═══╤═══╤═══╤═══╗\n";
            const std::string middle = "   ╟───┼───┼───┼───┼───┼───┼───┼───╢\n";
            const std::string footer = "   ╚═══╧═══╧═══╧═══╧═══╧═══╧═══╧═══╝\n";

            std::cout << "\n\n\n";
            std::cout << cadran;
            std::cout << header;

            for (int i = 0; i < 8; i++) {
                std::cout << " " << 8 - i << " ║";
                for (int j = 0; j < 8; j++) {
                    const bool pat = (i+j) % 2;
                    const int pla = board.player_id(white, i * 8 + j), opp = board.player_id(black, i * 8 + j);

                    if (pla >= 0) {
                        std::cout << " " << utwhite[pla] << " ";
                    } else if (opp >= 0) {
                        std::cout << " " << utblack[opp] << " ";
                    } else {
                        std::cout << ( pat == 0 ? "   " : "░░░");
                    }
                    std::cout << (j == 7 ? "║" : "│");
                }
                std::cout << "\n";
                std::cout << (i == 7 ? footer : middle);
            }
        }
        static void limited (const Board &board) {
            std::cout << "\n";
            char disblack[6] = {'p','b','n','r','q','k'};
            char diswhite[6] = {'P','B','N','R','Q','K'};

            std::vector<std::vector<u64>> bitboard = {{0,0,0,0,0,0},{0,0,0,0,0,0}};
            std::vector<std::string> grid (8, std::string(8, '.'));

            for (unsigned long i = 0; i < 64; i++) {
                const int x = move::x[i], y = move::y[i];
                int pla = board.player_id(white, i), opp = board.player_id(black, i);

                if (pla >= 0) {
                    std::cout << diswhite[pla];
                } else if (opp >= 0) {
                    std::cout << disblack[opp];
                } else {
                    std::cout << ".";
                }

                std::cout << " ";
                if (x == 7) std::cout << "\n";
            }

        }
        static std::string identify (int type) {

            switch (type) {
                case pawn : return "pawn"; break;
                case rook : return "rook"; break;
                case king : return "king"; break;
                case queen : return "queen"; break;
                case bishop : return "bishop"; break;
                case knight : return "knight";break;
            }

            return "no id";
        }
};

const std::vector<std::vector<int>> heuristic {
    { // pawn
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    }, { // bishop
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20,
    },{ // knight
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    }, { // rook
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    }, { // queen
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    },
    //     { // king
    // -30,-40,-40,-50,-50,-40,-40,-30,
    // -30,-40,-40,-50,-50,-40,-40,-30,
    // -30,-40,-40,-50,-50,-40,-40,-30,
    // -30,-40,-40,-50,-50,-40,-40,-30,
    // -20,-30,-30,-40,-40,-30,-30,-20,
    // -10,-20,-20,-20,-20,-20,-20,-10,
    //  20, 20,  0,  0,  0,  0, 20, 20,
    //  20, 30, 10,  0,  0, 10, 30, 20
    //     },
    {
        // king end game
        -50,-40,-30,-20,-20,-30,-40,-50,
        -30,-20,-10,  0,  0,-10,-20,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 30, 40, 40, 30,-10,-30,
        -30,-10, 20, 30, 30, 20,-10,-30,
        -30,-30,  0,  0,  0,  0,-30,-30,
        -50,-30,-30,-30,-30,-30,-30,-50
    }

};

///////////////////////////////general.cpp////////////////////////////////////
std::pair<int,int> notation (const std::string &txt) { // chess notation to (type, index) => ex : kb6 -> (5,38)
    const char id = txt[0];
    const int x = txt[1] - 97, y = 7 - (txt[2] - 49);
    // printf("%c %i %i\n", id, x, y);
    switch (id) {
      case 'P' : return {pawn, move::index(x,y)}; break;
      case 'R' : return {rook, move::index(x,y)}; break;
      case 'K' : return {king, move::index(x,y)}; break;
      case 'Q' : return {queen, move::index(x,y)}; break;
      case 'B' : return {bishop, move::index(x,y)}; break;
      case 'N' : return {knight, move::index(x,y)}; break;
    }

    return {-1, 64};
}
int score (int type) {

    switch (type) {
        case pawn   : return 1 ; break;
        case bishop : return 3; break;
        case knight : return 3;break;
        case rook   : return 5; break;
        case queen  : return 8; break;
        case king   : return 99; break;
    }

    return 0;
}
///////////////////////////////board.cpp/////////////////////////////////////

const bool Board::is_inside (int x, int y) {
    return x >= 0 && y >= 0 && x < 8 && y < 8;
}
const int Board::player_id(int color, int pos) const {
  for (int j = 0; j < 6; j++) {
      if (bit::chk(bitboard[color][j], pos)) return j;
  }
  return -1;
}
std::vector<u64> &Board::operator [] (int color) {
    return bitboard[color];
}
void Board::place (const std::string &txt) {
    int color = white;
    auto [piece, curr] = notation(txt);
    bitboard[color][piece] |= 1UL << curr;
}
void Board::create () {
    bitboard = {{
        0xff00, // pawn
        0x0042, // bishop
        0x0024, // knight
        0x0081, // rook
        0x0010, // queen
        0x0008, // king

     }, {
        0x00ff000000000000, // pawn
        0x4200000000000000, // bishop
        0x2400000000000000, // knight
        0x8100000000000000, // rook
        0x1000000000000000, // queen
        0x0800000000000000, // king
       }};
}
const int Board::count () {

    int total = 0;

    for (int i = 0; i < 6; i++) {
        // cout << bit::cnt(bitboard[white][i]) << " " << bit::cnt(bitboard[black][i]) << "\n";
        total += ( bit::cnt(bitboard[white][i]) - bit::cnt(bitboard[black][i])) * score(i);
    }

    return total;
}

////////////////////////////////////////////////////////////////////////////////
