
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
#include "cryptoStreamBuf.hpp"






void consolestrTest(char* data, size_t len){
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        std::cout << hex[data[i] >> 4] << hex[data[i] & 0xF];
    }
    std::cout << std::endl;
}

template <typename T>
avltree::AVLTree<T> createTree(T *data, size_t len)
{
    avltree::AVLTree<T> tree;
    for (size_t i = 0; i < len; i++)
    {
        tree.insert(data[i]);
    }
    return tree;
}

avltree::AVLTree<std::string> createTree(std::string *data, size_t len)
{
    avltree::AVLTree<std::string> tree;
    for (size_t i = 0; i < len; i++)
    {
        std::cout << "inserting: ";
        std::cout.write(data[i].data(), data[i].size());
        std::cout << std::endl;
        tree.insert(data[i]);
    }
    return tree;
}






int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    
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

    std::cout << "Enter data :" << std::endl;

    std::string data[10];
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << "Data " << i << ": ";
        std::cin >> data[i];
    }
    auto tree = createTree(data, 10);

    auto funcwrite = [](std::ostream& os, const std::string& data){
        size_t len = data.size();
        os.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        os.write(data.data(), len);
        std::cout << "Writing data: " << data << " len: " << len << std::endl;
    };

    tree.write(customStream, funcwrite);
    customStream.flush(); 
    file.close();



    std::cout << "Reading encrypted data from file..." << std::endl;




    std::ifstream file2("test.bin", std::ios::binary);
    DecryptStreamBuf customBuf2(file2, key_sha256);
    std::istream customStream2(&customBuf2);
    
    

    auto funcread = [](std::istream& is, std::string& data){
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(size_t));
        data.resize(len);
        is.read(data.data(), len);
        std::cout << "Reading data: " << data << " len: " << len << std::endl;
    };
    avltree::AVLTree<std::string> tree2;
    tree2.read(customStream2, funcread);
    file2.close();
    std::cout << "Data read from file: " << std::endl;
    std::cout << tree2 << std::endl;
    return 0;
}
