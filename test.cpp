
#include <iostream>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <fstream>
#include <windows.h>
#include <string>
#include "AES.hpp"
#include "AVLTree.hpp"
#include "SHA3_256.hpp"


void consolestrTest(char* data, size_t len){
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        std::cout << hex[data[i] >> 4] << hex[data[i] & 0xF];
    }
    std::cout << std::endl;
}

class EncryptStreamBuf : public std::streambuf {
public:
    EncryptStreamBuf(std::ostream& output, std::string key256) : outputStream(output), aes(key256)
    {
        setp(buffer, buffer + bufferSize - 1);  // 设置缓冲区指针
    }

protected:

    int overflow(int c) override {
        if (c != EOF) {
            *pptr() = c; 
            pbump(1);
        }
        return flushBuffer() == EOF ? EOF : c;
    }


    int sync() override {
        return flushBuffer() == EOF ? -1 : 0;
    }

private:
    static const int bufferSize = 16;
    char buffer[bufferSize];
    std::ostream& outputStream;
    AES::AES_256 aes;

    int flushBuffer() {
        int num = pptr() - pbase(); // 计算缓冲区中的字符数
        if (num != 16) {
            // PKCS7 填充
            int padding = 16 - num;
            std::fill(pptr(), pptr() + padding, padding);
            std::cout << "Padding: " << padding << std::endl;
        }
        
        std::cout << "Encrypted: ";
        consolestrTest(pbase(), 16);
        std::cout << " => ";
        aes.encrypt(pbase());
        consolestrTest(pbase(), 16);
        if (outputStream.write(pbase(), 16)) { // 将缓冲区内容写入输出流
            pbump(-num); // 重置缓冲区指针
            return num;
        }
        return EOF;
    }
};


class DecryptStreamBuf : public std::streambuf {
private:
    static const int bufferSize = 16;
    char buffer[bufferSize];
    std::istream& inputStream;
    AES::AES_256 aes;
protected:
    int underflow() override {
        if (gptr() < egptr()) {
            return *gptr();
        }
        int num = inputStream.readsome(buffer, bufferSize); // 从输入流读取数据
        if (num != bufferSize) {
            return EOF;
        }
        aes.decrypt(buffer);
        setg(buffer, buffer, buffer + num);
        return *gptr();
    }
public:
    DecryptStreamBuf(std::istream& input, std::string key256) : inputStream(input), aes(key256)
    {
        setg(buffer, buffer, buffer);  // 设置缓冲区指针
    }



};
int main() {
    SetConsoleCP(CP_UTF8);

    
    std::string key;
    
    std::cout << "Enter key: ";
    std::cin >> key;

    std::string key_sha256;
    key_sha256 = std::move(SHA3_256::hash(key));
    std::cout << "SHA3-256: ";
    consolestrTest(key_sha256.data(), 32);





    std::ofstream file("test.bin", std::ios::binary);
    file.flush();
    EncryptStreamBuf customBuf(file, key_sha256);
    std::ostream customStream(&customBuf);

    std::string data;
    std::cout << "Enter data: ";
    std::cin >> data;
    customStream << data; 

    customStream.flush(); // 刷新流，确保所有内容都输出
    file.close();



    std::cout << "Reading encrypted data from file..." << std::endl;

    std::string plainText;
    plainText.resize(data.size());

    std::ifstream file2("test.bin", std::ios::binary);
    DecryptStreamBuf customBuf2(file2, key_sha256);
    std::istream customStream2(&customBuf2);
    customStream2.read(plainText.data(), plainText.size());
    

    std::cout << "Decrypted: ";
    std::cout << plainText << std::endl;


    return 0;
}
