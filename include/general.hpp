#pragma once

#include <string>
#include <vector>
#include "moves.hpp"
#include "bitmanip.hpp"

enum {black, white};
enum {opening, middlegame, endgame};
enum {pawn, bishop, knight, rook, queen, king};

struct vertex { int type, now, nxt; };

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
    //     { // king opening
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
        // king endgame
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
std::string to_notation (u8 piece, u8 curr) {
    std::string place = "   ";
    u8 x = move::x[curr] , y = move::y[curr];

    switch (piece) {
        case pawn : place[0] = 'P'; break;
        case rook : place[0] = 'R'; break;
        case king : place[0] = 'K'; break;
        case queen : place[0] = 'Q'; break;
        case bishop : place[0] = 'B'; break;
        case knight : place[0] = 'N'; break;
    }

    place[1] = x + 97;
    place[2] = (8 - y) + 48;
    return place;
}
