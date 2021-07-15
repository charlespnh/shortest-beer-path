#include <vector>
#include <deque>
#include <random>
#include <iostream>

using namespace std;

#include "../include/btree.h"
#include "../include/utils.h"

// slide
struct Node* generate(int N) {
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

    vector<Node*> nodes(2*N + 1);
    for (int k = 0; k < 2*N + 1; k=k+2) 
        nodes[k] = new Node(links[k]);
    for (int k = 1; k < 2*N + 1; k=k+2) 
        nodes[k] = new Node(links[k]);
    struct Node* root = nodes[links[0]];
    for (int k = 1; k < 2*N; k=k+2) {
        nodes[k]->left = nodes[links[k]];
        nodes[k]->right = nodes[links[k+1]];
    }

    return root;
}

struct Node* generate_skewed_right(int N) {
    vector<int> links(N+1);
    for (int k = 0; k < N; k++){
        links[k] = k;
    }

    struct Node* root = new Node(0);
    root->right = insertRight(links, root->right, 1, N - 1);
    return root;
}

struct Node* insertRight(vector<int>& arr, Node* root, int i, int n) { 
    // Base case for recursion 
    if (i < n) { 
        root = new Node(arr[i]);

        // insert right child 
        root->right = insertRight(arr, 
                  root->right, i + 1, n); 
    }

    return root; 
} 

struct Node* generate_n_nodes_tree(int N){
    vector<int> links(N+1);
    for (int k = 0; k < N; k++){
        links[k] = k;
    }

    // int m = random_uniform(N);
    // struct Node* root = new Node(m);
    // root->left = insertLevelOrder(links, root->left, 0, m);
    // root->right = insertLevelOrder(links, root->right, m + 1, N);
    struct Node* root = insertLevelOrder(links, root, 0, N);

    return root;
}

// Function to insert nodes in level order 
struct Node* insertLevelOrder(vector<int>& arr, Node* root, int i, int n) { 
    // Base case for recursion 
    if (i < n) { 
        root = new Node(arr[i]);
        // insert left child 
        root->left = insertLevelOrder(arr, 
                   root->left, 2 * i + 1, n); 

        // insert right child 
        root->right = insertLevelOrder(arr, 
                  root->right, 2 * i + 2, n); 
    }

    return root; 
} 

struct Node* build_cartesian_tree(vector<double>& arr){
    int n = arr.size();
    struct Node *top, *node; 
    deque<struct Node*> s;
    for (int i = 0; i < n; i++){
        top = NULL;
        node = new Node(i, arr[i]);
        while (!s.empty() && s.back()->dd >= arr[i]){
            top = s.back();
            s.pop_back();
        }
        if (top) node->left = top; 
        if (!s.empty()) s.back()->right = node;

        s.push_back(node);
    }

    return s.front();
}

int maxDepth(Node* node) { 
    if (node == NULL) 
        return -1; 
    else{ 
        // compute the depth of each subtree
        int lDepth = maxDepth(node->left); 
        int rDepth = maxDepth(node->right);
     
        // use the larger one
        if (lDepth > rDepth) 
            return(lDepth + 1); 
        else return(rDepth + 1); 
    } 
}

void free_mem(struct Node* root){
    if (root == NULL) return;

    free_mem(root->left);
    free_mem(root->right);

    delete root;
    root = NULL;
}



/* Function for tree traversal of every node in the tree. */
void preorder_traversal(struct Node *root){
    if (root == NULL) return;

    //if (root->data >= 0)
    cout << "Node " << root->data << ": " << root->dd << "\n";

    preorder_traversal(root->left);
    preorder_traversal(root->right);
}


void inorder_traversal(struct Node *root){
    if (root == NULL) return;

    inorder_traversal(root->left);
    if (root->data >= 0)
        cout << "Node " << root->data << ": " << root->dd << "\n";
    inorder_traversal(root->right);
}


/* Function for inorder labeling of every node in the tree. */
int inorder_label(struct Node *root, int val) {
    if (root == NULL) return val;
	
    val = inorder_label(root->left, val);
    root->data = val;
    val = inorder_label(root->right, val + 1);

    return val;
}
