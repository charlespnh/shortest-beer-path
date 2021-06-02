#ifndef BTREE_H    
#define BTREE_H
#include <random>
#include <vector>

using namespace std;

struct Node {
    int data;
    struct HalfEdge* incident_edge;
    struct Node* left;
    struct Node* right;
 
    // val is the key or the value that
    // has to be added to the data part
    Node(int val){
        data = val;
        incident_edge = NULL;
        // Left and right child for node
        // will be initialized to null
        left = NULL;
        right = NULL;
    }
    
	Node(){
		
	}
};

double random(int range_to);
struct Node* generate(int N);
struct Node* generate_skewed_right(int N);
struct Node* insertRight(vector<int>& arr, Node* root, int i, int n);
struct Node* generate_n_nodes_tree(int N);
struct Node* insertLevelOrder(vector<int>& arr, Node* root, int i, int n);
int maxDepth(Node* node);
void free_mem(Node* node);
// void insert(Node *root, int data);
void traversal(Node *root);
void inorder_traversal(struct Node *root);
int inorder_label(Node *root, int val);

#endif
