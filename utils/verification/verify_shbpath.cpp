#include <iostream>

using namespace std;

#include "verify.h"
#include "../../include/outerplanar.h"
#include "../../include/graph/dcel.h"
#include "../../include/preprocess/beer.h"


bool verify::verify_distanceB(struct vertex* v){
	for (int k = 0; k < v->v_chain.size(); k++){
		if (dcel::dist(v) > min(dcel::dist(dcel::neighbour(v, k)) + beer::weightB(graph::get_edge(v, dcel::neighbour(v, k))),
								dcel::distB(dcel::neighbour(v, k)) + dcel::weight(graph::get_edge(v, dcel::neighbour(v, k)))))
			return false;
	}

	return true;
}


bool verify::verify_predecessorB(struct vertex* v){
	if (dcel::dist(v) > min(dcel::dist(dcel::predB(v).first) + beer::weightB(graph::get_edge(v, dcel::pred(v))), 
							dcel::distB(dcel::predB(v).first) + dcel::weight(graph::get_edge(v, dcel::pred(v)))))
		return false;

	return true;
}



bool verify::verify_sssbp(struct vertex* src, struct halfedge* root_e){
	struct halfedge* traverse = dcel::twin(root_e);
	struct vertex* cur = dcel::target(traverse);

	do {
		bool verified;
		verified = verify::verify_distanceB(cur);
		if (!verified){
			cout << "dB(" << dcel::data(cur) << ") is incorrectly computed - a better neighbour found\n\n";
			return verified;
		}
		else cout << "dB(" << dcel::data(cur) << ") is correctly computed - no better neighbour found\n\n";

		verified = verify::verify_predecessorB(cur);
		if (!verified){
			cout << "dB(" << dcel::data(cur) << ") is incorrectly computed - incorrect predecessor computed\n\n";
			return verified;
		}
		else cout << "dB(" << dcel::data(cur) << ") is correctly computed - correct predecessor computed\n\n";

		traverse = dcel::next(traverse);
		if (src == dcel::target(traverse))
			traverse = dcel::next(traverse);
		cur = dcel::target(traverse);
	} while(traverse != dcel::twin(root_e));

	return false;
}


