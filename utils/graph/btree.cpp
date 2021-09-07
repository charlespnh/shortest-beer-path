#include <vector>
#include <deque>
#include <random>
#include <iostream>

using namespace std;

#include "../../include/graph/btree.h"
#include "../../include/graph/dcel.h"
#include "../../include/utils.h"

// slide
struct node* btree::generate(int N) {
    vector<int> links(2*N + 1);
    for (int k = 1; k < 2*N; k=k+2){
        int x = random_uniform(k);
        if (random_bernoulli(0.5)){ 
            links[k] = k+1; 
            links[k+1] = links[x]; 
        } else { 
            links[k] = links[x]; 
            links[k+1] = k+1; 
        }
        links[x] = k;
    }

    vector<struct node*> nodes(2*N + 1);
    for (int k = 0; k < 2*N + 1; k=k+2)
        nodes[k] = new node(links[k]);
    for (int k = 1; k < 2*N + 1; k=k+2) 
        nodes[k] = new node(links[k]);

    struct node* root = nodes[links[0]];
    for (int k = 1; k < 2*N; k=k+2) {
        nodes[k]->left = nodes[links[k]];
        nodes[k]->right = nodes[links[k+1]];
    }

    return root;
}


struct node* btree::generate_skewed_right(int N) {
    vector<int> links(N+1);
    for (int k = 0; k < N; k++){
        links[k] = k;
    }

    struct node* root = new node(0);
    root->right = btree::insertRight(links, root->right, 1, N);

    return root;
}


struct node* btree::insertRight(vector<int>& arr, struct node* root, int i, int n) { 
    // Base case for recursion 
    if (i < n) { 
        root = new node(arr[i]);

        // insert right child 
        root->right = btree::insertRight(arr, 
                  root->right, i + 1, n); 
    }

    return root; 
} 


struct node* btree::generate_balance_tree(int N){
    vector<int> links(N+1);
    for (int k = 0; k < N; k++){
        links[k] = k;
    }

    // int m = random_uniform(N);
    // struct node* root = new Node(m);
    // root->left = insertLevelOrder(links, root->left, 0, m);
    // root->right = insertLevelOrder(links, root->right, m + 1, N);
    struct node* root = btree::insertLevelOrder(links, root, 0, N);

    return root;
}


// Function to insert nodes in level order 
struct node* btree::insertLevelOrder(vector<int>& arr, struct node* root, int i, int n) { 
    // Base case for recursion 
    if (i < n) { 
        root = new node(arr[i]);
        // insert left child 
        root->left = btree::insertLevelOrder(arr, 
                   root->left, 2 * i + 1, n); 

        // insert right child 
        root->right = btree::insertLevelOrder(arr, 
                  root->right, 2 * i + 2, n); 
    }

    return root; 
} 


struct node* btree::build_cartesian_tree(vector<double>& arr){
    int n = arr.size();
    deque<struct node*> s;
    for (int i = 0; i < n; i++){
        struct node* top = NULL;
        struct node* nnode = new node(i, arr[i]);
        while (!s.empty() && s.back()->dData >= arr[i]){
            top = s.back();
            s.pop_back();
        }
        if (top) nnode->left = top; 
        if (!s.empty()) s.back()->right = nnode;

        s.push_back(nnode);
    }

    return s.front();
}


/* Function for inorder labeling of every node in the tree. */
int btree::inorder_label(struct node *root, int val) {
    if (root == NULL) return val;
	
    val = inorder_label(root->left, val);
    root->iData = val;
    val = inorder_label(root->right, val + 1);

    return val;
}


int btree::max_depth(struct node* node) { 
    if (node == NULL) 
        return -1; 
    else{ 
        // compute the depth of each subtree
        int lDepth = btree::max_depth(node->left); 
        int rDepth = btree::max_depth(node->right);
     
        // use the larger one
        if (lDepth > rDepth) 
            return(lDepth + 1); 
        else return(rDepth + 1); 
    } 
}


void btree::free_mem(struct node* root){
    if (root == NULL) return;

    btree::free_mem(root->left);
    btree::free_mem(root->right);

    delete root;
    root = NULL;
}

