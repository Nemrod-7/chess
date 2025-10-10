#include <iostream>
#include "chess.hpp"

using namespace std;

enum {opening, middlegame, endgame};

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
bool threat (Board &board, int x, int y) {

    for (int k = 0; k < 8; k++) {
        int nx = x + move::knight_m[k].first, ny = y + move::knight_m[k].second;
        int nxt = move::index(nx,ny);

        if (board.is_inside(nx,ny) && bit::chk(board[black][knight], nxt)) return true;

        for (int dist = 1; dist <= 8; dist++) {
            nx = x + move::complete[k].first * dist, ny = y + move::complete[k].second * dist;
            nxt = move::index(nx,ny);

            if (board.is_inside(nx,ny)/* && board[nxt] != ' ' */) {
                if (k < 2 && dist < 2 && bit::chk(board[black][pawn], nxt)) return true;

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

int main() {

    // black cannot make a valid move. There are three possibilities:
    //     Checkmate after at most 16 white moves: the test succeeds.
    //     Checkmate after more than 16 white moves: the test fails. Note that the test is not interrupted after 16 white moves so at least you can see how many moves it took.
    //     Stalemate: the test fails.
    //
    // The threefold position or 50-move rule applies: the test fails.

    Board board;
    int game = opening;
    int history[64][6] = {0};


    auto [piece1, idx] = notation("Ke8");
    board[black][piece1] |= 1UL << idx;

    board.place("Qe7");
    board.place("Ke6");


    if (board[white][queen] == 0) {
        game = endgame;
    }


    const int kg = bit::pos(board[black][king])[0];
    const int kx = move::x[kg], ky = move::y[kg];

    const u64 fzone = threat_zone(board, black);
    int free = 0, place = 0;

    if (bit::chk(fzone, kg)) cout << "check\n";

    // for (auto &[dx,dy] : move::complete) {
    //     int nx = dx + kx, ny = dy + ky;
    //     int next = move::index(nx,ny);
    //
    //     if (board.is_inside(nx, ny)) {
    //
    //         if (!bit::chk(fzone, next)) {
    //             free++;
    //             place = next;
    //         }
    //     }
    // }


    vector<vertex> moves = get_moves(board, white);
    //
    // for (auto [piece, curr, next] : moves) {
    //
    //     if (next == kg) {
    //         cout << "check\n";
    //     }
    //     // cout << Display::identify(piece) ;
    //     // cout << "[" << move::x[curr] << " " << move::y[curr]  << "]";
    //     // cout << "[" << move::x[next] << " " << move::y[next]  << "]";
    //     // cout << "\n";
    // }
    //
    for (int curr = 0; curr < 64; curr++) {
        if (curr > 0 && curr % 8 == 0) cout << "\n";

        if (bit::chk(fzone, curr)) {
            cout << "x ";
        } else {
            cout << ". ";
        }
    }
    // history[next][piece]++;

    // cout << "\n";
    // Display::limited(board);

    cout << "\nexit\n";
}
