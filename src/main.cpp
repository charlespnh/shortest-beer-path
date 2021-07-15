#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#include "sp.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"

void reset(DAG& dag){
	for (auto i : dag){
		i->col_vtx->p = NULL;
		delete i;
	}
}

vector<vector<double> > floyd_warshall(HalfEdge* root_edge, int n){
	vector<vector<double> > dist(n, vector<double>(n, INF));
	struct HalfEdge* traverse_e = root_edge->twin;
	struct Vertex* u = traverse_e->target;

	for (int i = 0; i < n; i++){
		for (int j = 0; j < u->chain->v_chain.size(); j++){
			struct HalfEdge* tmp = u->chain->v_chain[j].first;
			struct Vertex* v = tmp->prev->target;
			dist[u->data + 1][v->data + 1] = tmp->weight;
		}

		traverse_e = traverse_e->prev;
		u = traverse_e->target;
	}

	for (int i = 0; i < n; i++){
		dist[i][i] = 0;
	}

	for (int k = 0; k < n; k++){
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				if (dist[i][j] > dist[i][k] + dist[k][j])
					dist[i][j] = dist[i][k] + dist[k][j];
			}
		}
	}

	return dist;
} 


int main(){
	int V, N = 2000;
	float beer_probability = 0.33;
	struct lca* your_lca = new lca(N);
	clock_t start = clock();
	struct Node* root_node = generate_n_nodes_tree(N);
	cout << "Generating balance binary tree took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";

	your_lca->tree_depth = maxDepth(root_node);
	int lDepth = maxDepth(root_node->left);
	int rDepth = maxDepth(root_node->right);
	cout << "Root: " << root_node->data << "\n";
	cout << "Depth: " << your_lca->tree_depth << "\n";
	cout << "lDepth: " << lDepth << "\n";
	cout << "rDepth: " << rDepth << "\n\n";

	N = inorder_label(root_node, 1) - 1;
	V = N + 2;
	cout << "Number of nodes: " << N << "\nNumber of vertices: " << V << "\n\n";
	cout << "New root after inorder labelling: " << root_node->data << "\n\n";  

	start = clock();
	HalfEdge* root_edge = build_polygon(root_node, V, beer_probability);
	cout << "Triangulation took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	precompute_lca(your_lca, root_node);
	your_lca->preprocessing_duration = (clock() - your_lca->preprocessing_start ) / (double) CLOCKS_PER_SEC;
    cout << "LCA preprossing took: " << your_lca->preprocessing_duration << " s\n";

	start = clock();
	preprocess_graph(root_edge, your_lca);
	cout << "Graph preprocesing took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";
	// edge_traversal(root_edge);

	start = clock();
	vector<vector<double> > apsp = floyd_warshall(root_edge, V);
	cout << "Floyd-Warshall APSP took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n\n";

	struct HalfEdge *tmp1 = root_edge->twin, *tmp2 = tmp1;
	struct Vertex *src, *dest;
	DAG dag;
	int count = 0, beer = 0;
	double time = 0.0;
	for (int i = 0; i < V; i++){
		tmp1 = tmp1->prev;
		src = tmp1->target;
		if (src->beer) beer++;
		for (int j = 0; j < V; j++){
			dest = tmp2->target;
			if (src == dest || check_on_chain(src, dest, your_lca)){
				tmp2 = tmp2->next;
				continue;
			}
			start = clock();
			// double dist = shortest_path(src, dest, your_lca);
			DAG dag = build_dag(src, dest, your_lca);
			double dist = shortest_path_dag(dag);
			time += (clock() - start) / (double) CLOCKS_PER_SEC;
			if (!epsilon_equal(dist, apsp[src->data + 1][dest->data + 1])){
				cout << "dist found: " << dist << " | correct dist: " << apsp[src->data + 1][dest->data + 1] << " - FAILED\n";
				return -1;
			}
			tmp2 = tmp2->next;
			reset(dag);
			count++;
		}
		tmp2 = tmp1;
	}

	cout << "Total number of paths (between non-adjacent vertices) checked: (n+2)P2 - 2(2n+1) = " << count << " - SUCCESS\n";
	cout << "Total time taken to find " << count << " shortest paths: " << time << "\n";
	cout << "Total number of beer stores: " << beer << "\n";
	free_mem(root_node);
	return 0;
}
