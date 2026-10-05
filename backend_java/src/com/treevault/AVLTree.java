package com.treevault;

import java.util.ArrayList;
import java.util.List;

/**
 * Self-Balancing AVL Tree Implementation in Java.
 * Maintains -1 <= Balance Factor <= 1 through automatic rotations.
 */
public class AVLTree {

    public static class OperationResult {
        public final boolean success;
        public final String message;
        public final List<String> rotations;

        public OperationResult(boolean success, String message, List<String> rotations) {
            this.success = success;
            this.message = message;
            this.rotations = rotations != null ? rotations : new ArrayList<>();
        }
    }

    private AVLNode root;
    private int totalRotations;

    public AVLTree() {
        this.root = null;
        this.totalRotations = 0;
    }

    public AVLNode getRoot() {
        return root;
    }

    private int nodeHeight(AVLNode node) {
        return (node == null) ? 0 : node.height;
    }

    private int balanceFactor(AVLNode node) {
        return (node == null) ? 0 : nodeHeight(node.left) - nodeHeight(node.right);
    }

    private void updateHeight(AVLNode node) {
        if (node != null) {
            node.height = 1 + Math.max(nodeHeight(node.left), nodeHeight(node.right));
        }
    }

    // Right Rotation (LL Imbalance)
    private AVLNode rotateRight(AVLNode y, List<String> rotations) {
        AVLNode x = y.left;
        AVLNode T2 = x.right;

        // Perform rotation
        x.right = y;
        y.left = T2;

        // Update heights
        updateHeight(y);
        updateHeight(x);

        totalRotations++;
        if (rotations != null) {
            rotations.add("Right Rotation on Node #" + y.student.getId());
        }

        return x; // New root
    }

    // Left Rotation (RR Imbalance)
    private AVLNode rotateLeft(AVLNode x, List<String> rotations) {
        AVLNode y = x.right;
        AVLNode T2 = y.left;

        // Perform rotation
        y.left = x;
        x.right = T2;

        // Update heights
        updateHeight(x);
        updateHeight(y);

        totalRotations++;
        if (rotations != null) {
            rotations.add("Left Rotation on Node #" + x.student.getId());
        }

        return y; // New root
    }

    private AVLNode minValueNode(AVLNode node) {
        AVLNode current = node;
        while (current != null && current.left != null) {
            current = current.left;
        }
        return current;
    }

    // --- Insertion ---

    private static class InsertTracker {
        boolean duplicate = false;
        List<String> rotations = new ArrayList<>();
    }

    private AVLNode insertHelper(AVLNode node, Student student, InsertTracker tracker) {
        // 1. Standard BST Insert
        if (node == null) {
            return new AVLNode(student);
        }

        if (student.getId() < node.student.getId()) {
            node.left = insertHelper(node.left, student, tracker);
        } else if (student.getId() > node.student.getId()) {
            node.right = insertHelper(node.right, student, tracker);
        } else {
            tracker.duplicate = true;
            return node;
        }

        // 2. Update Height
        updateHeight(node);

        // 3. Balance Factor Check
        int balance = balanceFactor(node);

        // 4. Rebalance
        // Case 1: LL
        if (balance > 1 && student.getId() < node.left.student.getId()) {
            return rotateRight(node, tracker.rotations);
        }

        // Case 2: RR
        if (balance < -1 && student.getId() > node.right.student.getId()) {
            return rotateLeft(node, tracker.rotations);
        }

        // Case 3: LR
        if (balance > 1 && student.getId() > node.left.student.getId()) {
            node.left = rotateLeft(node.left, tracker.rotations);
            return rotateRight(node, tracker.rotations);
        }

        // Case 4: RL
        if (balance < -1 && student.getId() < node.right.student.getId()) {
            node.right = rotateRight(node.right, tracker.rotations);
            return rotateLeft(node, tracker.rotations);
        }

        return node;
    }

    public OperationResult insert(int id, String name, double marks) {
        return insert(new Student(id, name, marks));
    }

    public OperationResult insert(Student student) {
        InsertTracker tracker = new InsertTracker();
        root = insertHelper(root, student, tracker);

        if (tracker.duplicate) {
            return new OperationResult(false, "Student ID #" + student.getId() + " already exists in AVL Tree.", null);
        }
        return new OperationResult(true, "Student #" + student.getId() + " inserted successfully.", tracker.rotations);
    }

    // --- Deletion ---

    private static class DeleteTracker {
        boolean found = false;
        List<String> rotations = new ArrayList<>();
    }

    private AVLNode deleteHelper(AVLNode node, int id, DeleteTracker tracker) {
        if (node == null) {
            return null;
        }

        if (id < node.student.getId()) {
            node.left = deleteHelper(node.left, id, tracker);
        } else if (id > node.student.getId()) {
            node.right = deleteHelper(node.right, id, tracker);
        } else {
            tracker.found = true;

            // One child or leaf case
            if (node.left == null || node.right == null) {
                AVLNode temp = (node.left != null) ? node.left : node.right;
                if (temp == null) {
                    node = null; // Leaf node
                } else {
                    node = temp; // One child
                }
            } else {
                // Two children case: Replace with inorder successor
                AVLNode successor = minValueNode(node.right);
                node.student = successor.student;
                node.right = deleteHelper(node.right, successor.student.getId(), tracker);
            }
        }

        if (node == null) {
            return null;
        }

        // Update height
        updateHeight(node);

        // Check balance factor
        int balance = balanceFactor(node);

        // Rebalance
        // Case 1: LL
        if (balance > 1 && balanceFactor(node.left) >= 0) {
            return rotateRight(node, tracker.rotations);
        }

        // Case 2: LR
        if (balance > 1 && balanceFactor(node.left) < 0) {
            node.left = rotateLeft(node.left, tracker.rotations);
            return rotateRight(node, tracker.rotations);
        }

        // Case 3: RR
        if (balance < -1 && balanceFactor(node.right) <= 0) {
            return rotateLeft(node, tracker.rotations);
        }

        // Case 4: RL
        if (balance < -1 && balanceFactor(node.right) > 0) {
            node.right = rotateRight(node.right, tracker.rotations);
            return rotateLeft(node, tracker.rotations);
        }

        return node;
    }

