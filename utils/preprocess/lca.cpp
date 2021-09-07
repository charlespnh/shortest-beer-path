#include <cassert>
#include <vector>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace std;

#include "../../include/preprocess/lca.h"
#include "../../include/graph/dcel.h"

void eulerTree(struct lca* lca, struct node* root, int h) { 
    // store current node's data 
    lca->height.push_back(h);
    lca->euler_tour.push_back(root);
    lca->first_visit[root->iData] = lca->euler_tour.size()-1;
    
    // If left node exists 
    if (root->left) { 
        // traverse left subtree 
        eulerTree(lca, root->left, h+1); 

        lca->height.push_back(h);

        // store parent node's data  
        lca->euler_tour.push_back(root); 
    } 
  
    // If right node exists 
    if (root->right) { 
        // traverse right subtree 
        eulerTree(lca, root->right, h+1);  
        
        lca->height.push_back(h);
  
        // store parent node's data 
        lca->euler_tour.push_back(root); 
    } 
} 


int min_by_height(struct lca* lca, int i, int j) {

    return lca->height[i] <= lca->height[j] ? i : j;
}

void build_sparse_table(struct lca* lca, int euler_size){
    
    // precompute minimum of each block and build sparse table
    lca->st.assign(lca->array_log[lca->block_cnt] + 1, vector<int>(lca->block_cnt));
    // first row of the sparse table, divide by the block size
    for (int i = 0, j = 0, b = 0; i < euler_size; i++, j++) {
        if (j == lca->block_size)
            j = 0, b++;
        if (j == 0 || min_by_height(lca, i, lca->st[0][b]) == i)
            lca->st[0][b] = i;
    }
    
    for (int l = 1; l <= lca->array_log[lca->block_cnt]; l++) {
        for (int i = 0; i < lca->block_cnt; i++) {
            // ni = i + 2^(l-1), on level 1 we compare 1+1 values, on level 2 we compare 1+2 values, on level 3 we compare 1+4 values ... 
            int ni = i + (1 << (l - 1));
            // if range exceed the num length
            if (ni >= lca->block_cnt)
                lca->st[l][i] = lca->st[l-1][i];
            else
                // choose the min value among the range
                lca->st[l][i] = min_by_height(lca, lca->st[l-1][i], lca->st[l-1][ni]);
        }
    }
}

void compute_all_block_binary_nums(struct lca* lca, int euler_size){
    // precompute unique num from bit string for each block
    lca->block_bit.assign(lca->block_cnt, 0);
    for (int i = 0, j = 0, b = 0; i < euler_size; i++, j++) {
        if (j == lca->block_size)
            j = 0, b++;
        // if current normalized bit is 1, which means it is larger than it's previous, then add 2^(j-1)
        if (j > 0 && (min_by_height(lca, i - 1, i) == i - 1))
            lca->block_bit[b] += 1 << (j - 1);
    }
}

// precompute RMQ for each unique block
void compute_all_combinations(struct lca* lca, int euler_size){
    // each num can be normalized to 0 or 1, so 2^(block size - 1)
    int num_possible_blocks = 1 << (lca->block_size - 1);
    lca->blocks.resize(num_possible_blocks);
    // for each block
    for (int b = 0; b < lca->block_cnt; b++) {
        // get bit string for this block
        int bit_num = lca->block_bit[b];
        // if this is a new block (which hasn't been computed before), do the computation
        // otherwise, go to the next block and check
        if (!lca->blocks[bit_num].empty())
            continue;
        // compute for all n choose 2 combinations
        lca->blocks[bit_num].assign(lca->block_size, vector<int>(lca->block_size));
        // left index
        for (int l = 0; l < lca->block_size; l++) {
            lca->blocks[bit_num][l][l] = l;
            // right index, increase by 1 each time to cover all combinations
            for (int r = l + 1; r < lca->block_size; r++) {
                lca->blocks[bit_num][l][r] = lca->blocks[bit_num][l][r - 1];
                if (b * lca->block_size + r < euler_size)
                    // check if current min is smaller or not
                    // "- b * block_size" to match the index inside the block
                    lca->blocks[bit_num][l][r] = min_by_height(lca, b * lca->block_size + lca->blocks[bit_num][l][r], 
                            b * lca->block_size + r) - b * lca->block_size;
            }
        }
    }
}

void build_log_array(struct lca* lca){
    // precompute all log values without using floor and logarithm functions
    int m = lca->euler_tour.size();
    lca->array_log.reserve(m + 1);
    // for index 0
    lca->array_log.push_back(-1);
    for (int i = 1; i <= m; i++) {
        lca->array_log.push_back(lca->array_log[i / 2] + 1);
    }
}

void precompute_lca(struct lca* lca, struct node* root) {
    // Stores Euler Tour 
    lca->first_visit.assign(lca->N, -1);
    lca->height.reserve(2 * lca->N);
    lca->euler_tour.reserve(2 * lca->N);

    eulerTree(lca, root, 0); 
    int m = lca->euler_tour.size();

    build_log_array(lca);

    // calculate block size and total number of blocks via log array
    lca->block_size = max(1, lca->array_log[m] / 2);
    lca->block_cnt = (m + lca->block_size - 1) / lca->block_size;

    // build up sparse table for 2n/logn (# of blocks) total numbers 
    build_sparse_table(lca, m);
    compute_all_block_binary_nums(lca, m);
    compute_all_combinations(lca, m);
}


int get_in_block_min(struct lca* lca, int b, int l, int r) {
    // block_bit[b]: unique bit string, eg: 110 -> 6
    // if it is 2nd block, block size is 4, the min in this block from l to r is index 3 (in this block)
    // then the return index should be 3 + 2 * 4
    return lca->blocks[lca->block_bit[b]][l][r] + b * lca->block_size;
}

struct node* get_lca(struct lca* lca, struct node* u, struct node* v){
    return get_lca(lca, u->iData, v->iData);
}

struct node* get_lca(struct lca* lca, int u, int v) {
    int l = lca->first_visit[u];
    int r = lca->first_visit[v];
    if (l > r) swap(l, r);

    int block_l = l / lca->block_size;
    int block_r = r / lca->block_size;

    // if u and v are in the same block
    if (block_l == block_r)
        // min in the block which contains both u and v, starting from u to v (or v to u, whichever is smaller)
        return lca->euler_tour[get_in_block_min(lca, block_l, l % lca->block_size, r % lca->block_size)];
    
    // index of the min in the block which contains u, starting from u to the end of this block
    int min1_in_block = get_in_block_min(lca, block_l, l % lca->block_size, lca->block_size - 1);
    // index of the min in the block which contains v, starting from the beginning of this block to v
    int min2_in_block = get_in_block_min(lca, block_r, 0, r % lca->block_size);
    // choose the smaller one
    int ans = min_by_height(lca, min1_in_block, min2_in_block);

    // if there are gaps (blocks) between block_l and block_r (blocks which contain u and v)
    if (block_l + 1 < block_r) {
        // compute log of (number of blocks between block_l and block_r), exclusively
        // to compute which row we should use to look up in sparse table
        int b = lca->array_log[block_r - block_l - 1];

        // look up in the sparse table and get the min among the blocks starting from block_l + 1
        // to 2^(b-1) more blocks
        int min1_blocks = lca->st[b][block_l+1];

        // look up in the sparse table and get the min among the blocks starting from block_r - 2^b
        // to 2^(b-1) more blocks
        int min2_blocks = lca->st[b][block_r - (1 << b)];
       
        ans = min_by_height(lca, ans, min_by_height(lca, min1_blocks, min2_blocks));

    }
    return lca->euler_tour[ans];
}
 
