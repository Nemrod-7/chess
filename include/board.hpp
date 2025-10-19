#pragma once

#include "general.hpp"
#include "bitmanip.hpp"
#include "moves.hpp"

inline static bool is_inside (int x, int y) { return x >= 0 && y >= 0 && x < 8 && y < 8; }

class Board {
    private :
        // wpawn, wbishop, wknight, wrook, wqueen, wking,
        // bpawn, bbishop, bknight, brook, bqueen, bking
        std::vector<std::vector<u64>> bitboard = {{0,0,0,0,0,0},{0,0,0,0,0,0}};
        // u64 bitboard[2][6] = { {0,0,0,0,0,0}, {0,0,0,0,0,0} };
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
    bitboard[color][piece] |= 1ull << curr;
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
