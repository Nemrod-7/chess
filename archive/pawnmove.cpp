#include <iostream>
#include <vector>

using namespace std;

using u8 = unsigned short int;
using u64 = unsigned long long int;

enum {white, black};
enum {pawn, rook, bishop, knight, queen, king};

struct vertex { int piece, curr, next; };

namespace bit {
  u64 chk (u64 grid, u64 ix) { return (grid >> ix) & 1ull; }
}

void display(u64 grid) {
    for (u64 i = 0; i < 64; i++) {
        if (i > 0 && i % 8 == 0) cout << "\n";
        cout << bit::chk(grid, i);
    }
    cout << "\n";
}

u64 pawnattack (int ix, int side) {
    if (side == white) {
        return (1ull << (ix - 9)) | (1ull << (ix - 7));
    } else {
        return (1ull << (ix + 9)) | (1ull << (ix + 7));
    }
}
u64 pawnmove (int ix, int side) {
    const int row = ix / 8;
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

int main () {

    const int side = white;
    const int ix = 42;

    const int oppo = side ^ 1;
    const int row = ix / 8;

    u64 player = 0ull, enemy = 0ull;
    enemy |= 1ull << (ix - 9);

    u64 bmask = (pawnattack(ix, side) & enemy) | (pawnmove(ix, side) ^ player);
    display( bmask );

}
