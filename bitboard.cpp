#include <iostream>
#include <vector>

#include "bitop.hpp"
#include "moves.hpp"
#include "general.hpp"
#include "board.hpp"

using namespace std;
////////////////////////////chess.hpp//////////////////////////////////////////////////
             // horizontal primary    // vertical primary      // diagonal primary      // diagonal secondary
const u64 HP = 0xff00000000000000, VP = 0x8080808080808080, DP = 0x8040201008040201, DS = 0x0102040810204080;

void display(u64 grid) {
    for (int i = 0; i < 64; i++) {
        if (i > 0 && i % 8 == 0) cout << '\n';
        if (grid >> i &1) {
            cout << 'x';
        } else {
            cout << '.';
        }
        cout << ' ';
    }
    cout << '\n';
    cout << '\n';
}

u64 getmask (u8 piece, u8 curr, u64 grid) {

    if (piece == pawn) {

    } else if (piece == knight) {
        return move::knight[curr];
    } else if (piece == king) {
        return move::king[curr];
    } else if (piece == rook) {
        return move::cross(curr, grid);
    } else if (piece == bishop) {
        return move::diag(curr, grid);
    } else if (piece == queen) {
        return move::cross(curr, grid) | move::diag(curr, grid);
    }

    return 0ull;
}

bool threat (Board &board, bool side, u8 pos) {
    const bool oppo = side ^ 1;
    u64 player = 0, enemy = 0;

    for (int i = 0; i < 6; i++) {
        player |= board[side][i], enemy |= board[oppo][i];
    }

    // u64 knight_threat = player & move::knight[pos];
    // u64 king_threat = player & move::king[pos];
    for (int i = 0; i < 8; i++) {
        const u64 hit = rays[pos][i] & (player | enemy);

        if (hit != 0) {
            const u8 ix = (i < 4) ? bit::ScanForward(hit) : bit::ScanReverse(hit);
            const u8 x = move::x[ix], y = move::y[ix];

            if (x == 0 || y == 0) {
                if (bit::chk(board[oppo][rook], ix)) return true;
                if (bit::chk(board[oppo][queen], ix)) return true;
            } else {
                if (bit::chk(board[oppo][bishop], ix)) return true;
                if (bit::chk(board[oppo][queen], ix)) return true;
            }
        }
        // display(hit);
        // cout << " index : ";
        // cout << bit::ScanForward(hit) << " ";
        // cout << bit::ScanReverse(hit) << " ";
    }

    return false;
}
u64 threat_zone (Board &board, u8 side) {
    const u8 oppo = side ^ 1;

    const u64 player = board[side][pawn] | board[side][bishop] | board[side][knight] |
                       board[side][rook] | board[side][queen]  | board[side][king];
    const u64 enemy = board[oppo][pawn] | board[oppo][bishop] | board[oppo][knight] |
                      board[oppo][rook] | board[oppo][queen]  | board[oppo][king];

    const std::vector<u8> pos = bit::pos(player);
    u64 fzone = 0ull;

    for (const auto &curr : pos) {
        const u8 piece = board.player_id(side, curr);
        u64 bitmov = getmask(piece, curr, player | enemy);

        fzone |= ( bitmov ^= bitmov & player) ;
    }

    return fzone;
}
vector<vertex> get_moves4 (Board &board, u8 side) {
    const u8 oppo = side ^ 1;
    vector<vertex> vs;

    const u64 player = board[side][pawn] | board[side][bishop] | board[side][knight] |
        board[side][rook] | board[side][queen]  | board[side][king];
    const u64 enemy = board[oppo][pawn] | board[oppo][bishop] | board[oppo][knight] |
        board[oppo][rook] | board[oppo][queen]  | board[oppo][king];

    const std::vector<u8> pos = bit::pos(player);
    const u8 kgp = bit::pos(board[side][king])[0];
    const u8 kgo = bit::pos(board[oppo][king])[0];
    const u64 fzone = threat_zone(board, black);

    // display(fzone);
    for (const auto &curr : pos) {
        const u8 piece = board.player_id(side, curr);
        u64 bitmov = getmask(piece, curr, player | enemy);
        bitmov ^= bitmov & player;
        // const int x = move::x[curr], y = move::y[curr];

        for (auto &next : bit::pos(bitmov)) {
            if (bit::chk(fzone, next)) {
              // cout << Display::identify(piece) ;
              // cout << "[" << move::x[curr] << " " << move::y[curr]  << "]";
              //  cout << to_notation(piece, next) << "\n";
            } else {
                vs.push_back( { piece, curr, next } );
            }
        }
        // cout << Display::identify(piece) ;
        // cout << to_notation(piece, curr);
        // cout << "[" << move::x[curr] << " " << move::y[curr]  << "]";
        // cout << "[" << move::x[next] << " " << move::y[next]  << "]";
        // cout << "\n";
        // display(bitmov);
    }

    return vs;
}

int main () {

    Board board;
    int game = opening;
    int cycle = 0, fifty = 0;

    auto [piece1, idx] = notation("Kc8");
    board[black][piece1] |= 1UL << idx;

    board.place("Ke8");
    board.place("Rh7");
    // board.place("Nb6");
    // board.place("Qh8");

    if (!board[white][queen] || !board[black][queen]) {
        game = endgame;
    }

    const int side = white;
    const int oppo = side ^ 1;

    u64 player = board[side][pawn] | board[side][bishop] | board[side][knight] |
        board[side][rook] | board[side][queen]  | board[side][king];
    u64 enemy = board[oppo][pawn] | board[oppo][bishop] | board[oppo][knight] |
        board[oppo][rook] | board[oppo][queen]  | board[oppo][king];

    enemy = 111154397639ull;

    const int sens = (side + 2) + side * 3;

    int ix = 42;
    int d1 = ix - 7, d2 = ix + 7;

    u64 bt = 0ull;

    if (side == white) {
        bt |= 1ull << ix;
        bt |= 1ull << (ix - 7);
        bt |= 1ull << (ix - 9);
    } else {

    }

    display(bt);
    // display(rays[ix][N]);
    // const u8 kgp = bit::pos(board[black][king])[0];
    // const bool th = threat(board, black, kgp);
    // cout << "[" << move::x[kgp] << " " << move::y[kgp]  << "]";
    // get_moves4(board, white);
    // Display::limited(board);
    // switch (game) {
    //     case opening : std::cout << "opening"; break;
    //     case endgame : std::cout << "endgame"; break;
    //     case middlegame : std::cout << "middlegame"; break;
    // }
    // std::cout << "\n";
    // std::cout << "move : " << cycle  << " ";
    // cout << to_notation(king, kgp) << " ";

    cout << "\nend\n";
}
