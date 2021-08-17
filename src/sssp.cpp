#include <algorithm>
#include <string>
#include <cstdio>
#include <iostream>

using namespace std;

#include "sssp.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/beer.h"


void preorder_traversal_sssp(struct HalfEdge* ab){
	if (ab->incident_face == NULL) return;
	struct HalfEdge* ac = ab->prev;
	struct HalfEdge* bc = ab->next;

	struct Vertex* a = ac->target;
	struct Vertex* b = ab->target;
	struct Vertex* c = bc->target;

	c->dist = min(a->dist + ac->weight, b->dist + bc->weight);
	if (c->dist == a->dist + ac->weight)
		c->pred = a;
	else
		c->pred = b;

	preorder_traversal_sssp(ac->twin);
	preorder_traversal_sssp(bc->twin);
}

void sssp(struct HalfEdge* root_e){
	struct Vertex* s = root_e->target;
	struct Vertex* a = root_e->prev->target;
	struct Vertex* b = root_e->next->target;

	s->dist = 0.0;
	s->pred = NULL;
	a->dist = root_e->weight;
	b->dist = root_e->next->weight;
	a->pred = b->pred = s;

	for (int i = 0; i < 3; i++){
		preorder_traversal_sssp(root_e->twin);
		root_e = root_e->next;
	}
}

void preorder_traversal_sssbp(struct HalfEdge* ab){
	if (ab->incident_face == NULL) return;
	struct HalfEdge* ac = ab->prev;
	struct HalfEdge* bc = ab->next;
	struct Vertex* a = ac->target;
	struct Vertex* b = ab->target;
	struct Vertex* c = bc->target;

	c->distB = min(min(a->dist + ac->beer_edge->distB, b->dist + bc->beer_edge->distB),
				   min(a->distB + ac->weight, b->distB + bc->weight));
	if (c->distB == a->dist + ac->beer_edge->distB)
		c->predB = make_pair(ac, 1);
	else if (c->distB == b->dist + bc->beer_edge->distB)
		c->predB = make_pair(bc, 1);
	else if (c->distB == a->distB + ac->weight)
		c->predB = make_pair(ac, 0);
	else 
		c->predB = make_pair(bc, 0);

	preorder_traversal_sssbp(ac->twin);
	preorder_traversal_sssbp(bc->twin);
}

void sssbp(struct HalfEdge* root_e){
	struct Vertex* s = root_e->target;	
	struct Vertex* a = root_e->prev->target;
	struct Vertex* b = root_e->next->target;

	s->distB = s->beer_edge->distB;
	s->predB = make_pair((struct HalfEdge*) NULL, 1);
	a->distB = root_e->beer_edge->distB;
	a->predB = make_pair(root_e, 1);
	b->distB = root_e->next->beer_edge->distB;
	b->predB = make_pair(root_e->next, 1);

	for (int i = 0; i < 3; i++){
		preorder_traversal_sssbp(root_e->twin);
		root_e = root_e->next;
	}
}



vector<struct Vertex*> print_sssp(struct Vertex* v){
	vector<struct Vertex*> spath;
	if (v == NULL) return spath;
	
	spath = print_sssp(v->pred);
	spath.push_back(v);
	return spath;
}

vector<struct Vertex*> print_sssbp(struct Vertex* v){
	return print_sssbp_subpath(v);
}

vector<struct Vertex*> print_sssbp_subpath(struct Vertex* v){
	vector<struct Vertex*> pathB;
	auto [subPath, beerLoc] = v->predB;
	if (subPath == NULL) return pathB;

	vector<struct Vertex*> subPathB;
	struct Vertex* w = (subPath->target != v) ? subPath->target : subPath->prev->target;

	if (beerLoc){
		subPathB = print_beer_path(subPath->beer_edge);
		pathB = print_sssp(w);
	}
	else{
		subPathB = {w, v};
		pathB = print_sssbp_subpath(w);
	}

	if (pathB.back() != subPathB[0]) reverse(subPathB.begin(), subPathB.end());
	pathB.insert(pathB.end(), subPathB.begin(), subPathB.end());
	return pathB;
}
