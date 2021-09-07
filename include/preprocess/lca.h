#ifndef LCA_H    
#define LCA_H
#include <cassert>
#include <vector>
#include <ctime>
#include <cstdio>

using namespace std;

// forward declaration
struct node;


struct lca {
    vector<int> height;
    vector<int> first_visit; 
    int N, block_size, block_cnt;
    vector<struct node*> euler_tour;
    vector<int> array_log;
    vector<vector<int> > st;
    vector<vector<vector<int> > > blocks;
    vector<int> block_bit;

    lca(int n){
		vector<int> height;
        vector<int> first_visit; 
        N = n;
        int block_size, block_cnt;
        vector<struct node*> euler_tour;
        vector<int> array_log;
        vector<vector<int> > st;
        vector<vector<vector<int> > > blocks;
        vector<int> block_bit;
	}

    lca(){
        vector<int> height;
        vector<int> first_visit; 
        N = 1000000;
        int block_size, block_cnt;
        vector<struct node*> euler_tour;
        vector<int> array_log;
        vector<vector<int> > st;
        vector<vector<vector<int> > > blocks;
        vector<int> block_bit;
    }
};

void eulerTree(struct lca* lca, struct node* root, int h);
int min_by_height(struct lca* lca, int i, int j);
void build_sparse_table(struct lca* lca, int euler_size);
void compute_all_block_binary_nums(struct lca* lca, int euler_size);
void compute_all_combinations(struct lca* lca, int euler_size);
void build_log_array(struct lca* lca);
int get_in_block_min(struct lca* lca, int b, int l, int r);

void precompute_lca(struct lca* lca, struct node* root);
struct node* get_lca(struct lca* lca, struct node* u, struct node* v);
struct node* get_lca(struct lca* lca, int u, int v);

#endif
