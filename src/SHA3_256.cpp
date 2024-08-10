#include "SHA3_256.hpp"

SHA3_256::SHA3_256() {
    reset();
}

void SHA3_256::update(const uint8_t* data, size_t len) {
    while (len--) {
        state[byteIndex++] ^= *(data++);
        if (byteIndex == rateBytes) {
            keccakF();
            byteIndex = 0;
        }
    }
}

uint8_t* SHA3_256::finalize() {
    state[byteIndex] ^= 0x06;  // Padding (0x06)
    state[rateBytes - 1] ^= 0x80;  // Padding
    keccakF();

    return state.data();
}



void SHA3_256::reset() {
    std::fill(state.begin(), state.end(), 0);
    byteIndex = 0;
}

void SHA3_256::keccakF() {
    static const uint64_t roundConstants[24] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
        0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
        0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
        0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
        0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
        0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
        0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
    };

    uint64_t A[5][5] = {0};

    // Load state into A
    for (size_t y = 0; y < 5; y++) {
        for (size_t x = 0; x < 5; x++) {
            A[x][y] = 0;
            for (size_t b = 0; b < 8; b++) {
                A[x][y] |= uint64_t(state[8 * (x + 5 * y) + b]) << (8 * b);
            }
        }
    }

    for (int round = 0; round < 24; round++) {
        // Theta
        uint64_t C[5] = {0};
        for (int x = 0; x < 5; x++) {
            C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];
        }

        uint64_t D[5] = {0};
        for (int x = 0; x < 5; x++) {
            D[x] = C[(x + 4) % 5] ^ ((C[(x + 1) % 5] << 1) | (C[(x + 1) % 5] >> (64 - 1)));
        }

        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                A[x][y] ^= D[x];
            }
        }

        // Rho and Pi
        uint64_t B[5][5] = {0};
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                B[y][(2 * x + 3 * y) % 5] = (A[x][y] << (x + 5 * y)) | (A[x][y] >> (64 - (x + 5 * y)));
            }
        }

        // Chi
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 5; x++) {
                A[x][y] = B[x][y] ^ (~B[(x + 1) % 5][y] & B[(x + 2) % 5][y]);
            }
        }

        // Iota
        A[0][0] ^= roundConstants[round];
    }

    // Store A back to state
    for (size_t y = 0; y < 5; y++) {
        for (size_t x = 0; x < 5; x++) {
            for (size_t b = 0; b < 8; b++) {
                state[8 * (x + 5 * y) + b] = (A[x][y] >> (8 * b)) & 0xFF;
            }
        }
    }
}

