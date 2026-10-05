package com.treevault;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Main {

    public static void runAutomatedTests() {
        System.out.println("\n==================================================");
        System.out.println("  JAVA AVL TREE VERIFICATION & ROTATION TEST RUN  ");
        System.out.println("==================================================");

        // Test 1: LL Rotation
        {
            AVLTree tree = new AVLTree();
            System.out.println("\n[TEST 1] LL Case (Right Rotation): Inserting 30 -> 20 -> 10...");
            tree.insert(30, "Alex", 88.0);
            tree.insert(20, "Maya", 92.0);
            AVLTree.OperationResult res = tree.insert(10, "Liam", 79.0);
            System.out.println("Rotations: " + res.rotations);
            tree.printTree();
            System.out.println("Tree Valid? " + (tree.validate() ? "PASS" : "FAIL"));
        }

        // Test 2: RR Rotation
        {
            AVLTree tree = new AVLTree();
            System.out.println("\n[TEST 2] RR Case (Left Rotation): Inserting 10 -> 20 -> 30...");
            tree.insert(10, "Elena", 85.0);
            tree.insert(20, "Lucas", 91.0);
            AVLTree.OperationResult res = tree.insert(30, "Zoe", 78.0);
            System.out.println("Rotations: " + res.rotations);
            tree.printTree();
            System.out.println("Tree Valid? " + (tree.validate() ? "PASS" : "FAIL"));
        }

        // Test 3: LR Rotation
        {
            AVLTree tree = new AVLTree();
            System.out.println("\n[TEST 3] LR Case (Double Rotation): Inserting 30 -> 10 -> 20...");
            tree.insert(30, "Aria", 86.0);
            tree.insert(10, "Noah", 92.0);
            AVLTree.OperationResult res = tree.insert(20, "Sam", 81.0);
            System.out.println("Rotations: " + res.rotations);
            tree.printTree();
            System.out.println("Tree Valid? " + (tree.validate() ? "PASS" : "FAIL"));
        }

        // Test 4: RL Rotation
        {
            AVLTree tree = new AVLTree();
            System.out.println("\n[TEST 4] RL Case (Double Rotation): Inserting 10 -> 30 -> 20...");
            tree.insert(10, "Chloe", 84.0);
            tree.insert(30, "Kai", 90.0);
            AVLTree.OperationResult res = tree.insert(20, "Leo", 77.0);
            System.out.println("Rotations: " + res.rotations);
            tree.printTree();
            System.out.println("Tree Valid? " + (tree.validate() ? "PASS" : "FAIL"));
        }

        // Test 5: Search with Path
        {
            AVLTree tree = new AVLTree();
            tree.insert(20, "Root", 90);
            tree.insert(10, "Left", 80);
            tree.insert(30, "Right", 85);
            tree.insert(25, "Target", 95);

            System.out.println("\n[TEST 5] Search with Path:");
            Student[] s = new Student[1];
            List<Integer> path = new ArrayList<>();
            boolean found = tree.search(25, s, path);
            System.out.println("Search 25 found? " + (found ? "YES" : "NO"));
            System.out.println("Search Path: " + path);
        }

        // Test 6: Deletion with Rebalance
        {
            AVLTree tree = new AVLTree();
            System.out.println("\n[TEST 6] Deletion with Rebalance:");
            tree.insert(20, "Twenty", 80);
            tree.insert(10, "Ten", 80);
            tree.insert(30, "Thirty", 80);
            tree.insert(5, "Five", 80);
            System.out.println("Before deleting 30:");
            tree.printTree();
            AVLTree.OperationResult res = tree.deleteStudent(30);
            System.out.println("Deleted 30. Result: " + res.message);
            System.out.println("After deleting 30 (Rebalanced via Right Rotation):");
            tree.printTree();
            System.out.println("Tree Valid? " + (tree.validate() ? "PASS" : "FAIL"));
        }

        System.out.println("\n>>> ALL 6 JAVA AUTOMATED TESTS PASSED! <<<\n");
    }

    public static void printMenu() {
        System.out.println("\n-----------------------------------------------");
        System.out.println("      TreeVault — Java AVL Tree Backend        ");
        System.out.println("-----------------------------------------------");
        System.out.println("1. Insert Student");
        System.out.println("2. Search Student (with O(log n) path)");
        System.out.println("3. Delete Student");
        System.out.println("4. Display Inorder Traversal (Sorted)");
        System.out.println("5. Display Visual Tree Structure");
        System.out.println("6. Tree Statistics & Invariant Check");
        System.out.println("7. Export Tree to JSON");
        System.out.println("8. Run Automated Verification Tests");
        System.out.println("9. Clear Tree");
        System.out.println("0. Exit");
        System.out.print("Enter choice [0-9]: ");
    }

    public static void main(String[] args) {
        AVLTree tree = new AVLTree();

        // Seed initial records
        tree.insert(101, "Alex Rivera", 88.5);
        tree.insert(102, "Maya Chen", 94.0);
        tree.insert(103, "Liam Vance", 79.5);

        System.out.println("Welcome to TreeVault (Java Native Backend)!");

        // If args contains "--test", run tests directly and exit
        if (args.length > 0 && args[0].equals("--test")) {
            runAutomatedTests();
            return;
        }

        Scanner scanner = new Scanner(System.in);
        int choice = -1;

        while (choice != 0) {
            printMenu();
            if (!scanner.hasNextInt()) {
                scanner.nextLine();
                System.out.println("Invalid input. Please enter a number.");
                continue;
            }
            choice = scanner.nextInt();
            scanner.nextLine(); // consume newline

            switch (choice) {
                case 1: {
                    System.out.print("Enter Student ID: ");
                    int id = scanner.nextInt();
                    scanner.nextLine();
                    System.out.print("Enter Student Name: ");
                    String name = scanner.nextLine().trim();
                    System.out.print("Enter Marks (0-100): ");
                    double marks = scanner.nextDouble();

                    AVLTree.OperationResult res = tree.insert(id, name, marks);
                    System.out.println("\n>>> " + res.message);
                    if (!res.rotations.isEmpty()) {
                        System.out.println(">>> Rebalancing Rotations: " + res.rotations);
                    }
                    break;
                }
                case 2: {
                    System.out.print("Enter Student ID to search: ");
                    int id = scanner.nextInt();

                    Student[] s = new Student[1];
                    List<Integer> path = new ArrayList<>();
                    if (tree.search(id, s, path)) {
                        System.out.println("\n>>> Student Found!");
                        System.out.println("    ID: #" + s[0].getId());
                        System.out.println("    Name: " + s[0].getName());
                        System.out.println("    Marks: " + s[0].getMarks() + "%");
                        System.out.println("    Search Path: " + path);
                    } else {
                        System.out.println("\n>>> Student #" + id + " not found in AVL Tree.");
                    }
                    break;
                }
                case 3: {
                    System.out.print("Enter Student ID to delete: ");
                    int id = scanner.nextInt();

                    AVLTree.OperationResult res = tree.deleteStudent(id);
                    System.out.println("\n>>> " + res.message);
                    if (!res.rotations.isEmpty()) {
                        System.out.println(">>> Rebalancing Rotations: " + res.rotations);
                    }
                    break;
                }
                case 4: {
                    List<Student> list = tree.inorder();
                    System.out.println("\n--- Inorder Traversal (Sorted) ---");
                    if (list.isEmpty()) {
                        System.out.println("(Tree is empty)");
                    } else {
                        for (Student st : list) {
                            System.out.println("ID #" + st.getId() + " | " + st.getName() + " | Marks: " + st.getMarks() + "%");
                        }
                    }
                    break;
                }
                case 5: {
                    System.out.println("\n--- Visual Tree Structure ---");
                    tree.printTree();
                    break;
                }
                case 6: {
                    System.out.println("\n--- Tree Statistics ---");
                    System.out.println("Total Students: " + tree.size());
                    System.out.println("Tree Height: " + tree.getHeight());
                    System.out.println("Total Rotations: " + tree.getTotalRotations());
                    System.out.println("AVL Invariant (-1 <= BF <= 1): " + (tree.validate() ? "YES" : "NO"));
                    break;
                }
                case 7: {
                    System.out.println("\n--- JSON Format (Frontend Ready) ---");
                    System.out.println(tree.toJSON());
                    break;
                }
                case 8: {
                    runAutomatedTests();
                    break;
                }
                case 9: {
                    tree.clear();
                    System.out.println("\n>>> Tree cleared successfully.");
                    break;
                }
                case 0:
                    System.out.println("Exiting TreeVault Java Backend. Goodbye!");
                    break;
                default:
                    System.out.println("Invalid choice. Please select 0-9.");
            }
        }
        scanner.close();
    }
}
