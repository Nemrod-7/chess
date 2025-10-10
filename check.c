#include <stdio.h>
#include <stdbool.h>

static inline bool is_inside(int x, int y) { return x >= 0 && y >= 0 && x < 8 && y < 8; }

bool threat (const char board[8][8], int x, int y) {
    //                         [         diagonal        ]  [         cross          ]
    const int compass[8][2] = {{-1,-1},{1,-1},{1,1},{-1,1}, {0,-1},{1,0},{0,1},{-1,0}};
    const int knight[8][2] = { {-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1} };

    for (int k = 0; k < 8; k++) {
        int nx = x + knight[k][0], ny = y + knight[k][1];

        if (is_inside(nx,ny) && board[ny][nx] == 'N') return true;

        for (int dist = 1; dist <= 8; dist++) {
            nx = x + compass[k][0] * dist, ny = y + compass[k][1] * dist;

            if (is_inside(nx,ny) && board[ny][nx] != ' ') {

                if (k < 2 && dist < 2 && board[ny][nx] == 'P') return true;

                if (k > 3) { 
                    if (board[ny][nx] == 'Q' || board[ny][nx] == 'R') return true;
                } else {
                    if (board[ny][nx] == 'Q' || board[ny][nx] == 'B') return true;
                }

                break;
            }
        }
    }

    return false;
}
bool king_is_in_check (const char board[8][8]) {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 'K') return threat(board, j,i);
        }        
    }

    return false;
}

void do_test(const char board[8][8], bool expect) {

    bool actual = king_is_in_check(board);

    if (actual != expect) {
        printf("wrong");
    }
}

int main() {

    do_test((char[8][8]) {
            "   B    ",
            "        ",
            "  K     ",
            "  N     ",
            "    P   ",
            "        ",
            "      Q ",
            "        "
            }, false);




}
