#include <iostream>
#include <vector>

#include "board.hpp"
#include "moves.hpp"
#include "general.hpp"
#include "bitop.hpp"

#include <chrono>
#include <sstream>

class Timer {
    //using time = chrono::steady_clock;
    private :
        std::chrono::steady_clock::time_point alpha, end;
        std::chrono::duration<double> elapsed;
        uint64_t cycle;

        void update () {
            end = std::chrono::steady_clock::now ();
            elapsed = end - alpha;
        }
    public :
        Timer() {
            alpha = std::chrono::steady_clock::now ();
            cycle = 0;
        }
        void start () { alpha = std::chrono::steady_clock::now ();}
        void stop () { update();}
        void get_duration () {

            double duration = elapsed.count();
            const std::vector<std::pair<int,std::string>> base {{31536000,"year"},{86400,"day"}, {3600,"hour"},{60,"mn"}, {1,"s"}};
            int count = 0;
            std::vector<int> period;
            std::ostringstream os;

            for (auto it : base) {
                period.push_back (duration / it.first);

                if (period.back() != 0)
                    count++;

                duration -= (period.back() * it.first);
            }
            for (int i = 0; i < 5; ++i) {
                if (period[i] != 0) {
                    count--;
                    os << period[i] << " " << base[i].second;
                    // if (period[i] > 1) os << "s";
                    if (count > 1) os << ", ";
                    if (count == 1) os << " ";
                }
            }
            if (duration > 0.0) os << " " << int(duration * 60 * 1e3) / 1e3 << " ms";

            std::cout << "\nDuration " << os.str() << std::endl;
        }
        bool running (double total) {

            update();
            cycle++;
            if (elapsed.count() < total) return true;
            std::cout << "cycles :: " ;

            std::string num = std::to_string (cycle);
            int size = num.size(), index = 0;

            while (size-->0) {
                std::cout << num[index++];
                if (size % 3 == 0) std::cout << ' ';
            }
            std::cout << std::endl;
            return false;
        }
};

using namespace std;

// random_device rd;  //Will be used to obtain a seed for the random number engine
// mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

// vertex rnd_walk (vector<vertex> &vs) {
//     sort(vs.begin(), vs.end(), [](vertex a, vertex b) { return a.alt > b.alt; });
//     std::geometric_distribution dist;
//
//     return vs[dist(gen) % vs.size()];
// }

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

    if (piece == knight) {
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

u64 pawnmove (u8 ix, bool side) {
    const u8 row = move::y[ix];
    u64 bmask = 0ull;

    if (side == white) {
        bmask = (1ull << (ix - 8));
        if (row == 6) bmask |= (1ull << (ix - 16));
    } else {
        bmask = (1ull << (ix + 8));
        if (row == 1) bmask |= (1ull << (ix + 16));
    }
    return bmask;
}
u64 pawnattack (u8 ix, bool side) {
    if (side == white) {
        return (1ull << (ix - 9)) | (1ull << (ix - 7));
    } else {
        return (1ull << (ix + 9)) | (1ull << (ix + 7));
    }
}

void change_pos (Board &board, const vertex &node, int enemy) {
    const auto &[piece, curr, next] = node;

    if (enemy != -1) board[black][enemy] ^= 1UL << next;
    board[white][piece] ^= 1UL << curr;
    board[white][piece] ^= 1UL << next;
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
        u64 bmask;

        if (piece == pawn) {
            bmask = pawnmove(curr, side) | (pawnattack(curr, side) & enemy);
        } else {
            bmask = getmask(piece, curr, player | enemy);
        }
        bmask ^= (bmask & player);

        // display( bmask );
        for (auto &next : bit::pos(bmask)) {
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
        // display(bmask);
    }

    return vs;
}

int minimax2 (Board &board, int depth, int alpha, int beta, int mode) {

    if (depth == 0 || board[black][king] == 0) {
        return board.count();
    }
    const vector<vertex> vs = get_moves4(board, white);
    int maxv = -99999, minv = 99999;

    for (const auto &node : vs) {
        const auto &[piece, curr, next] = node;
        const int enemy = board.player_id(black, next);

        change_pos(board, node, enemy);

        if (mode == true) {
            maxv = max(maxv, minimax2(board, depth - 1, alpha, beta, false));
            alpha = max(alpha, maxv);
        } else {
            minv = min(minv, minimax2(board, depth - 1, alpha, beta, true));
            beta = min(beta, minv);
        }

        change_pos(board, node, enemy);

        if (mode == true && maxv >= beta) break;
        if (mode == false && minv <= alpha) break;
    }

    return mode == true ? maxv : minv;
}
vertex select3 (Board &board) {

    int maxv = -999999;
    vertex best;
    const vector<vertex> vs = get_moves4(board, white);

    const u64 th = threat_zone(board, black);

    for (const auto &node : vs) {
        const auto &[piece, curr, next] = node;
        const int enemy = board.player_id(black, next);
        const int heur = (heuristic[piece][next] - heuristic[piece][curr]);

        if (bit::chk(th, next)) continue;

        change_pos(board, node, enemy);

        const int eval = minimax2(board, 2, -999, 999, true) + heur ;

        cout << Display::identify(piece) ;
        cout << "[" << move::x[curr] << " " << move::y[curr]  << "]";
        cout << "[" << move::x[next] << " " << move::y[next]  << "]";
        cout << eval << "\n";

        if (eval > maxv) {
            maxv = eval;
            best = node;
        }

        change_pos(board, node, enemy);
    }


    return best;
}
string white_move (Board &board) {
    vertex node = select3(board);
    auto [piece, curr, next] = node;
    const int enemy = board.player_id(black, next);

    change_pos(board, node, enemy);

    return to_notation(piece, next);
}
string play (Board &board, const string &txt) {

    const auto [opp, post] = notation(txt);
    bool valid_move = false;
    //
    // for (auto &curr : move::get( move::identify(opp), post) ) {
    //     if (bit::chk(board[black][opp], curr)) {
    //         board[black][opp] ^= 1UL << curr;
    //         board[black][opp] ^= 1UL << post;
    //         valid_move = true;
    //         break;
    //     }
    // }

    if (valid_move) {
        return white_move(board);
    } else {
        return "invalid move.";
    }

    return "";
}

int main () {
  // black cannot make a valid move. There are three possibilities:
  //     Checkmate after at most 16 white moves: the test succeeds.
  //     Checkmate after more than 16 white moves: the test fails. Note that the test is not interrupted after 16 white moves so at least you can see how many moves it took.
  //     Stalemate: the test fails.
  //
  // The threefold position or 50-move rule applies: the test fails.
    Timer tt;

    Board board;
    int game = opening;
    int cycle = 0, fifty = 0;

    auto [piece1, idx] = notation("Kc8");
    board[black][piece1] |= 1UL << idx;

    board.place("Ke8");
    board.place("Rh7");

    if (!board[white][queen] || !board[black][queen]) {
        game = endgame;
    }

    white_move(board);

    Display::limited(board);
    // const u8 kgp = bit::pos(board[black][king])[0];
    // const bool th = threat(board, black, kgp);
    // cout << "[" << move::x[kgp] << " " << move::y[kgp]  << "]";
    // Display::limited(board);
    // switch (game) {
    //     case opening : std::cout << "opening"; break;
    //     case endgame : std::cout << "endgame"; break;
    //     case middlegame : std::cout << "middlegame"; break;
    // }
    // std::cout << "\n";
    // std::cout << "move : " << cycle  << " ";
    // cout << to_notation(king, kgp) << " ";

    tt.stop();
    tt.get_duration();
    cout << "\nend\n";
}
