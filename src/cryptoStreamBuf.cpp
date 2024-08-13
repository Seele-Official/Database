#include "cryptoStreamBuf.hpp"

EncryptStreamBuf::EncryptStreamBuf(std::ostream& output, std::string key256)
    : outputStream(output), aes(key256) {
    setp(buffer, buffer + bufferSize - 1);  // 设置缓冲区指针
}

EncryptStreamBuf::EncryptStreamBuf(std::ostream& output, uint8_t* key256)
    : outputStream(output), aes(key256) {
    setp(buffer, buffer + bufferSize - 1);  // 设置缓冲区指针
}

int EncryptStreamBuf::overflow(int c) {
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    return flushBuffer() == EOF ? EOF : c;
}

int EncryptStreamBuf::sync() {
    return flushBuffer() == EOF ? -1 : 0;
}

int EncryptStreamBuf::flushBuffer() {
    int num = pptr() - pbase();  // 计算缓冲区中的字符数
    if (num != 16) {
        // PKCS7 填充
        int padding = 16 - num;
        std::fill(pptr(), pptr() + padding, padding);
        std::cout << "Padding: " << padding << std::endl;
    }

    aes.encrypt(pbase());
    if (outputStream.write(pbase(), 16)) {  // 将缓冲区内容写入输出流
        pbump(-num);  // 重置缓冲区指针
        return num;
    }
    return EOF;
}

DecryptStreamBuf::DecryptStreamBuf(std::istream& input, std::string key256)
    : inputStream(input), aes(key256) {
    setg(buffer, buffer, buffer);  // 设置缓冲区指针
}

DecryptStreamBuf::DecryptStreamBuf(std::istream& input, uint8_t* key256)
    : inputStream(input), aes(key256) {
    setg(buffer, buffer, buffer);  // 设置缓冲区指针
}

int DecryptStreamBuf::underflow() {
    if (gptr() < egptr()) {
        return *gptr();
    }
    int num = inputStream.readsome(buffer, bufferSize);  // 从输入流读取数据
    if (num != bufferSize) {
        return EOF;
    }
    aes.decrypt(buffer);
    setg(buffer, buffer, buffer + num);
    return *gptr();
}
