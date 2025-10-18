#include <iostream>
#include "chess.hpp"

using namespace std;

enum {opening, middlegame, endgame};

bool threat (Board &board, int x, int y) {

    u64 fzone = 0;

    for (int i = 0; i < 6; i++) {
        fzone |= (board[white][i] | board[black][i]);
    }


    for (int k = 0; k < 8; k++) {
        int nx = x + move::knight_m[k].first, ny = y + move::knight_m[k].second;
        int nxt = move::index(nx,ny);

        if (board.is_inside(nx,ny) && bit::chk(board[black][knight], nxt)) return true;

        for (int dist = 1; dist <= 8; dist++) {
            nx = x + move::complete[k].first * dist, ny = y + move::complete[k].second * dist;
            nxt = move::index(nx,ny);

            if (board.is_inside(nx,ny) && bit::chk(fzone, nxt)) {
                if (k < 2 && dist < 2 && bit::chk(board[black][pawn], nxt)) return true;
                // cout << char(nx - 1 + 'A') << " " << ny << "\n";

                if (k > 3) {
                    if (bit::chk(board[black][queen], nxt) || bit::chk(board[black][rook], nxt)) return true;
                } else {
                    if (bit::chk(board[black][queen], nxt) || bit::chk(board[black][bishop], nxt)) return true;
                }

                break;
            }
        }
    }

    return false;
}

u64 threat_zone (Board &board, int side) {
    const int opp = side ^ 1;
    u64 player = 0, enemy = 0, fzone = 0;
    u64 grid = 0, curr = 0;

    for (int i = 0; i < 6; i++) {
        player |= board[side][i], enemy |= board[opp][i];
    }

    for (const auto &curr : bit::pos(enemy)) {
        const int piece = board.player_id(opp, curr);
        auto [dist, direction] = move::identify(piece);

        if (piece == pawn)
            dist = 1, direction = move::pawn_attack[opp];

        for (auto &[dx,dy] : direction) {
            for (int i = 1; i <= dist; i++) {
                const int nx = move::x[curr] + dx * i, ny = move::y[curr] + dy * i;

                if (board.is_inside(nx, ny) ) {
                    const int next = move::index(nx,ny);
                    if (bit::chk(enemy, next)) break;

                    fzone |= 1UL << next;

                    if (bit::chk(player, next)) break;
                }
            }
        }
    }

    return fzone;
}
vector<vertex> get_moves (Board &board, int side) {
    const int opp = side ^ 1;
    const u64 fzone = threat_zone(board, side);
    u64 player = 0, enemy = 0;
    vector<vertex> hist;

    for (int i = 0; i < 6; i++) {
        enemy |= board[opp][i], player |= board[side][i];
    }

    // do {
    for (int curr = 0; curr < 64; curr++) {
        if (bit::chk(player, curr)) {
            const int piece = board.player_id(side, curr);
            auto [dist, direction] = move::identify(piece);

            if (piece == pawn) {
                dist = (move::y[curr] == 1 || move::y[curr] == 6) ? 2 : 1;
                direction = move::pawn_move[side];
            }

            for (auto &[dx,dy] : direction) {
                for (int i = 1; i <= dist; i++) {
                    const int nx = move::x[curr] + dx * i, ny = move::y[curr] + dy * i;
                    const int next = move::index(nx, ny);

                    if (board.is_inside(nx, ny) && !bit::chk(fzone, next) ) {
                        if (bit::chk(player, next)) break;

                        hist.push_back( {piece, curr, next});

                        if (bit::chk(enemy, next)) break;
                    }
                }
            }
        }
    }
    // while(big >>= 1);

    return hist;
}

int main() {

    // black cannot make a valid move. There are three possibilities:
    //     Checkmate after at most 16 white moves: the test succeeds.
    //     Checkmate after more than 16 white moves: the test fails. Note that the test is not interrupted after 16 white moves so at least you can see how many moves it took.
    //     Stalemate: the test fails.
    //
    // The threefold position or 50-move rule applies: the test fails.

    Board board;

    int game = opening;
    int cycle = 0, fifty = 0;
    int hash[64] = {0};
    int history[64][6] = {0};

    auto [piece1, ix1] = notation("Ka8");
    board[black][piece1] |= 1UL << ix1;

    // auto [piece2, ix2] = notation("Qd5");
    // board[black][piece2] |= 1UL << ix2;

    board.place("Kh5");
    board.place("Qb6");


    if (!board[white][queen] || !board[black][queen]) {
        game = endgame;
    }

    switch (game) {
      case opening : std::cout << "opening"; break;
      case endgame : std::cout << "endgame"; break;
      case middlegame : std::cout << "middlegame"; break;
    }

    std::cout << "\n";
    Display::limited(board);
    std::cout << "move : " << cycle ;

    // const int bk = bit::pos(board[black][king])[0];
    // const int bx = move::x[bk], by = move::y[bk];
    // const u64 fzone = threat_zone(board, black);
    //
    // const int kg = bit::pos(board[white][king])[0];
    // const int kx = move::x[kg], ky = move::y[kg];
    //
    // vector<vertex> moves = get_moves(board, white);
    //
    // cout << "threat : " << threat(board, bx, by);
    //
    // cout << "\n";
    // for (int curr = 0; curr < 64; curr++) {
    //     if (curr > 0 && curr % 8 == 0) cout << "\n";
    //
    //     if (bit::chk(fzone, curr)) {
    //         cout << "x ";
    //     } else {
    //         cout << ". ";
    //     }
    // }
    // cout << "\n";

    // for (auto [piece, curr, next] : moves) {
    //     board[white][piece] ^= 1UL << curr;
    //     board[white][piece] ^= 1UL << next;
    //
    //     cout << Display::identify(piece) ;
    //     cout << "[" << (char) (move::x[curr] + 'A') << " " << (8 - move::y[curr])  << "]";
    //     cout << "[" << (char) (move::x[next] + 'A') << " " << (8 - move::y[next]) << "]";
    //
    //     cout << threat(board, kx, ky);
    //     cout << "\n";
    //
    //
    //     board[white][piece] ^= 1UL << curr;
    //     board[white][piece] ^= 1UL << next;
    // }


    cout << "\n";

    cout << "\nexit\n";
}
