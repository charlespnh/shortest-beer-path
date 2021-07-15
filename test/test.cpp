#include <iostream>
#include <ctime>

using namespace std;

#include "../include/btree.h"
#include "../include/lca.h"

int main() {
    cout << endl << "======================================================================================" << endl;
    cout << "*** select two nodes in left subtree and right subtree of node root->left, should return root->left ***" << endl << endl;
    struct lca* lca_1 = new lca();
    cout << "Testing on 1000 nodes" << endl;
    test_1(lca_1, 500);
    delete lca_1;
    struct lca* lca_2 = new lca();
    cout << "Testing on 10000 nodes" << endl;
    test_1(lca_2, 5000);
    delete lca_2;
    struct lca* lca_3 = new lca();
    cout << "Testing on 100000 nodes" << endl;
    test_1(lca_3, 50000);
    delete lca_3;

    cout << "=======================================================================================" << endl;
    cout << "*** select one node in the subtree of root->left, should return root->left ***" << endl << endl;
    struct lca* lca_4 = new lca();
    cout << "Testing on 1000 nodes" << endl;
    test_2(lca_4, 1000);
    delete lca_4;
    struct lca* lca_5 = new lca();
    cout << "Testing on 10000 nodes" << endl;
    test_2(lca_5, 10000);
    delete lca_5;
    struct lca* lca_6 = new lca();
    cout << "Testing on 100000 nodes" << endl;
    test_2(lca_6, 100000);
    delete lca_6;

    cout << "=======================================================================================" << endl;
    cout << "*** Right skewed tree: select one node in the subtree of root->right, should return root->right ***" << endl << endl;
    struct lca* lca_7 = new lca(1000);
    cout << "Testing on 1000 nodes" << endl;
    test_right_skewed(lca_7, 1000);
    delete lca_7;
    struct lca* lca_8 = new lca(10000);
    cout << "Testing on 10000 nodes" << endl;
    test_right_skewed(lca_8, 10000);
    delete lca_8;
    struct lca* lca_9 = new lca(100000);
    cout << "Testing on 100000 nodes" << endl;
    test_right_skewed(lca_9, 100000);

    cout << "=======================================================================================" << endl;
    cout << "*** 'Balanced binary tree': select two nodes in left subtree and right subtree of node root->left, should return root->left ***" << endl << endl;
    struct lca* lca_10 = new lca(1000);
    cout << "Testing on 1000 nodes" << endl;
    test_balanced_btree(lca_10, 1000);
    delete lca_10;
    struct lca* lca_11 = new lca(10000);
    cout << "Testing on 10000 nodes" << endl;
    test_balanced_btree(lca_11, 10000);
    delete lca_11;
    struct lca* lca_12 = new lca(100000);
    cout << "Testing on 100000 nodes" << endl;
    test_balanced_btree(lca_12, 100000);
    delete lca_12;
}