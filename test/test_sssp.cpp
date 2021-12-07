#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;

#include "test_sssp.h"
#include "../include/sssp.h"
#include "../utils/verification/verify.h"
// #include "../include/outerplanar.h"
#include "../include/graph/dcel.h"



void test::test_ss(graph& G){
	while(1){
		cout << "\n\t--------------------Testing Single-Source Manual--------------------\n";
		cout << "\t[1]	Select source\n";
		cout << "\t[2]	Compute Single-Source Shortest Path\n";
		cout << "\t[3]	Compute Single-Source Shortest Beer Path\n";
		cout << "\t[4]	Query time\n";
			cout << "\t\t[1]	Time computing shortest path\n";
			cout << "\t\t[2]	Time computing shortest beer path\n";
		cout << "\t[5]	Verification\n";
			cout << "\t\t[1]	Verify shortest path\n";
			cout << "\t\t[2]	Verify shortest beer path\n\n";
		int s;
		struct vertex* src;
		struct halfedge* root_edge;
		struct halfedge* traverse = dcel::twin(G.mroot_edge);

		int option;
		cout << "Choose a manual option: ";
		cin >> option;	cout << "\n";

		switch(option){
			case 1:
			{
				cout << "Select source: ";
				cin >> s;	cout << "\n";
				src = G.get_vertex(s);
				if (s == -1)
					root_edge = dcel::prev(G.mroot_edge);
				else if (s == 0)
					root_edge = G.mroot_edge;
				else{
					root_edge = dcel::twin(G.mroot_edge);
					do {
						root_edge = dcel::next(root_edge);
						if (dcel::data(dcel::target(root_edge)) == s){
							root_edge = dcel::prev(dcel::twin(root_edge));
							break;
						}
					} while(root_edge != dcel::twin(G.mroot_edge));
				}

				break;
			}

			case 2:
				cout << "Computing shortest path...\n";
				single_source::sssp(root_edge);
				cout << "Completed\n\n";

				do {
					printf("\u03B4(%d, %d) =  %f\n", s, dcel::data(dcel::target(traverse)), dcel::dist(dcel::target(traverse))); 
					vector<struct vertex*> sssp = single_source::print_sssp(dcel::target(traverse));

					cout << "PATH: ";
					for (auto v : sssp) cout << dcel::data(v) << " ";	cout << "\n";
					cout << "Completed\n\n";

					traverse = dcel::next(traverse);
				} while(traverse != dcel::twin(G.mroot_edge));

				break;

			case 3:
				cout << "Computing shortest beer path...\n";
				single_source::sssbp(root_edge);
				cout << "Completed\n\n";

				do {
					printf("\u03B4B(%d, %d) = %f\n", s, dcel::data(dcel::target(traverse)), dcel::distB(dcel::target(traverse))); 
					vector<struct vertex*> sssbp = single_source::print_sssbp(dcel::target(traverse));
					
					cout << "PATH: ";
					for (auto v : sssbp){
						cout << dcel::data(v);
						if (dcel::beer(v))
							cout << "(BR)";
						cout << " ";
					}
					cout << "\nCompleted\n\n";

					traverse = dcel::next(traverse);
				} while(traverse != dcel::twin(G.mroot_edge));

				break;

			case 4:
			{
				int time;
				cout << "Select a query time: ";
				cin >> time;

				switch(time){
					case 1:
						cout << "Computing shortest path took: " << single_source::sssp_duration << " s\n\n";
						break;

					case 2:						
						cout << "Computing shortest beer path took: " << single_source::sssbp_duration << " s\n\n";
						break;

					default:
						break;
				}

				break;
			}

			case 5:
			{
				int verification;
				cout << "Select an option to verify: ";
				cin >> verification;

				switch(verification){
					case 1:
						cout << "Verifying shortest path...\n";
						verify::verify_sssp(src, G.mroot_edge);
						break;
					case 2:
						cout << "Verifying shortest beer path...\n";
						verify::verify_sssbp(src, G.mroot_edge);
						break;
					default:
						break;
				}

				break;
			}

			default:
				break;
		}

	}

}