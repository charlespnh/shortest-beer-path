#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;

#include "test_dag.h"
#include "../include/dag.h"
#include "../include/graph/dcel.h"
#include "../include/utils.h"



void test::test_dag(graph& G){
	struct vertex* src;
	struct vertex* dest;
	dag D;



	while(1){
		cout << "\t--------------------Testing DAG Manual--------------------\n";
		cout << "\t[1]	Build DAG\n";
		cout << "\t[2]	Compute DAG Shortest Path\n";
		cout << "\t[3]	Compute DAG Shortest Beer Path\n";
		cout << "\t[4]	Query Time\n";
			cout << "\t\t[1]	Time Building DAG\n";
			cout << "\t\t[2]	Time Computing Shortest Path\n";
			cout << "\t\t[3]	Time Computing Shortest Beer Path\n";
			cout << "\t\t[4]	Time Getting Instance of Shortest Path\n";
			cout << "\t\t[5]	Time Getting Instance of Shortest Beer Path\n\n";

		clock_t start;

		int option;
		cout << "Choose a manual option: ";
		cin >> option;	cout << "\n";
		switch(option){
			case 1:
			{
				int s, d;
				cout << "Select source: ";
				cin >> s;
				cout << "Select destination: ";
				cin >> d;

				src = G.get_vertex(s);
				dest = G.get_vertex(d);
				if (!src || !dest){
					cout << "No vertex found...\n\n";
					break;
				}

				cout << "Building DAG...\n";
				D.reset();
				D.build_dag(G, src, dest);
				// for (auto v : D.mDAG){
				// 	cout << v->data << endl;
				// 	for (int i = 0; i < v->dag_edges.size(); i++){
				// 		cout << "DEdge -> " << v->dag_edges[i].first->dest->data << " w: " << v->dag_edges[i].first->weight << endl;
				// 		auto [apex, loc] = v->dag_edges[i].first->subpath;
				// 		cout << "Apex: ";
				// 		if (apex == NULL)
				// 			cout << "NULL ";
				// 		else 
				// 			cout << apex->data;
				// 		cout << "\t Loc: " << loc << endl;

				// 		cout << "DBEdge -> " << v->dag_edges[i].second->dest->data << " w: " << v->dag_edges[i].second->weight << endl;
				// 		auto [apexB, locB] = v->dag_edges[i].second->subpath;
				// 		cout << "Apex BR: ";
				// 		if (apexB == NULL)
				// 			cout << "NULL ";
				// 		else 
				// 			cout << apexB->data;
				// 		cout << "\t Loc BR: " << locB << endl;
				// 	}
				// 	cout << endl;
				// }
				cout << "Completed\n\n";
				break;
			}

			case 2:
			{
				cout << "Computing Shortest Path...\n";
				D.shortest_path_dag();
				cout << "Completed\n\n";
				// for (int i = 1; i < D.mDAG.size(); ++i)
				// 	printf("\u03B4(%d) = %f\tpred: %d\n", D.mDAG[i]->data, D.mDAG[i]->dist, D.mDAG[i]->pred->data);

				cout << "Printing Shortest Path...\n";
				printf("\u03B4(%d, %d) = %f\n", dcel::data(src), dcel::data(dest), dcel::dist(dest));
				start = clock();
				vector<struct vertex*> path = D.print_spsp_dag(dest);
				D.get_spsp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;

				cout << "PATH: ";
				for (auto v : path) cout << dcel::data(v) << " "; cout << "\n";
				cout << "Completed\n\n";
				break;
			}

			case 3:
			{
				cout << "Computing Shortest Beer Path...\n";
				D.shortest_beer_path_dag();
				cout << "Completed\n\n";
				// for (int i = 1; i < D.mDAG.size(); ++i)
				// 	printf("\u03B4B(%d) = %f\tpred: (%d, %d)\n", D.mDAG[i]->data, D.mDAG[i]->distB, 
				// 												 (D.mDAG[i]->predB.first) ? D.mDAG[i]->predB.first->data : -13,
				// 												 D.mDAG[i]->predB.second);
				
				cout << "Printing Shortest Beer Path...\n";
				printf("\u03B4B(%d, %d) = %f\n", dcel::data(src), dcel::data(dest), dcel::distB(dest));
				if (dcel::distB(dest) == INF){
					cout << "There Exists No Beer Path...\n";
					break;
				}

				start = clock();
				vector<struct vertex*> pathB = D.print_spsbp_dag(dest);
				D.get_spsbp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;

				cout << "PATH: ";
				for (auto v : pathB){ 
					cout << dcel::data(v);
					if (dcel::beer(v))
						cout << "(BR)";
					cout << " ";
				}

				cout << "\nCompleted\n\n";
				break;
			}

			case 4:
				int time;
				cout << "Select a queried time: ";
				cin >> time;
				switch(time){
					case 1:
						cout << "Building DAG took: " << D.build_duration << " s\n\n";
						break;
					case 2:
						cout << "Computing Shortest Path took: " << D.spsp_duration << " s\n\n";
						break;
					case 3:
						cout << "Computing Shortest Beer Path took: " << D.spsbp_duration << " s\n\n";
						break;
					case 4:
						cout << "Getting Actual Shortest Path took: " << D.get_spsp_duration << " s\n\n";
						break;
					case 5:
						cout << "Getting Actual Shortest Beer Path took: " << D.get_spsbp_duration << " s\n\n";
						break;
					default:
						break;
				}
				break;

			default:
				D.reset();
				// exit(1);
				break; 
		}
	}

	return;
}
