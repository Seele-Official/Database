#ifndef AVLTree_HPP
#define AVLTree_HPP
#include <iostream>
#include <utility>
#include <cstdint>
#include <typeinfo>

namespace avltree {
    template <typename T>
    class AVLTree
    {
    private:

        struct Node
        {
            Node *left;
            Node *right;
            uint32_t height;
            T data;
            void *operator new(size_t size)
            {
                void *ptr = ::operator new(size);
                std::cout << "Allocating " << size << " bytes at " << ptr << std::endl;
                return ptr;
            }
            void operator delete(void *ptr)
            {
                std::cout << "Deallocating at " << ptr << std::endl;
                ::operator delete(ptr);
            }
            Node() : left(nullptr), right(nullptr), height(0) {}
            Node(T &data) : data(data), left(nullptr), right(nullptr), height(1) {}
            Node(T &&data) : data(std::move(data)), left(nullptr), right(nullptr), height(1) {}
            ~Node() 
            {
                if (left != nullptr) delete left;
                if (right != nullptr) delete right;
            }
            friend std::ostream &operator<<(std::ostream &os, const Node *node)
            {
                if (node == nullptr)
                {
                    os << "null";
                    return os;
                }
                os << "{";
                os << "\"data\": \"" << '<' << typeid(node->data).name() << '>' <<  node->data <<"\", ";
                os << "\"height\": " << node->height << ", ";
                os << "\"left\": " << node->left << ", ";
                os << "\"right\": " << node->right;

                os << "}";
                return os;
            }
        };

        std::ostream &write(std::ostream &os, const Node *node, void (*writeData) (std::ostream&, const T&))
        {
            if (node == nullptr)
            {
                return os;
            }
            os.write(reinterpret_cast<const char*>(&node->height), sizeof(uint32_t));
            char child = 0;
            if (node->left != nullptr) child |= 0b11110000;
            if (node->right != nullptr) child |= 0b1111;
            os << child;
            writeData(os, node->data);
            write(os, node->left, writeData);
            write(os, node->right, writeData);
            return os;
        }
        
        std::istream &read(std::istream &is, Node *node , void (*readData) (std::istream&, T&))
        {
            if (node == nullptr)
            {
                return is;
            }
            is.read(reinterpret_cast<char*>(&node->height), sizeof(uint32_t));
            char child;
            is >> child;
            readData(is, node->data);
            if (child & 0b11110000)
            {
                node->left = new Node();
                read(is, node->left, readData);
            }
            if (child & 0b1111)
            {
                node->right = new Node();
                read(is, node->right, readData);
            }
            return is;
        }


        Node *root;
        uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }
        int32_t height(Node *node) { return node == nullptr ? 0 : node->height; }
        Node *rightRotate(Node *node){
            Node *temp = node->left;
            node->left = temp->right;
            temp->right = node;
            node->height = 1 + max(height(node->left), height(node->right));
            temp->height = 1 + max(height(temp->left), height(temp->right));
            return temp;
        }
        Node *leftRotate(Node *node){
            Node *temp = node->right;
            node->right = temp->left;
            temp->left = node;
            node->height = 1 + max(height(node->left), height(node->right));
            temp->height = 1 + max(height(temp->left), height(temp->right));
            return temp;
        }

        Node *balanceTree(Node *node){
            int32_t balance = height(node->left) - height(node->right);
            if (balance > 1){
                if (node->left->left != nullptr){
                    return rightRotate(node);
                }else{
                    node->left = leftRotate(node->left);
                    return rightRotate(node);
                }
                
            } else if (balance < -1){
                if (node->right->right != nullptr){
                    return leftRotate(node);
                } else {
                    node->right = rightRotate(node->right);
                    return leftRotate(node);
                }
            }
            return node;
        }
        Node* insert(Node *currentNode, T &data){
            if (currentNode == nullptr) return new Node(data);


            if (data < currentNode->data){

                currentNode->left = insert(currentNode->left, data);

            } else if (data > currentNode->data){

                currentNode->right = insert(currentNode->right, data);

            } else return currentNode;


            currentNode->height = 1 + max(height(currentNode->left), height(currentNode->right));
            
            return this->balanceTree(currentNode);
        }

        Node* remove(Node *currentNode, T &data){
            if (currentNode == nullptr) return nullptr;
            if (data < currentNode->data){
                currentNode->left = remove(currentNode->left, data);
            } else if (data > currentNode->data){
                currentNode->right = remove(currentNode->right, data);
            } else {
                

                if (currentNode->left == nullptr || currentNode->right == nullptr){
                    Node *temp = currentNode->left == nullptr ? currentNode->right : currentNode->left;
                    if (temp == nullptr){
                        delete currentNode;
                        return nullptr;
                    } else {
                        currentNode->left = nullptr;
                        currentNode->right = nullptr;
                        delete currentNode;
                        return temp;
                    }
                } else {
                    Node *temp = currentNode->left;
                    while (temp->right != nullptr) temp = temp->right;
                    currentNode->data = temp->data;
                    currentNode->left = remove(currentNode->left, temp->data);
                    return currentNode;
                }
                        
            }

            currentNode->height = 1 + max(height(currentNode->left), height(currentNode->right));
            return this->balanceTree(currentNode);
        }

    public:
        
        void insert(T &data) { root = insert(root, data); }
        void insert(T &&data) { insert(data); }
        void remove(T &data) { 
            Node *temp = remove(root, data);
            if (temp != nullptr) root = temp;
            else std::cerr << "Element not found" << std::endl;
        }
        void remove(T &&data) { remove(data); }
        bool find(T &data){
            Node *temp = root;
            while (temp != nullptr){
                if (data < temp->data) temp = temp->left;
                else if (data > temp->data) temp = temp->right;
                else return true;
            }
            return false;
        }
        bool find(T &&data) { return find(data); }

        friend std::ostream &operator<<(std::ostream &os, const AVLTree<T> &tree)
        {
            os << tree.root;
            return os;
        }
        
        
        std::ostream &write(std::ostream &os, void (*writeData) (std::ostream&, const T&))
        {
            
            write(os, this->root, writeData);
            return os;
        }
        std::ostream &write(std::ostream &os)
        {
            this->write(os, this->root, [](std::ostream &os, const T &data) { os.write(reinterpret_cast<const char*>(&data), sizeof(T)); 
            });
            return os;
        }
        std::istream &read(std::istream &is, void (*readData) (std::istream&, T&))
        {
            if (this->root != nullptr) delete this->root;
            this->root = new Node();
            read(is, this->root, readData);
            return is;
        }
        std::istream &read(std::istream &is)
        {
            if (this->root != nullptr) delete this->root;
            this->root = new Node();
            read(is, this->root, [](std::istream &is, T &data) { is.read(reinterpret_cast<char*>(&data), sizeof(T)); });
            return is;
        }
        AVLTree();
        ~AVLTree();
    };




    template <typename T>
    AVLTree<T>::AVLTree() : root(nullptr) {}
    template <typename T>
    AVLTree<T>::~AVLTree()
    {
        if (root != nullptr) delete root;
    }

}





#endif // AVLTree_HPP