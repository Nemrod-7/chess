#include <iostream>
#include <random>
#include <limits>

using namespace std;
using u64 = unsigned long int;

enum {white, black};
enum {pawn, bishop, knight, rook, queen, king};

int table[64][12];

void init() { // # fill a table of random numbers/bitstrings
    int random = 0;
    u64 inf = std::numeric_limits<unsigned long int>::max();
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dist (0, inf);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 12; j++) {
            table[i][j] = dist(gen); // random bitstring
        }
    }
    // table.black_to_move = random_bitstring()
}

int hash(int board[64]) {
    int h = 0;
    // if is_black_turn(board): h := h XOR table.black_to_move

    for (int i = 0; i < 64; i++) {
        if (board[i]) {
            h ^= table[i][board[i]];
        }
    }

    return h;
}



void display(u64 grid) {
    for (int i = 0; i < 64; i++) {
        if (i > 0 && i % 8 == 0) cout << '\n';
        if (grid >> i &1) {
            cout << 'x';
        } else {
            cout << '.';
        }
    }
    cout << '\n';
    cout << '\n';
}
const uint64_t
    HP = 0xff00000000000000, // horizontal primary
    VP = 0x8080808080808080, // vertical primary
    DP = 0x8040201008040201, // diagonal primary
    DS = 0x0102040810204080; // diagonal secondary

u64 stable (int x, int y) {
    u64 m = VP >> x | HP >> (y << 3);
    int z = 7 - x;

    if (x >= y) {
        m |= DP << ((x - y) << 3);
    } else {
        m |= DP >> ((y - x) << 3);
    }

    if (z >= y) {
        m |= DS << ((z - y) << 3);
    } else {
        m |= DS >> ((y - z) << 3);
    }

    return m;
}

int main () {

    u64 board = 0;
    u64 pieces[2][6] = { {0,0,0,0,0,0}, {0,0,0,0,0,0} };
    // wpawn, wbishop, wknight, wrook, wqueen, wking,
    // bpawn, bbishop, bknight, brook, bqueen, bking

    u64 player =
        pieces[white][pawn] | pieces[white][bishop] | pieces[white][knight] |
        pieces[white][rook] | pieces[white][queen] | pieces[white][king];
    u64 enemy =
        pieces[black][pawn] | pieces[black][bishop] | pieces[black][knight] |
        pieces[black][rook] | pieces[black][queen] | pieces[black][king];

    u64 grid = player | enemy;

    u64 h = 0;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 6; j++) {
            if (pieces[white][j] >> i &1) h ^= table[i][j + 0];
            if (pieces[black][j] >> i &1) h ^= table[i][j + 6];
        }
    }

    const u64 horiz = 255UL;
    const u64 verti = 72340172838076673UL;
    const u64 diago = 9241421688590303745UL;

    player = 2;
    enemy = 16384 + 32;

    grid = 2;

    display(stable(0,0));
    // display(enemy);

    // for (int i = 0; i < 4; i++) {
    //     grid <<= 8;
    // }


}
