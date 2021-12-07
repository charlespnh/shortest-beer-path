#include <unordered_map>
#include <iostream>

using namespace std;

#include "verify.h"
#include "../../include/outerplanar.h"
#include "../../include/graph/dcel.h"
#include "../../include/utils.h"



bool verify::verify_tree(struct vertex* root_v, struct halfedge* root_e){
	struct halfedge* traverse_e = dcel::twin(root_e);
	struct vertex* curr_v = dcel::target(traverse_e);
	unordered_map<struct vertex*, int> visited;			// there's a path to root in SPTree

	int count = 0;			// detect loop
	visited[root_v] = count++;
	do {
		while (curr_v != root_v && visited.count(curr_v) == 0){
			visited[curr_v] = count;
			curr_v = dcel::pred(curr_v);
		}

		if (curr_v != root_v && visited[curr_v] == count){
			cout << "Cycle detected at " << dcel::data(curr_v) << "\n";
			return false;
		}

		count++;
		traverse_e = dcel::next(traverse_e);
		curr_v = dcel::target(traverse_e);
	} while(traverse_e != dcel::twin(root_e));

	return true; 
}


bool verify::verify_distance(struct vertex* v){
	if (dcel::pred(v) == NULL)
		return true;

	for (int k = 0; k < v->v_chain.size(); k++){
		if (dcel::dist(v) > dcel::dist(dcel::neighbour(v, k)) + dcel::weight(graph::get_edge(v, dcel::neighbour(v, k))))
			return false;
	}

	return true;
}


bool verify::verify_predecessor(struct vertex* v){
	if (dcel::pred(v) == NULL)
		return true;
	
	if (dcel::dist(v) > dcel::dist(dcel::pred(v)) + dcel::weight(graph::get_edge(v, dcel::pred(v))))
		return false;

	return true;
}



bool verify::verify_sssp(struct vertex* src, struct halfedge* root_e){
	bool verified;

	verified = verify::verify_tree(src, root_e);
	if (!verified){
		cout << "Predecessors computed induce a tree\n\n";
		return verified;
	}
	else cout << "Predecessors computed do not induce a tree\n\n";

	struct halfedge* traverse = dcel::twin(root_e);
	struct vertex* cur = dcel::target(traverse);
	do {
		verified = verify::verify_distance(cur);
		if (!verified){
			cout << "d(" << dcel::data(cur) << ") is incorrectly computed - a better neighbour found\n\n";
			return verified;
		}
		else cout << "d(" << dcel::data(cur) << ") is correctly computed - no better neighbour found\n\n";

		verified = verify::verify_predecessor(cur);
		if (!verified){
			cout << "d(" << dcel::data(cur) << ") is incorrectly computed - incorrect predecessor computed\n\n";
			return verified;
		}
		else cout << "d(" << dcel::data(cur) << ") is correctly computed - correct predecessor computed\n\n";

		traverse = dcel::next(traverse);
		cur = dcel::target(traverse);
	} while(traverse != dcel::twin(root_e));

	return true;
}