#ifndef AVLTREE_H
#define AVLTREE_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>

// Represents a Student record
struct Student {
    int id;
    std::string name;
    double marks;

    Student() : id(0), name(""), marks(0.0) {}
    Student(int id, const std::string& name, double marks) : id(id), name(name), marks(marks) {}
};

// Represents a Node in the AVL Tree
struct AVLNode {
    Student student;
    int height;
    AVLNode* left;
    AVLNode* right;

    AVLNode(const Student& s)
        : student(s), height(1), left(nullptr), right(nullptr) {}
};

// Return structure for insert/delete operations detailing applied rotations
struct OperationResult {
    bool success;
    std::string message;
    std::vector<std::string> rotations;
};

// Complete Self-Balancing AVL Tree Implementation
class AVLTree {
private:
    AVLNode* root;
    int totalRotations;

    // Helper functions
    int nodeHeight(AVLNode* node) const;
    int balanceFactor(AVLNode* node) const;
    void updateHeight(AVLNode* node);
    
    AVLNode* rotateRight(AVLNode* y, std::vector<std::string>& rotations);
    AVLNode* rotateLeft(AVLNode* x, std::vector<std::string>& rotations);
    AVLNode* minValueNode(AVLNode* node) const;

    AVLNode* insertHelper(AVLNode* node, const Student& student, bool& isDuplicate, std::vector<std::string>& rotations);
    AVLNode* deleteHelper(AVLNode* node, int id, bool& found, std::vector<std::string>& rotations);
    
    void inorderHelper(AVLNode* node, std::vector<Student>& result) const;
    void preorderHelper(AVLNode* node, std::vector<Student>& result) const;
    void postorderHelper(AVLNode* node, std::vector<Student>& result) const;
    
    int countNodes(AVLNode* node) const;
    void destroyTree(AVLNode* node);
    void toJSONHelper(AVLNode* node, std::stringstream& ss) const;
    
    bool validateBSTHelper(AVLNode* node, long long minVal, long long maxVal) const;
    bool validateHeightsHelper(AVLNode* node) const;

public:
    AVLTree();
    ~AVLTree();

    // Core AVL Operations
    OperationResult insert(int id, const std::string& name, double marks);
    OperationResult insert(const Student& student);
    OperationResult deleteStudent(int id);
    
    bool search(int id, Student& outStudent, std::vector<int>& outPath) const;
    
    // Traversals
    std::vector<Student> inorder() const;
    std::vector<Student> preorder() const;
    std::vector<Student> postorder() const;

    // Tree Metrics
    int getHeight() const;
    int size() const;
    int getTotalRotations() const;
    bool isBalanced() const;
    bool isEmpty() const;
    void clear();

    // Validation & Export
    bool validate() const;
    std::string toJSON() const;
    void printPretty() const;
    void printPrettyHelper(AVLNode* node, const std::string& prefix, bool isLeft) const;
};

#endif // AVLTREE_H
