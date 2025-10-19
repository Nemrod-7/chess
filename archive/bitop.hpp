#pragma once

using u8 = unsigned short int;
using u64 = unsigned long long int;

namespace bit {
    const u8 index64[64] = {
        0,  1, 48,  2, 57, 49, 28,  3,
       61, 58, 50, 42, 38, 29, 17,  4,
       62, 55, 59, 36, 53, 51, 43, 22,
       45, 39, 33, 30, 24, 18, 12,  5,
       63, 47, 56, 27, 60, 41, 37, 16,
       54, 35, 52, 21, 44, 32, 23, 11,
       46, 26, 40, 15, 34, 20, 31, 10,
       25, 14, 19,  9, 13,  8,  7,  6
    };

    u8 ScanForward(u64 bb) {
       const u64 debruijn64 = u64(0x03f79d71b4cb0a89);
       // assert (bb != 0);
       return index64[((bb & -bb) * debruijn64) >> 58];
    }
    int ScanReverse(u64 bb) {
       union {
          double d;
          struct {
             unsigned int mantissal : 32;
             unsigned int mantissah : 20;
             unsigned int exponent : 11;
             unsigned int sign : 1;
          };
       } ud;
       ud.d = (double)(bb & ~(bb >> 32));  // avoid rounding error
       return ud.exponent - 1023;

    }
    bool chk (u64 num, u64 ix) { return num >> ix &1ull; }
    u64 set (u64 num, u64 ix) { return num | 1ull << ix; }
    u64 tog (u64 num, u64 ix) { return num ^ 1ull << ix; }
    u64 clr (u64 num, u64 ix) { return num & ~(1ull << ix); }

    u64 cnt (u64 num) {
        u64 cnt = 0;

        do { cnt += num &1; } while (num >>= 1);

        return cnt;
    }
    std::vector<u8> pos2 (u64 num) {
        u8 ix = 0;
        std::vector<u8> vs;

        do {
            if (num & 1) {
                vs.push_back(ix);
            }

            ix++;
        } while (num >>= 1);

        return vs;
    }

    std::vector<u8> pos (u64 x) {
        std::vector<u8> vs;

        while (x) {
            vs.push_back(bit::ScanForward(x));
            x &= x - 1; // reset LS1B
        }

        return vs;
    }

    u64 lsb (u64 x) { return x & -x; }
    u64 msb (u64 x) {
      x |= x >> 32;
      x |= x >> 16;
      x |= x >> 8;
      x |= x >> 4;
      x |= x >> 2;
      x |= x >> 1;
      x ^= x >> 1;
      return x;
    }
};
