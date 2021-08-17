#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#include "dag.h"
#include "sssp.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"

void reset(DAG& dag){
	for (auto i : dag){
		i->col_vtx->p = NULL;
		for (auto e : i->spes){
			delete e.first;
			delete e.second;
		}
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

void test_1(struct HalfEdge* root_edge, struct lca* your_lca, int V){
	cout << "------------------------TEST I------------------------\n";
	clock_t start = clock();
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
			double distB = shortest_beer_path_dag(dag);
			time += (clock() - start) / (double) CLOCKS_PER_SEC;
			/* if (!epsilon_equal(dist, apsp[src->data + 1][dest->data + 1])){
				cout << "dist found: " << dist << " | correct dist: " << apsp[src->data + 1][dest->data + 1] << " - FAILED\n";
				return -1;
			} */
			tmp2 = tmp2->next;
			reset(dag);
			count++;
		}
		tmp2 = tmp1;
	}

	cout << "Total number of paths (between non-adjacent vertices) checked: (n+2)P2 - 2(2n+1) = " << count << " - SUCCESS\n";
	cout << "Total time taken to find " << count << " shortest paths: " << time << "\n";
	cout << "Total number of beer stores: " << beer << "\n";
}

struct Vertex* get_vertex(struct HalfEdge* root, int v, struct lca* your_lca){
	struct Vertex* vertex;
	if (v == -1) vertex = root->twin->target;
	else if (v == 0) vertex = root->target;
	else vertex = get_lca(your_lca, v, v)->incident_edge->next->target;

	return vertex;
}

void test_2(struct HalfEdge* root, int s, int d, struct lca* your_lca){
	struct Vertex* src = get_vertex(root, s, your_lca);
	struct Vertex* dest = get_vertex(root, d, your_lca);;

	cout << "------------------------TEST II------------------------\n";
	clock_t start = clock();
	DAG dag = build_dag(src, dest, your_lca);
	double dist = shortest_path_dag(dag);
	double distB = shortest_beer_path_dag(dag);
	cout << "Single-pair shortest path and beer path took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	reset(dag);
}

void test_3(struct HalfEdge* root, int s, struct lca* your_lca){
	cout << "------------------------TEST III------------------------\n";
	struct Vertex* src = get_vertex(root, s, your_lca);
	clock_t start = clock();
	sssp(get_inEdge(src));
	cout << "Single-source shortest path took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	struct HalfEdge* traverse = root->twin;
	do {
		printf("\u03B4(%d, %d): ", s, traverse->target->data); 
		vector<struct Vertex*> sssp = print_sssp(traverse->target);
		for (auto v : sssp) cout << v->data << " ";		cout << "\n\n";

		traverse = traverse->next;
	} while(traverse != root->twin);
}

void test_4(struct HalfEdge* root, int s, struct lca* your_lca){
	cout << "------------------------TEST IV------------------------\n";
	struct Vertex* src = get_vertex(root, s, your_lca);
	clock_t start = clock();
	sssbp(get_inEdge(src));
	cout << "Single-source shortest beer path took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	struct HalfEdge* traverse = root->twin;
	do {
		printf("\u03B4(%d, %d): ", s, traverse->target->data);
		vector<struct Vertex*> sssbp = print_sssbp(traverse->target);
		for (auto v : sssbp) cout << v->data << " ";	cout << "\n\n";

		traverse = traverse->next;
	} while(traverse != root->twin);
}


int main(){
	int V, N = 18;
	float beer_probability = 0.33;
	clock_t start = clock();
	struct Node* root_node = generate_n_nodes_tree(N);
	cout << "Generating balance binary tree took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";

	int lDepth = maxDepth(root_node->left);
	int rDepth = maxDepth(root_node->right);
	cout << "Depth: " << maxDepth(root_node) << "\n";
	cout << "lDepth: " << lDepth << "\n";
	cout << "rDepth: " << rDepth << "\n\n";

	N = inorder_label(root_node, 1) - 1;
	V = N + 2;
	cout << "Number of nodes: " << N << "\nNumber of vertices: " << V << "\n\n";
	cout << "New root after inorder labelling: " << root_node->iData << "\n\n";  

	start = clock();
	struct HalfEdge* root_edge = build_polygon(root_node, V, beer_probability);
	cout << "Triangulation took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	struct lca* your_lca = new lca(N);
	start = clock();
	precompute_lca(your_lca, root_node);
    cout << "LCA preprossing took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n";

	start = clock();
	preprocess_graph(root_edge, your_lca);
	cout << "Graph preprocesing took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n\n";
	// edge_traversal(root_edge);

	// test_1(root_edge, your_lca, V);
	test_2(root_edge, 0, 14, your_lca);
	// test_3(root_edge, 1, your_lca);
	// test_4(root_edge, 1, your_lca);

	free_mem(root_node);
	return 0;
}
