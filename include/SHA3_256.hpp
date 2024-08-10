#ifndef SHA3_256_HPP
#define SHA3_256_HPP


#include <iostream>
#include <array>
#include <cstring>
#include <string>
#include <cstdint>
class SHA3_256 {
public:
    SHA3_256();

    void update(const uint8_t* data, size_t len);

    uint8_t* finalize();

    static uint8_t* hash(const uint8_t* data, size_t len) {
        SHA3_256 sha3;
        sha3.update(data, len);
        
        uint8_t* hash = new uint8_t[32]; // Allocate memory on the heap
        std::memcpy(hash, sha3.finalize(), 32); // Copy the hash result to allocated memory

        return hash;
    }
    static std::string hash(const std::string& data) {
        SHA3_256 sha3;
        sha3.update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
        uint8_t* hash = sha3.finalize();
        std::string hashStr;
        hashStr.resize(32);
        std::memcpy(hashStr.data(), hash, 32);
        return hashStr;
    }

private:
    static constexpr size_t stateSize = 1600 / 8;           //200
    static constexpr size_t rateBytes = (1600 - 512) / 8;   //136
    static constexpr size_t capacityBits = 512;

    std::array<uint8_t, stateSize> state;
    size_t byteIndex = 0;

    void reset();
    void keccakF();
};

#endif // SHA3_256_HPP
