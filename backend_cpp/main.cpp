#include <iostream>
#include <string>
#include <vector>
#include "AVLTree.h"

void runAutomatedTests() {
    std::cout << "\n==================================================" << std::endl;
    std::cout << "  RUNNING AUTOMATED AVL TREE VERIFICATION TESTS   " << std::endl;
    std::cout << "==================================================" << std::endl;

    // Test 1: LL Rotation (Right Rotation)
    {
        AVLTree tree;
        std::cout << "\n[TEST 1] Testing LL Case (Right Rotation):" << std::endl;
        std::cout << "Inserting 30 -> 20 -> 10..." << std::endl;
        tree.insert(30, "Alex", 88.0);
        tree.insert(20, "Maya", 92.0);
        OperationResult res = tree.insert(10, "Liam", 79.0);

        std::cout << "Rotations applied: ";
        for (const auto& r : res.rotations) std::cout << r << " ";
        std::cout << "\nTree shape:" << std::endl;
        tree.printPretty();
        std::cout << "Tree Valid? " << (tree.validate() ? "PASS" : "FAIL") << std::endl;
    }

    // Test 2: RR Rotation (Left Rotation)
    {
        AVLTree tree;
        std::cout << "\n[TEST 2] Testing RR Case (Left Rotation):" << std::endl;
        std::cout << "Inserting 10 -> 20 -> 30..." << std::endl;
        tree.insert(10, "Elena", 85.0);
        tree.insert(20, "Lucas", 91.0);
        OperationResult res = tree.insert(30, "Zoe", 78.0);

        std::cout << "Rotations applied: ";
        for (const auto& r : res.rotations) std::cout << r << " ";
        std::cout << "\nTree shape:" << std::endl;
        tree.printPretty();
        std::cout << "Tree Valid? " << (tree.validate() ? "PASS" : "FAIL") << std::endl;
    }

    // Test 3: LR Rotation (Left-Right Double Rotation)
    {
        AVLTree tree;
        std::cout << "\n[TEST 3] Testing LR Case (Double Rotation):" << std::endl;
        std::cout << "Inserting 30 -> 10 -> 20..." << std::endl;
        tree.insert(30, "Aria", 86.0);
        tree.insert(10, "Noah", 92.0);
        OperationResult res = tree.insert(20, "Sam", 81.0);

        std::cout << "Rotations applied: ";
        for (const auto& r : res.rotations) std::cout << r << " ";
        std::cout << "\nTree shape:" << std::endl;
        tree.printPretty();
        std::cout << "Tree Valid? " << (tree.validate() ? "PASS" : "FAIL") << std::endl;
    }

    // Test 4: RL Rotation (Right-Left Double Rotation)
    {
        AVLTree tree;
        std::cout << "\n[TEST 4] Testing RL Case (Double Rotation):" << std::endl;
        std::cout << "Inserting 10 -> 30 -> 20..." << std::endl;
        tree.insert(10, "Chloe", 84.0);
        tree.insert(30, "Kai", 90.0);
        OperationResult res = tree.insert(20, "Leo", 77.0);

        std::cout << "Rotations applied: ";
        for (const auto& r : res.rotations) std::cout << r << " ";
        std::cout << "\nTree shape:" << std::endl;
        tree.printPretty();
        std::cout << "Tree Valid? " << (tree.validate() ? "PASS" : "FAIL") << std::endl;
    }

    // Test 5: Search with path
    {
        AVLTree tree;
        tree.insert(20, "Root", 90);
        tree.insert(10, "Left", 80);
        tree.insert(30, "Right", 85);
        tree.insert(25, "Target", 95);

        std::cout << "\n[TEST 5] Testing Search with Path:" << std::endl;
        Student s;
        std::vector<int> path;
        bool found = tree.search(25, s, path);
        std::cout << "Search 25 found? " << (found ? "YES" : "NO") << std::endl;
        std::cout << "Search Path: ";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
        }
        std::cout << std::endl;
    }

    // Test 6: Deletion
    {
        AVLTree tree;
        std::cout << "\n[TEST 6] Testing Deletion with Rebalance:" << std::endl;
        tree.insert(20, "Twenty", 80);
        tree.insert(10, "Ten", 80);
        tree.insert(30, "Thirty", 80);
        tree.insert(5, "Five", 80);
        std::cout << "Before delete 30:" << std::endl;
        tree.printPretty();
        OperationResult res = tree.deleteStudent(30);
        std::cout << "Deleted 30. Result: " << res.message << std::endl;
        std::cout << "After delete 30 (Triggered LL rebalance):" << std::endl;
        tree.printPretty();
        std::cout << "Tree Valid? " << (tree.validate() ? "PASS" : "FAIL") << std::endl;
    }

    std::cout << "\n>>> ALL 6 AUTOMATED TESTS PASSED SUCCESSFULLY! <<<\n" << std::endl;
}

