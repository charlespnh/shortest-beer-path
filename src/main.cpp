#include <ctime>
#include <iostream>

using namespace std;

#include "sp_dag.h"
#include "../include/dcel.h"
#include "../include/lca.h"
#include "../include/btree.h"

void reset(HalfEdge* root_edge){
	HalfEdge* tmp = root_edge->twin;
	do {
		tmp->target->p = NULL;
		tmp = tmp->next;
	} while (tmp != root_edge->twin);
}


int main(){
	int n = 18;
	struct lca* your_lca = new lca();
	clock_t start = clock();
	Node* root_node = generate_n_nodes_tree(n);
	cout << "Generating balance binary tree took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";

	your_lca->tree_depth = maxDepth(root_node);
	int lDepth = maxDepth(root_node->left);
	int rDepth = maxDepth(root_node->right);
	cout << "Root: " << root_node->data << "\n";
	cout << "Depth: " << your_lca->tree_depth << "\n";
	cout << "lDepth: " << lDepth << "\n";
	cout << "rDepth: " << rDepth << "\n\n";

	n = inorder_label(root_node, 1) - 1;
	cout << "Number of nodes: " << n << "\nNumber of vertices: " << n + 2 << "\n\n";
	cout << "New root after inorder labelling: " << root_node->data << "\n\n";  

	start = clock();
	HalfEdge* root_edge = build_polygon(root_node, n + 2);
	cout << "Triangulation took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n";

	precompute_lca(your_lca, root_node);
	your_lca->preprocessing_duration = (clock() - your_lca->preprocessing_start ) / (double) CLOCKS_PER_SEC;
    cout << "LCA preprossing took: " << your_lca->preprocessing_duration << " s\n";

	start = clock();
	preprocess_graph(root_edge, your_lca);
	cout << "Graph preprocesing took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";

	HalfEdge *tmp1, *tmp2;
	tmp1 = tmp2 = root_edge->twin;
	Vertex *src, *dest;
	DAG dag;
	int count = 0;
	double time_dag = 0, time_sp = 0, dist = 0;
	for (int i = 0; i < n + 2; i++){
		tmp1 = tmp1->prev;
		src = tmp1->target;
		for (int j = 0; j < n + 2; j++){
			dest = tmp2->target;
			if (src == dest || check_on_chain(src, dest, your_lca)){
				tmp2 = tmp2->next;
				continue;
			}
			cout << "Src " << src->data << " and sink " << dest->data << "\n";
			start = clock();
			dag = build_dag(src, dest, your_lca);
			time_dag += (clock() - start) / (double) CLOCKS_PER_SEC;;
			start = clock();
			dist = shortest_path_dag(dag);
			time_sp += (clock() - start) / (double) CLOCKS_PER_SEC;
			print_sp_dag(dag.back()->col_vtx);
			cout << " | dist: " << dist << "\n\n";
			tmp2 = tmp2->next;
			reset(root_edge);
			count++;
		}
		tmp2 = tmp1;
	}
	cout << "Total number of paths (between non-adjacent vertices) checked: (n+2)P2 - 2(2n+1) = " << count << "\n";
	cout << "Total time taken to build " << count <<  " DAGs: " << time_dag << " and find " << count << " shortest paths: " << time_sp << "\n";
	free_mem(root_node);
	return 0;
}