    public OperationResult deleteStudent(int id) {
        DeleteTracker tracker = new DeleteTracker();
        root = deleteHelper(root, id, tracker);

        if (!tracker.found) {
            return new OperationResult(false, "Student #" + id + " not found in AVL Tree.", null);
        }
        return new OperationResult(true, "Student #" + id + " deleted successfully.", tracker.rotations);
    }

    // --- Search with Path ---

    public boolean search(int id, Student[] outStudent, List<Integer> outPath) {
        if (outPath != null) outPath.clear();
        AVLNode current = root;

        while (current != null) {
            if (outPath != null) outPath.add(current.student.getId());

            if (id == current.student.getId()) {
                if (outStudent != null && outStudent.length > 0) {
                    outStudent[0] = current.student;
                }
                return true;
            } else if (id < current.student.getId()) {
                current = current.left;
            } else {
                current = current.right;
            }
        }
        return false;
    }

    // --- Traversals ---

    private void inorderHelper(AVLNode node, List<Student> list) {
        if (node != null) {
            inorderHelper(node.left, list);
            list.add(node.student);
            inorderHelper(node.right, list);
        }
    }

    public List<Student> inorder() {
        List<Student> list = new ArrayList<>();
        inorderHelper(root, list);
        return list;
    }

    private void preorderHelper(AVLNode node, List<Student> list) {
        if (node != null) {
            list.add(node.student);
            preorderHelper(node.left, list);
            preorderHelper(node.right, list);
        }
    }

    public List<Student> preorder() {
        List<Student> list = new ArrayList<>();
        preorderHelper(root, list);
        return list;
    }

    private void postorderHelper(AVLNode node, List<Student> list) {
        if (node != null) {
            postorderHelper(node.left, list);
            postorderHelper(node.right, list);
            list.add(node.student);
        }
    }

    public List<Student> postorder() {
        List<Student> list = new ArrayList<>();
        postorderHelper(root, list);
        return list;
    }

    // --- Metrics ---

    public int getHeight() {
        return nodeHeight(root);
    }

    private int countNodes(AVLNode node) {
        if (node == null) return 0;
        return 1 + countNodes(node.left) + countNodes(node.right);
    }

    public int size() {
        return countNodes(root);
    }

    public int getTotalRotations() {
        return totalRotations;
    }

    public boolean isEmpty() {
        return root == null;
    }

    public void clear() {
        root = null;
    }

    // --- Validation ---

    private boolean validateBST(AVLNode node, long min, long max) {
        if (node == null) return true;
        if (node.student.getId() <= min || node.student.getId() >= max) return false;
        return validateBST(node.left, min, node.student.getId()) &&
               validateBST(node.right, node.student.getId(), max);
    }

    private boolean validateHeights(AVLNode node) {
        if (node == null) return true;
        int bf = balanceFactor(node);
        if (bf < -1 || bf > 1) return false;

        int expectedHeight = 1 + Math.max(nodeHeight(node.left), nodeHeight(node.right));
        if (node.height != expectedHeight) return false;

        return validateHeights(node.left) && validateHeights(node.right);
    }

    public boolean validate() {
        return validateBST(root, Long.MIN_VALUE, Long.MAX_VALUE) && validateHeights(root);
    }

    // --- JSON Serialization for Web Frontend Compatibility ---

    private void toJSONHelper(AVLNode node, StringBuilder sb) {
        if (node == null) {
            sb.append("null");
            return;
        }

        sb.append("{")
          .append("\"id\":").append(node.student.getId()).append(",")
          .append("\"name\":\"").append(node.student.getName()).append("\",")
          .append("\"marks\":").append(node.student.getMarks()).append(",")
          .append("\"height\":").append(node.height).append(",")
          .append("\"balance_factor\":").append(balanceFactor(node)).append(",")
          .append("\"left\":");
        toJSONHelper(node.left, sb);
        sb.append(",\"right\":");
        toJSONHelper(node.right, sb);
        sb.append("}");
    }

    public String toJSON() {
        StringBuilder sb = new StringBuilder();
        toJSONHelper(root, sb);
        return sb.toString();
    }

    // --- Pretty ASCII Console Printing ---

    private void printTreeHelper(AVLNode node, String prefix, boolean isLeft) {
        if (node != null) {
            System.out.print(prefix);
            System.out.print(isLeft ? "├── " : "└── ");
            System.out.println("#" + node.student.getId() + " (" + node.student.getName() + ", "
                    + node.student.getMarks() + "%) [h=" + node.height + ", BF=" + balanceFactor(node) + "]");

            printTreeHelper(node.left, prefix + (isLeft ? "│   " : "    "), true);
            printTreeHelper(node.right, prefix + (isLeft ? "│   " : "    "), false);
        }
    }

    public void printTree() {
        if (root == null) {
            System.out.println("(Empty Tree)");
            return;
        }
        printTreeHelper(root, "", false);
    }
}
