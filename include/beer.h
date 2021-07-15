#ifndef BEER_H
#define BEER_H
#include <tuple>
#include <string>

using namespace std;

#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/utils.h"

struct BEdge {
	struct Vertex* u;
	struct Vertex* v;
	double distB;
	tuple<struct Vertex*, struct HalfEdge*, bool, string> pathB;

	BEdge(struct Vertex* a, struct Vertex* b){
		u = a;
		v = b;
		distB = INF;
		pathB = NIL;
	}

	BEdge(){
	}
};


void beerDistNotRoot(struct HalfEdge* E, struct Node* F);
void computeBeerDistNotRoot(struct HalfEdge* E, struct Node* R);
void beerDistRoot(struct HalfEdge* E, struct Node* F);
void computeBeerDistRoot(struct HalfEdge* E, struct Node* R);
void print_beer_path(struct HalfEdge* e);
void print_beer_subpath(struct BEdge* e, struct Vertex* p);

#endif
