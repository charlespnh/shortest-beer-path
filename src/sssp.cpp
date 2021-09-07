#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../include/sssp.h"
#include "../include/outerplanar.h"
#include "../include/graph/dcel.h"
#include "../include/preprocess/beer.h"
#include "../include/utils.h"


void single_source::preorder_traversal_sssp(struct halfedge* ab){
	if (ab->incident_face == NULL) return;
	struct halfedge* ac = ab->prev;
	struct halfedge* bc = ab->next;

	struct vertex* a = ac->target;
	struct vertex* b = ab->target;
	struct vertex* c = bc->target;

	c->dist = min(a->dist + ac->weight, b->dist + bc->weight);
	if (c->dist == a->dist + ac->weight)
		c->pred = a;
	else
		c->pred = b;

	single_source::preorder_traversal_sssp(ac->twin);
	single_source::preorder_traversal_sssp(bc->twin);
	return;
}


void single_source::sssp(struct halfedge* root_e){
	clock_t start = clock();
	struct vertex* s = root_e->target;
	struct vertex* a = root_e->prev->target;
	struct vertex* b = root_e->next->target;

	s->dist = 0.0;
	s->pred = NULL;
	a->dist = root_e->weight;
	b->dist = root_e->next->weight;
	a->pred = b->pred = s;

	for (int i = 0; i < 3; i++){
		single_source::preorder_traversal_sssp(root_e->twin);
		root_e = root_e->next;
	}

	single_source::sssp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	return;
}




void single_source::preorder_traversal_sssbp(struct halfedge* ab){
	if (ab->incident_face == NULL) return;
	struct halfedge* ac = ab->prev;
	struct halfedge* bc = ab->next;
	struct vertex* a = ac->target;
	struct vertex* b = ab->target;
	struct vertex* c = bc->target;

	c->distB = min(min(a->dist + ac->beer_edge->distB, b->dist + bc->beer_edge->distB),
				   min(a->distB + ac->weight, b->distB + bc->weight));
	if (c->distB == a->dist + ac->beer_edge->distB)
		c->predB = make_pair(a, 1);
	else if (c->distB == b->dist + bc->beer_edge->distB)
		c->predB = make_pair(b, 1);
	else if (c->distB == a->distB + ac->weight)
		c->predB = make_pair(a, 0);
	else 
		c->predB = make_pair(b, 0);

	single_source::preorder_traversal_sssbp(ac->twin);
	single_source::preorder_traversal_sssbp(bc->twin);
	return;
}


void single_source::sssbp(struct halfedge* root_e){
	clock_t start = clock();
	struct vertex* s = root_e->target;	
	struct vertex* a = root_e->prev->target;
	struct vertex* b = root_e->next->target;

	s->distB = s->beer_edge->distB;
	s->predB = NIL;
	a->distB = root_e->beer_edge->distB;
	a->predB = make_pair(s, 1);
	b->distB = root_e->next->beer_edge->distB;
	b->predB = make_pair(s, 1);

	for (int i = 0; i < 3; i++){
		single_source::preorder_traversal_sssbp(root_e->twin);
		root_e = root_e->next;
	}

	single_source::sssbp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	return;
}





vector<struct vertex*> single_source::print_sssp(struct vertex* v){
	vector<struct vertex*> spath;
	if (v == NULL) return spath;
	
	spath = single_source::print_sssp(v->pred);
	spath.push_back(v);
	return spath;
}



vector<struct vertex*> single_source::print_sssbp(struct vertex* v){
	vector<struct vertex*> path = single_source::print_sssbp_subpath(v);
	return path_union(path);
}


vector<struct vertex*> single_source::print_sssbp_subpath(struct vertex* v){
	vector<struct vertex*> pathB;
	auto [w, beerLoc] = v->predB;
	if (w == NULL) return pathB;

	vector<struct vertex*> subPathB;

	if (beerLoc == 1){
		subPathB = beer::print_beer_path(graph::get_edge(w, v));
		pathB = single_source::print_sssp(w);
	}
	else{
		subPathB = {w, v};
		pathB = single_source::print_sssbp_subpath(w);
	}

	pathB.insert(pathB.end(), subPathB.begin(), subPathB.end());
	return pathB;
}
