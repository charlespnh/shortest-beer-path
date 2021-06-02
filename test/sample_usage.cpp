#include <iostream>

using namespace std;

#include "../include/btree.h"
#include "../include/lca.h"

int k = 1000;
int main() {
    struct lca* lca_1 = new lca();
    struct Node* root = generate_n_nodes_tree(k);
    
    // command of preprocessing
    precompute_lca(lca_1, root);

    struct Node* node_u = root->left;
    struct Node* node_v = root->left;

    // choose sample node_u node_v
    for(int i = 0; i < 10; i++){
        if (node_u->left)
            node_u = node_u->left;
        else
            break;

        if (node_u->right)
            node_u = node_u->right;
        else
            break;
    }

    for(int i = 0; i < 10; i++){
        if (node_v->right)
            node_v = node_v->right;
        else
            break;

        if (node_v->left)
            node_v = node_v->left;
        else
            break;
    }

    cout << "Node_u: " << node_u->data << endl;
    cout << "Node_v: " << node_v->data << endl;

    // command of getting the query
    Node* result = get_lca(lca_1, node_u->data, node_v->data);

    cout << "Result should be: " << root->left->data << " Actual result is: "<< result->data << endl;
}

