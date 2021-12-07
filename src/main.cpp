#include <vector>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../test/test_dag.h"
#include "../test/test_sssp.h"



int main(int argc, char *argv[]){
	// init
	int V = 20;
	float beer_prob = 0.1;
	cout << "Enter total number of vertices: ";
	cin >> V;
	cout << "Enter probability of having a beer store: ";
	cin >> beer_prob;

	graph G(V, beer_prob);


	while(1){
		cout << "\t--------------------Main Manual--------------------\n";
		cout << "\t[1]	Build Dual D(G)\n";
			cout << "\t\t[1]	Generate balance dual D(G)\n";
			cout << "\t\t[2]	Generate skew-right dual D(G)\n";
			cout << "\t\t[3]	Generate random dual D(G)\n";
		cout << "\t[2]	Build Graph G\n";
		cout << "\t[3]	Preprocess graph G\n";
		cout << "\t[4]	Test Single-Pair Shortest (Beer) Path\n";
		cout << "\t[5]	Test Single-Source Shortest (Beer) Path\n";
		cout << "\t[6]	Query time\n";
		cout << "\t\t[1] Time Triangulating Graph G\n";
		cout << "\t\t[2] Time Preprcessing Dual D(G) into LCA\n";
		cout << "\t\t[3] Time Computing Beer Distances\n";
		cout << "\t\t[4] Time Preprcessing v-chain\n\n";

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
				cout << "|V| = " << G.mV << endl;
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

			case 6:
				cout << "Select a queried time: ";
				int time;
				cin >> time;

				switch(time){
					case 1:
						cout << "Triangulating G took: " << G.triangulate_duration << " s\n\n";
						break;
					case 2:
						cout << "Precompute LCA for D(G) took: " << G.lca_duration << " s\n\n";
						break;
					case 3:
						cout << "Computing Beer Distances took: " << G.beer_duration << " s\n\n";
						break;
					case 4:
						cout << "Preprocessing chains tooks: " << G.chain_duration << " s\n\n";
						break;
					default:
						break;
				}
				break;

			default:
				break;
		}

	}

	return 0;
}
