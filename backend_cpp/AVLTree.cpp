#include "AVLTree.h"
#include <climits>
#include <iomanip>

AVLTree::AVLTree() : root(nullptr), totalRotations(0) {}

AVLTree::~AVLTree() {
    clear();
}

void AVLTree::destroyTree(AVLNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void AVLTree::clear() {
    destroyTree(root);
    root = nullptr;
}

int AVLTree::nodeHeight(AVLNode* node) const {
    return (node == nullptr) ? 0 : node->height;
}

int AVLTree::balanceFactor(AVLNode* node) const {
    return (node == nullptr) ? 0 : nodeHeight(node->left) - nodeHeight(node->right);
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node != nullptr) {
        node->height = 1 + std::max(nodeHeight(node->left), nodeHeight(node->right));
    }
}

// Right Rotation (LL Imbalance)
//       y                               x
//      / .     Right Rotation          / .
//     x   T3   -------------->        T1  y
//    / .                                 / .
//   T1  T2                              T2  T3
AVLNode* AVLTree::rotateRight(AVLNode* y, std::vector<std::string>& rotations) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    updateHeight(y);
    updateHeight(x);

    totalRotations++;
    std::stringstream ss;
    ss << "Right Rotation on Node #" << y->student.id;
    rotations.push_back(ss.str());

    return x; // New root of subtree
}

// Left Rotation (RR Imbalance)
//     x                                   y
//    / .         Left Rotation           / .
//   T1  y        -------------->        x   T3
//      / .                             / .
//     T2  T3                          T1  T2
AVLNode* AVLTree::rotateLeft(AVLNode* x, std::vector<std::string>& rotations) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    updateHeight(x);
    updateHeight(y);

    totalRotations++;
    std::stringstream ss;
    ss << "Left Rotation on Node #" << x->student.id;
    rotations.push_back(ss.str());

    return y; // New root of subtree
}

AVLNode* AVLTree::minValueNode(AVLNode* node) const {
    AVLNode* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

AVLNode* AVLTree::insertHelper(AVLNode* node, const Student& student, bool& isDuplicate, std::vector<std::string>& rotations) {
    // 1. Standard BST Insert
    if (node == nullptr) {
        return new AVLNode(student);
    }

    if (student.id < node->student.id) {
        node->left = insertHelper(node->left, student, isDuplicate, rotations);
    } else if (student.id > node->student.id) {
        node->right = insertHelper(node->right, student, isDuplicate, rotations);
    } else {
        isDuplicate = true;
        return node;
    }

    // 2. Update Height of this ancestor node
    updateHeight(node);

    // 3. Get Balance Factor
    int balance = balanceFactor(node);

    // 4. Rebalance if imbalanced (|balance| > 1)
    
    // Case 1: LL (Left-Left)
    if (balance > 1 && student.id < node->left->student.id) {
        return rotateRight(node, rotations);
    }

    // Case 2: RR (Right-Right)
    if (balance < -1 && student.id > node->right->student.id) {
        return rotateLeft(node, rotations);
    }

    // Case 3: LR (Left-Right)
    if (balance > 1 && student.id > node->left->student.id) {
        node->left = rotateLeft(node->left, rotations);
        return rotateRight(node, rotations);
    }

    // Case 4: RL (Right-Left)
    if (balance < -1 && student.id < node->right->student.id) {
        node->right = rotateRight(node->right, rotations);
        return rotateLeft(node, rotations);
    }

    return node;
}

OperationResult AVLTree::insert(int id, const std::string& name, double marks) {
    return insert(Student(id, name, marks));
}

OperationResult AVLTree::insert(const Student& student) {
    OperationResult res;
    bool isDuplicate = false;
    std::vector<std::string> rotations;

    root = insertHelper(root, student, isDuplicate, rotations);

    if (isDuplicate) {
        res.success = false;
        res.message = "Student ID already exists in the AVL Tree.";
        res.rotations = {};
    } else {
        res.success = true;
        res.message = "Student inserted successfully into AVL Tree.";
        res.rotations = rotations;
    }
    return res;
}

AVLNode* AVLTree::deleteHelper(AVLNode* node, int id, bool& found, std::vector<std::string>& rotations) {
    // 1. Standard BST Deletion
    if (node == nullptr) {
        return nullptr;
    }

    if (id < node->student.id) {
        node->left = deleteHelper(node->left, id, found, rotations);
    } else if (id > node->student.id) {
        node->right = deleteHelper(node->right, id, found, rotations);
    } else {
        // Node found
        found = true;

        // Node with only one child or no child
        if (node->left == nullptr || node->right == nullptr) {
            AVLNode* temp = node->left ? node->left : node->right;

            // No child case
            if (temp == nullptr) {
                temp = node;
                node = nullptr;
            } else { // One child case
                *node = *temp; // Copy contents
            }
            delete temp;
        } else {
            // Node with two children: Get Inorder Successor (min in right subtree)
            AVLNode* temp = minValueNode(node->right);
            node->student = temp->student;
            // Delete the successor from right subtree
            node->right = deleteHelper(node->right, temp->student.id, found, rotations);
        }
    }

    // If the tree had only one node then return
    if (node == nullptr) {
        return nullptr;
    }

    // 2. Update Height
    updateHeight(node);

    // 3. Get Balance Factor
    int balance = balanceFactor(node);

    // 4. Rebalance if needed
    // Case 1: LL
    if (balance > 1 && balanceFactor(node->left) >= 0) {
        return rotateRight(node, rotations);
    }

    // Case 2: LR
    if (balance > 1 && balanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left, rotations);
        return rotateRight(node, rotations);
    }

    // Case 3: RR
    if (balance < -1 && balanceFactor(node->right) <= 0) {
        return rotateLeft(node, rotations);
    }

    // Case 4: RL
    if (balance < -1 && balanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right, rotations);
        return rotateLeft(node, rotations);
    }

    return node;
}

