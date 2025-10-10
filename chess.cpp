#include <algorithm>
#include "chess.hpp"
// #include <random>

using namespace std;

// random_device rd;  //Will be used to obtain a seed for the random number engine
// mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

// vertex rnd_walk (vector<vertex> &vs) {
//     sort(vs.begin(), vs.end(), [](vertex a, vertex b) { return a.alt > b.alt; });
//     std::geometric_distribution dist;
//
//     return vs[dist(gen) % vs.size()];
// }


string format (int piece, int curr) {
    std::string place = "   ";
    int x = move::x[curr] , y = move::y[curr];

    switch (piece) {
        case pawn : place[0] = 'P'; break;
        case rook : place[0] = 'R'; break;
        case king : place[0] = 'K'; break;
        case queen : place[0] = 'Q'; break;
        case bishop : place[0] = 'B'; break;
        case knight : place[0] = 'N'; break;
    }

    place[1] = x + 97;
    place[2] = y + 49;
    return place;
}

void change (Board &board, const vertex &node, int enemy) {
    const auto &[piece, curr, next] = node;

    if (enemy != -1) board[black][enemy] ^= 1UL << next;
    board[white][piece] ^= 1UL << curr;
    board[white][piece] ^= 1UL << next;
}

u64 threat (Board &board, int side) {
    const int opp = side ^ 1;
    u64 player = 0, enemy = 0, threat = 0;

    for (int i = 0; i < 6; i++) {
        player |= board[side][i], enemy |= board[opp][i];
    }

    for (int curr = 0; curr < 64; curr++) {
        if (bit::chk(enemy, curr)) {
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

                        threat |= 1UL << next;

                        if (bit::chk(player, next)) break;
                    }
                }
            }
        }
    }

    return threat;
}
vector<vertex> get_moves3 (Board &board, int side) {
    const int opp = side ^ 1;
    u64 player = 0, enemy = 0;
    vector<vertex> hist;

    for (int i = 0; i < 6; i++) {
        enemy |= board[opp][i], player |= board[side][i];
    }

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

                    if (board.is_inside(nx, ny)) {
                        const int next = move::index(nx, ny);

                        if (bit::chk(player, next)) break;

                        hist.push_back( {piece, curr, next});

                        if (bit::chk(enemy, next)) break;
                    }
                }
            }
        }
    }

    return hist;
}

int negamax (Board &board, int depth) {

    if (board[black][king] == 0) {
        // return 9999999;
    }

    if (depth == 0) {
        return board.count();
    }

    int maxv = -999;
    const vector<vertex> vs = get_moves3(board, white);

    for (auto &node : vs) {
        auto &[piece, curr, next] = node;
        const int enemy = board.player_id(black, next);

        change(board, node, enemy);

        int eval = -negamax (board, depth - 1);

        if (eval > maxv)
          maxv = eval;

        change(board, node, enemy);
    }

    return maxv;
}
int minimax2 (Board &board, int depth, int alpha, int beta, int mode) {

    if (depth == 0 || board[black][king] == 0) {
        return board.count();
    }
    const vector<vertex> vs = get_moves3(board, white);
    int maxv = -99999, minv = 99999;

    for (const auto &node : vs) {
        const auto &[piece, curr, next] = node;
        const int enemy = board.player_id(black, next);

        change(board, node, enemy);

        if (mode == true) {
            maxv = max(maxv, minimax2(board, depth - 1, alpha, beta, false));
            alpha = max(alpha, maxv);
        } else {
            minv = min(minv, minimax2(board, depth - 1, alpha, beta, true));
            beta = min(beta, minv);
        }

        change(board, node, enemy);

        if (mode == true && maxv >= beta) break;
        if (mode == false && minv <= alpha) break;
    }

    return mode == true ? maxv : minv;
}
vertex select3 (Board &board) {

    int maxv = -999999;
    vertex best;
    const vector<vertex> vs = get_moves3(board, white);

    const u64 th = threat(board, white);

    for (const auto &node : vs) {
        const auto &[piece, curr, next] = node;
        const int enemy = board.player_id(black, next);
        const int heur = (heuristic[piece][next] - heuristic[piece][curr]);

        if (bit::chk(th, next)) continue;

        change(board, node, enemy);

        const int eval = minimax2(board, 2, -999, 999, true) + heur;
        // int eval = negamax(board, 2);
        cout << Display::identify(piece) ;
        cout << "[" << move::x[curr] << " " << move::y[curr]  << "]";
        cout << "[" << move::x[next] << " " << move::y[next]  << "]";
        cout << eval << "\n";

        if (eval > maxv) {
            maxv = eval;
            best = node;
        }

        change(board, node, enemy);
    }


    return best;
}

string white_move (Board &board) {
    vertex node = select3(board);
    auto [piece, curr, next] = node;
    const int enemy = board.player_id(black, next);

    change(board, node, enemy);

    return format(piece, next);
}
string play (Board &board, const string &txt) {

    const auto [opp, post] = notation(txt);
    bool valid_move = false;

    for (auto &curr : move::get( move::identify(opp), post) ) {
        if (bit::chk(board[black][opp], curr)) {
            board[black][opp] ^= 1UL << curr;
            board[black][opp] ^= 1UL << post;
            valid_move = true;
            break;
        }
    }

    if (valid_move) {
        return white_move(board);
    } else {
        return "invalid move.";
    }

    return "";
}

int main () {

    Board board;
    auto [piece1, idx] = notation("Kc8");
    board[black][piece1] |= 1UL << idx;

    board.place("Ke8");
    board.place("Rh7");

    string txt, actual;

    vertex node = select3(board);
    auto [piece, curr, next] = node;
    const int enemy = board.player_id(black, next);

    change(board, node, enemy);
    // int cycle = 4;
    //
    // while (cycle-->0) {
    Display::limited(board);
    //     cout << "play your next move : \n";
    //
    //     cin >> txt;
    //
    //     actual = play(board, txt);
    // }

    std::cout << "\nexit\n";


}
