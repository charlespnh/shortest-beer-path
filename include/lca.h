#ifndef LCA_H    
#define LCA_H
#include <cassert>
#include <vector>
#include <ctime>
#include <cstdio>

using namespace std;

#include "btree.h"

struct lca {
    vector<int> height;
    vector<int> first_visit; 
    int N, block_size, block_cnt;
    vector<struct Node*> euler_tour;
    vector<int> array_log;
    vector<vector<int> > st;
    vector<int> array_B;
    vector<int> array_C;
    vector<vector<vector<int> > > blocks;
    vector<int> block_bit;
    clock_t preprocessing_start;
    double preprocessing_duration;
    clock_t getting_start;
    double getting_duration;
    int tree_depth;

    lca(int n){
		vector<int> height;
        vector<int> first_visit; 
        N = n;
        int block_size, block_cnt;
        vector<struct Node*> euler_tour;
        vector<int> array_log;
        vector<vector<int> > st;
        vector<int> array_B;
        vector<int> array_C;
        vector<vector<vector<int> > > blocks;
        vector<int> block_bit;
        clock_t getting_start;
        double getting_duration;
        int tree_depth;
	}

    lca(){
        vector<int> height;
        vector<int> first_visit; 
        N = 1000000;
        int block_size, block_cnt;
        vector<struct Node*> euler_tour;
        vector<int> array_log;
        vector<vector<int> > st;
        vector<int> array_B;
        vector<int> array_C;
        vector<vector<vector<int> > > blocks;
        vector<int> block_bit;
        clock_t getting_start;
        double getting_duration;
        int tree_depth;
    }
};

void eulerTree(struct lca* lca, struct Node* root, int h);
int min_by_height(struct lca* lca, int i, int j);
void build_sparse_table(struct lca* lca, int euler_size);
void compute_all_block_binary_nums(struct lca* lca, int euler_size);
void compute_all_combinations(struct lca* lca, int euler_size);
void build_log_array(struct lca* lca);
void precompute_lca(struct lca* lca, struct Node* root);
int get_in_block_min(struct lca* lca, int b, int l, int r);
struct Node* get_lca(struct lca* lca, struct Node* u, struct Node* v);
struct Node* get_lca(struct lca* lca, int u, int v);

void test_1(struct lca* lca, int k);
void test_2(struct lca* lca, int k);
void test_right_skewed(struct lca* lca, int k);
void test_balanced_btree(struct lca* lca, int k);


#endif