OperationResult AVLTree::deleteStudent(int id) {
    OperationResult res;
    bool found = false;
    std::vector<std::string> rotations;

    root = deleteHelper(root, id, found, rotations);

    if (!found) {
        res.success = false;
        res.message = "Student ID not found in the AVL Tree.";
        res.rotations = {};
    } else {
        res.success = true;
        res.message = "Student record deleted successfully from AVL Tree.";
        res.rotations = rotations;
    }
    return res;
}

bool AVLTree::search(int id, Student& outStudent, std::vector<int>& outPath) const {
    outPath.clear();
    AVLNode* current = root;

    while (current != nullptr) {
        outPath.push_back(current->student.id);

        if (id == current->student.id) {
            outStudent = current->student;
            return true;
        } else if (id < current->student.id) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return false;
}

void AVLTree::inorderHelper(AVLNode* node, std::vector<Student>& result) const {
    if (node != nullptr) {
        inorderHelper(node->left, result);
        result.push_back(node->student);
        inorderHelper(node->right, result);
    }
}

std::vector<Student> AVLTree::inorder() const {
    std::vector<Student> res;
    inorderHelper(root, res);
    return res;
}

void AVLTree::preorderHelper(AVLNode* node, std::vector<Student>& result) const {
    if (node != nullptr) {
        result.push_back(node->student);
        preorderHelper(node->left, result);
        preorderHelper(node->right, result);
    }
}

std::vector<Student> AVLTree::preorder() const {
    std::vector<Student> res;
    preorderHelper(root, res);
    return res;
}

void AVLTree::postorderHelper(AVLNode* node, std::vector<Student>& result) const {
    if (node != nullptr) {
        postorderHelper(node->left, result);
        postorderHelper(node->right, result);
        result.push_back(node->student);
    }
}

std::vector<Student> AVLTree::postorder() const {
    std::vector<Student> res;
    postorderHelper(root, res);
    return res;
}

int AVLTree::getHeight() const {
    return nodeHeight(root);
}

int AVLTree::countNodes(AVLNode* node) const {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

int AVLTree::size() const {
    return countNodes(root);
}

int AVLTree::getTotalRotations() const {
    return totalRotations;
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

bool AVLTree::isBalanced() const {
    return validate();
}

bool AVLTree::validateBSTHelper(AVLNode* node, long long minVal, long long maxVal) const {
    if (node == nullptr) return true;
    if (node->student.id <= minVal || node->student.id >= maxVal) return false;
    return validateBSTHelper(node->left, minVal, node->student.id) &&
           validateBSTHelper(node->right, node->student.id, maxVal);
}

bool AVLTree::validateHeightsHelper(AVLNode* node) const {
    if (node == nullptr) return true;
    
    int bf = balanceFactor(node);
    if (bf < -1 || bf > 1) return false;

    int expectedHeight = 1 + std::max(nodeHeight(node->left), nodeHeight(node->right));
    if (node->height != expectedHeight) return false;

    return validateHeightsHelper(node->left) && validateHeightsHelper(node->right);
}

bool AVLTree::validate() const {
    return validateBSTHelper(root, LLONG_MIN, LLONG_MAX) && validateHeightsHelper(root);
}

void AVLTree::toJSONHelper(AVLNode* node, std::stringstream& ss) const {
    if (node == nullptr) {
        ss << "null";
        return;
    }

    ss << "{\"id\":" << node->student.id
       << ",\"name\":\"" << node->student.name << "\""
       << ",\"marks\":" << node->student.marks
       << ",\"height\":" << node->height
       << ",\"balance_factor\":" << balanceFactor(node)
       << ",\"left\":";
    toJSONHelper(node->left, ss);
    ss << ",\"right\":";
    toJSONHelper(node->right, ss);
    ss << "}";
}

std::string AVLTree::toJSON() const {
    std::stringstream ss;
    toJSONHelper(root, ss);
    return ss.str();
}

void AVLTree::printPrettyHelper(AVLNode* node, const std::string& prefix, bool isLeft) const {
    if (node != nullptr) {
        std::cout << prefix;
        std::cout << (isLeft ? "├── " : "└── ");
        std::cout << "#" << node->student.id << " (" << node->student.name << ", " 
                  << node->student.marks << "%) [h=" << node->height 
                  << ", BF=" << balanceFactor(node) << "]" << std::endl;

        printPrettyHelper(node->left, prefix + (isLeft ? "│   " : "    "), true);
        printPrettyHelper(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }
}

void AVLTree::printPretty() const {
    if (root == nullptr) {
        std::cout << "(Empty Tree)" << std::endl;
        return;
    }
    printPrettyHelper(root, "", false);
}
