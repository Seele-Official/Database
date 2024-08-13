#ifndef CRYPTO_STREAM_BUF_HPP
#define CRYPTO_STREAM_BUF_HPP
#include <iostream>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "AES.hpp"

class EncryptStreamBuf : public std::streambuf {
public:
    EncryptStreamBuf(std::ostream& output, std::string key256);
    EncryptStreamBuf(std::ostream& output, uint8_t* key256);

protected:
    int overflow(int c) override;
    int sync() override;

private:
    static const int bufferSize = 16;
    char buffer[bufferSize];
    std::ostream& outputStream;
    AES::AES_256 aes;

    int flushBuffer();
};


class DecryptStreamBuf : public std::streambuf {
public:
    DecryptStreamBuf(std::istream& input, std::string key256);
    DecryptStreamBuf(std::istream& input, uint8_t* key256);

protected:
    int underflow() override;

private:
    static const int bufferSize = 16;
    char buffer[bufferSize];
    std::istream& inputStream;
    AES::AES_256 aes;
};

#endif // CRYPTO_STREAM_HPP