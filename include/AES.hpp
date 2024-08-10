#ifndef AES_HPP
#define AES_HPP
#include <string>
#include <cstdint>
#include <stdexcept>
namespace AES {
    class AES_256 {
    private:
        uint32_t key[8];
        // uint32_t iv[4];
        uint32_t roundKeys[16][4];


        void keyExpansion();
        void pkcs7_padding(std::string &data);
        void pkcs7_unpadding(std::string &data);


        void ivXOR(uint32_t *state, uint32_t *iv);

        uint8_t GF_mul(uint8_t a, uint8_t b);
        void rightRotate(uint32_t *state, int n);
        void leftRotate(uint32_t *state, int n);

        void subBytes(uint32_t *state);
        void shiftRows(uint32_t *state);
        void mixColumns(uint32_t *state);
        void addRoundKey(uint32_t *state, uint32_t *roundKey);


        void invSubBytes(uint32_t *state);
        void invShiftRows(uint32_t *state);
        void invMixColumns(uint32_t *state);
        void invAddRoundKey(uint32_t *state, uint32_t *roundKey);


        



        void round(uint32_t *state, uint32_t *roundKey);
        void invRound(uint32_t *state, uint32_t *roundKey);

    public:
        // AES_256(const std::string &key, const std::string &iv);
        AES_256(const std::string &key);
        AES_256(const uint8_t *key);

        std::string encrypt(std::string plainText);
        std::string decrypt(std::string cipherText);

        template <typename T>
        void encrypt(T *plainText){
            uint32_t *state = reinterpret_cast<uint32_t *>(plainText);
            addRoundKey(state, roundKeys[0]);
            for (int j = 1; j < 15; j++) {
                round(state, roundKeys[j]);
            }
            subBytes(state);
            shiftRows(state);
            addRoundKey(state, roundKeys[15]);
        }
        template <typename T>
        void decrypt(T *cipherText){
            uint32_t *state = reinterpret_cast<uint32_t *>(cipherText);
            addRoundKey(state, roundKeys[15]);
            invShiftRows(state);
            invSubBytes(state);
            for (int j = 14; j > 0; j--) {
                invAddRoundKey(state, roundKeys[j]);
                invMixColumns(state);
                invShiftRows(state);
                invSubBytes(state);
            }
            addRoundKey(state, roundKeys[0]);
        }
    };
}

#endif // AES_HPP