package com.treevault;

/**
 * Represents a Node in the AVL Tree containing a Student record.
 */
public class AVLNode {
    public Student student;
    public int height;
    public AVLNode left;
    public AVLNode right;

    public AVLNode(Student student) {
        this.student = student;
        this.height = 1;
        this.left = null;
        this.right = null;
    }

    public int getBalance() {
        int leftH = (left != null) ? left.height : 0;
        int rightH = (right != null) ? right.height : 0;
        return leftH - rightH;
    }
}
