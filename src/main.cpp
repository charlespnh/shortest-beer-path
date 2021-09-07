#include <vector>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../test/test_dag.h"
#include "../test/test_sssp.h"



// vector<vector<double> > floyd_warshall(graph& G, int n){
// 	vector<vector<double> > dist(n, vector<double>(n, INF));
// 	struct halfedge* traverse_e = G.mroot_edge->twin;
// 	struct vertex* u = traverse_e->target;

// 	for (int i = 0; i < n; i++){
// 		struct halfedge* degree_e = dcel::twin(traverse_e);
// 		for (int j = 0; j < u->v_chain.size(); j++){
// 			struct vertex* v = dcel::target(degree_e);
// 			dist[u->data + 1][v->data + 1] = dcel::weight(graph::get_edge(u, v));
// 			degree_e = dcel::twin(dcel::prev(degree_e));
// 		}

// 		traverse_e = traverse_e->prev;
// 		u = traverse_e->target;
// 	}

// 	for (int i = 0; i < n; i++){
// 		dist[i][i] = 0;
// 	}

// 	for (int k = 0; k < n; k++){
// 		for (int i = 0; i < n; i++){
// 			for (int j = 0; j < n; j++){
// 				if (dist[i][j] > dist[i][k] + dist[k][j])
// 					dist[i][j] = dist[i][k] + dist[k][j];
// 			}
// 		}
// 	}

// 	return dist;
// } 

// void test_1(graph& G, int V){
// 	cout << "------------------------TEST I------------------------\n";
// 	clock_t start = clock();
// 	vector<vector<double> > apsp = floyd_warshall(G, V);
// 	cout << "Floyd-Warshall APSP took: " << (clock() - start) / (double) CLOCKS_PER_SEC << " s\n\n";

// 	struct halfedge *tmp1 = G.mroot_edge->twin, *tmp2 = tmp1;
// 	struct vertex *src, *dest;
// 	int count = 0, beer = 0;
// 	double time = 0.0;

// 	for (int i = 0; i < V; i++){
// 		tmp1 = tmp1->prev;
// 		src = tmp1->target;
// 		if (src->is_beer) beer++;
// 		for (int j = 0; j < V; j++){
// 			dest = tmp2->target;
// 			if (src == dest || graph::get_edge(src, dest)){
// 				tmp2 = tmp2->next;
// 				continue;
// 			}

// 			dag D;
// 			start = clock();
// 			D.build_dag(G, src, dest);
// 			double dist = D.shortest_path_dag();
// 			double distB = D.shortest_beer_path_dag();
// 			time += (clock() - start) / (double) CLOCKS_PER_SEC;
// 			reset(D);
// 			if (!epsilon_equal(dist, apsp[src->data + 1][dest->data + 1])){
// 				cout << "dist found: " << dist << " | correct dist: " << apsp[src->data + 1][dest->data + 1] << " - FAILED\n";
// 				return;
// 			} 
// 			tmp2 = tmp2->next;
// 			count++;
// 		}
// 		tmp2 = tmp1;
// 	}

// 	cout << "Total number of paths (between non-adjacent vertices) checked: (n+2)P2 - 2(2n+1) = " << count << " - SUCCESS\n";
// 	cout << "Total time taken to find " << count << " shortest paths: " << time << "\n";
// 	cout << "Total number of beer stores: " << beer << "\n\n";
// }



int main(){
	int V;
	float beer_prob;
	cout << "Enter total number of vertices: ";
	cin >> V;
	cout << "Enter probability of having a beer store: ";
	cin >> beer_prob;

	graph G(V, beer_prob);

	cout << "\n\t--------------------Main Manual--------------------\n";
	cout << "\t[1]	Build dual D(G)\n";
		cout << "\t\t[1]	Generate balance dual D(G)\n";
		cout << "\t\t[2]	Generate skew-right dual D(G)\n";
		cout << "\t\t[3]	Generate random dual D(G)\n";
	cout << "\t[2]	Build graph G\n";
	cout << "\t[3]	Preprocess graph G\n";
	cout << "\t[4]	Test Single-Pair Shortest (Beer) Path\n";
	cout << "\t[5]	Test Single-Source Shortest (Beer) Path\n\n";


	while(1){
		int dual_type;

		int option;
		cout << "Choose a manual option: ";
		cin >> option;	cout << "\n";
		switch(option){
			case 1:
				cout << "Select dual type of graph G: ";
				cin >> dual_type;

				switch(dual_type){
					case 1:
						cout << "Building D(G) as balance binary tree...\n";
						G.build_dual(1);
						cout << "Completed\n\n";
						break;
					case 2:
						cout << "Building D(G) as skew-right binary tree...\n";
						G.build_dual(2);
						cout << "Completed\n\n";
						break;
					case 3:
						cout << "Building D(G) as random binary tree...\n";
						G.build_dual(3);
						cout << "Completed\n\n";
						break;
					default:
						break;
				}

				break;

			case 2:
				cout << "Triangulating and building graph G...\n";
				G.build_polygon();
				cout << "Completed\n\n";
				break;

			case 3:
				cout << "Preprocessing graph G...\n";
				G.preprocess_graph();
				cout << "Completed\n\n";
				break;

			case 4:
				test::test_dag(G);
				break;

			case 5:
				test::test_ss(G);
				break;

			default:
				break;
		}

	}


	// btree::free_mem(G.mroot_node);
	return 0;
}
