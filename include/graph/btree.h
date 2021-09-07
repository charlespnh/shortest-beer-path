#ifndef BTREE_H    
#define BTREE_H
#include <vector>
#include <ctime>

using namespace std;

// forward declaration
struct node;


class btree {
    private:
        static struct node* insertRight(vector<int>& arr, struct node* root, int i, int n);
        static struct node* insertLevelOrder(vector<int>& arr, struct node* root, int i, int n);
        static struct node* insertLevelOrderRandom(vector<int>& arr, struct node* root, int i, int n);

    public:
        static struct node* generate(int N);
        static struct node* generate_skewed_right(int N);
        static struct node* generate_balance_tree(int N);
        static struct node* generate_balance_tree_random(int N);
        static struct node* build_cartesian_tree(vector<double>& arr);

        static int inorder_label(struct node *root, int val);
        static int max_depth(struct node* node);
        static void free_mem(struct node* node);
};

#endif