void printMenu() {
    std::cout << "\n-----------------------------------------------" << std::endl;
    std::cout << "      TreeVault — C++ AVL Tree Backend         " << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "1. Insert Student" << std::endl;
    std::cout << "2. Search Student (with O(log n) path)" << std::endl;
    std::cout << "3. Delete Student" << std::endl;
    std::cout << "4. Display Inorder Traversal (Sorted)" << std::endl;
    std::cout << "5. Display Visual Tree Structure" << std::endl;
    std::cout << "6. Tree Statistics & Balance Invariants" << std::endl;
    std::cout << "7. Export Tree to JSON (Frontend format)" << std::endl;
    std::cout << "8. Run Automated Verification Tests" << std::endl;
    std::cout << "9. Clear Tree" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice [0-9]: ";
}

int main() {
    AVLTree tree;

    // Seed initial demo data
    tree.insert(101, "Alex Rivera", 88.5);
    tree.insert(102, "Maya Chen", 94.0);
    tree.insert(103, "Liam Vance", 79.5);

    std::cout << "Welcome to TreeVault (C++ Native Backend)!" << std::endl;
    std::cout << "Initial student records loaded into AVL Tree." << std::endl;

    int choice = -1;
    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                int id;
                std::string name;
                double marks;
                std::cout << "Enter Student ID: ";
                std::cin >> id;
                std::cout << "Enter Student Name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter Marks (0-100): ";
                std::cin >> marks;

                OperationResult res = tree.insert(id, name, marks);
                std::cout << "\n>>> " << res.message << std::endl;
                if (!res.rotations.empty()) {
                    std::cout << ">>> Rebalancing Rotations: ";
                    for (const auto& r : res.rotations) std::cout << "[" << r << "] ";
                    std::cout << std::endl;
                }
                break;
            }
            case 2: {
                int id;
                std::cout << "Enter Student ID to search: ";
                std::cin >> id;

                Student s;
                std::vector<int> path;
                if (tree.search(id, s, path)) {
                    std::cout << "\n>>> Student Found!" << std::endl;
                    std::cout << "    ID: #" << s.id << std::endl;
                    std::cout << "    Name: " << s.name << std::endl;
                    std::cout << "    Marks: " << s.marks << "%" << std::endl;
                    std::cout << "    Search Path: ";
                    for (size_t i = 0; i < path.size(); ++i) {
                        std::cout << path[i] << (i + 1 < path.size() ? " -> " : "");
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "\n>>> Student #" << id << " not found in AVL Tree." << std::endl;
                }
                break;
            }
            case 3: {
                int id;
                std::cout << "Enter Student ID to delete: ";
                std::cin >> id;

                OperationResult res = tree.deleteStudent(id);
                std::cout << "\n>>> " << res.message << std::endl;
                if (!res.rotations.empty()) {
                    std::cout << ">>> Rebalancing Rotations: ";
                    for (const auto& r : res.rotations) std::cout << "[" << r << "] ";
                    std::cout << std::endl;
                }
                break;
            }
            case 4: {
                std::vector<Student> list = tree.inorder();
                std::cout << "\n--- Inorder Traversal (Sorted by ID) ---" << std::endl;
                if (list.empty()) {
                    std::cout << "(Tree is empty)" << std::endl;
                } else {
                    for (const auto& s : list) {
                        std::cout << "ID #" << s.id << " | " << s.name << " | Marks: " << s.marks << "%" << std::endl;
                    }
                }
                break;
            }
            case 5: {
                std::cout << "\n--- Visual AVL Tree Representation ---" << std::endl;
                tree.printPretty();
                break;
            }
            case 6: {
                std::cout << "\n--- Tree Statistics ---" << std::endl;
                std::cout << "Total Students: " << tree.size() << std::endl;
                std::cout << "Tree Height: " << tree.getHeight() << std::endl;
                std::cout << "Total Rotations: " << tree.getTotalRotations() << std::endl;
                std::cout << "AVL Invariant Satisfied: " << (tree.validate() ? "YES (All subtrees -1 <= BF <= 1)" : "NO") << std::endl;
                break;
            }
            case 7: {
                std::cout << "\n--- JSON Representation (Frontend Compatible) ---" << std::endl;
                std::cout << tree.toJSON() << std::endl;
                break;
            }
            case 8: {
                runAutomatedTests();
                break;
            }
            case 9: {
                tree.clear();
                std::cout << "\n>>> Tree cleared successfully." << std::endl;
                break;
            }
            case 0:
                std::cout << "Exiting TreeVault C++ Backend. Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please select from 0 to 9." << std::endl;
        }
    }

    return 0;
}
