#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../include/beer.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/utils.h"


// post-order traversal of D(G)
// compute distB(a, b, G\R)
void beerDistNotRoot(struct HalfEdge* ab, struct Node* F){
	if (F == NULL) return;
	struct HalfEdge* ac = ab->prev;
	struct HalfEdge* bc = ab->next;
	beerDistNotRoot(ac->twin, ac->twin->incident_face);
	beerDistNotRoot(bc->twin, bc->twin->incident_face);

	// for all v in face F
	for (int i = 0; i < 3; i++){
		struct HalfEdge* uv = ab;
		struct HalfEdge* vw = bc;
		struct Vertex* v = uv->target;
		struct Vertex* u = uv->prev->target;
		struct Vertex* w = vw->target;

		v->beer_edge->distB = min(v->beer_edge->distB, min(2 * uv->weight + u->beer_edge->distB, 
														   2 * vw->weight + w->beer_edge->distB));
		if (v->beer_edge->distB == 2 * uv->weight + u->beer_edge->distB)
			v->beer_edge->pathB = make_tuple(u, uv, 0, "\0");
		else if (v->beer_edge->distB == 2 * vw->weight + w->beer_edge->distB)
			v->beer_edge->pathB = make_tuple(w, vw, 0, "\0");

		ab = ab->next;
	}

	// distB(a, b, G\R)
	struct Vertex* a = ab->prev->target;
	struct Vertex* b = ab->target;
	struct Vertex* c = ab->next->target;

	if (ab->beer_edge->u != a) swap(ab->beer_edge->u, ab->beer_edge->v);
	ab->beer_edge->distB = min(min(ab->weight + a->beer_edge->distB, ab->weight + b->beer_edge->distB), 
							   min(ac->weight + bc->beer_edge->distB, ac->beer_edge->distB + bc->weight));
	if (ab->beer_edge->distB == ab->weight + a->beer_edge->distB)
		ab->beer_edge->pathB = make_tuple(a, ab, 0, "BNR1");
	else if (ab->beer_edge->distB == ab->weight + b->beer_edge->distB)
		ab->beer_edge->pathB = make_tuple(b, ab, 1, "BNR2");
	else if (ab->beer_edge->distB == ac->weight + bc->beer_edge->distB)
		ab->beer_edge->pathB = make_tuple(c, bc, 1, "BNR3");
	else ab->beer_edge->pathB = make_tuple(c, ac, 0, "BNR4");

}

// compute beer dist for edge E in subgraph G not R
void computeBeerDistNotRoot(struct HalfEdge* E, struct Node* R){
	struct HalfEdge* traverse_e = E->twin;
	struct Vertex *u, *v, *w;
	struct HalfEdge *uv, *vw; 
	// Base case post-order traversal: for all v in V(G)
	do {
		v = traverse_e->target;
		if (v->beer)
			v->beer_edge->distB = 0;
		//    v->beer_edge->beer = NIL;
		// else v->beer_edge->distB = INF;
		traverse_e = traverse_e->next;
	} while(traverse_e != E->twin);

	// Base case post-order traversal: for all exterior edge (u, v) in E(G)
	do {
		uv = traverse_e;
		u = uv->prev->target;
		v = uv->target;

		if (uv->beer_edge->u != u) swap(uv->beer_edge->u, uv->beer_edge->v);
		if (u->beer){
			uv->beer_edge->distB = uv->weight;
			uv->beer_edge->pathB = make_tuple(u, uv, 0, "CBNR1");
		}
		else if (v->beer){
			uv->beer_edge->distB = uv->weight;
			uv->beer_edge->pathB = make_tuple(v, uv, 1, "CBNR2");
		}
		// else uv->beer_edge->distB = INF;

		traverse_e = traverse_e->next;
	} while(traverse_e != E->twin);

	// Recurrence: for all interior edges in face R
	struct HalfEdge* interior_e = E;
	for (int i = 0; i < 3; i++){
		beerDistNotRoot(interior_e->twin, interior_e->twin->incident_face);
		interior_e = interior_e->next;
	}

	// Base case pre-order traversal: for all v in face R
	for (int i = 0; i < 3; i++){
		uv = interior_e;
		vw = interior_e->next;
		v = uv->target;						/* (u -> v)          v     */
		u = uv->prev->target;				/* (w -> u)        / R \   */
		w = vw->target;						/* (v -> w)       w --- u  */

		v->beer_edge->distB = min(v->beer_edge->distB, min(2 * uv->weight + u->beer_edge->distB,
														   2 * vw->weight + w->beer_edge->distB));
		if (v->beer_edge->distB == 2 * uv->weight + u->beer_edge->distB)
			v->beer_edge->pathB = make_tuple(u, uv, 0, "\0");
		else if (v->beer_edge->distB == 2 * vw->weight + w->beer_edge->distB)
			v->beer_edge->pathB = make_tuple(w, vw, 0, "\0");

		interior_e = interior_e->next;
	}
}




// pre-order traversal of D(G)
void beerDistRoot(struct HalfEdge* ab, struct Node* F){
	// distB(a, b, G_R) is already computed (in previous recursive call during pre-order traversal)
	if (F == NULL) return;
	struct HalfEdge* ac = ab->prev;
	struct HalfEdge* bc = ab->next;
	struct Vertex* a = ac->target;
	struct Vertex* b = ab->target;
	struct Vertex* c = bc->target;

	c->beer_edge->distB = min(c->beer_edge->distB, min(2 * ac->weight + a->beer_edge->distB, 
													   2 * bc->weight + b->beer_edge->distB));
	if (c->beer_edge->distB == 2 * ac->weight + a->beer_edge->distB)
		c->beer_edge->pathB = make_tuple(a, ac, 0, "\0");
	else if	(c->beer_edge->distB == 2 * bc->weight + b->beer_edge->distB) 
		c->beer_edge->pathB = make_tuple(b, bc, 0, "\0");

	// compute distB(a, c, G_R)
	bool swapped = false;
	if (ac->beer_edge->u != a){
		swap(ac->beer_edge->u, ac->beer_edge->v);
		swapped = true;
	}
	ac->beer_edge->distB = min(ac->beer_edge->distB, min(min(ac->weight + a->beer_edge->distB, ac->weight + c->beer_edge->distB), 
														 min(ab->beer_edge->distB + bc->weight, ab->weight + bc->beer_edge->distB)));
	if (ac->beer_edge->distB == ac->weight + a->beer_edge->distB)
		ac->beer_edge->pathB = make_tuple(a, ac, 0, "BR1");
	else if	(ac->beer_edge->distB == ac->weight + c->beer_edge->distB)
		ac->beer_edge->pathB = make_tuple(c, ac, 1, "BR2");
	else if	(ac->beer_edge->distB == ab->beer_edge->distB + bc->weight)
		ac->beer_edge->pathB = make_tuple(b, ab, 0, "BR3");
	else if	(ac->beer_edge->distB == ab->weight + bc->beer_edge->distB)
		ac->beer_edge->pathB = make_tuple(b, bc, 1, "BR4");
	else if (swapped) swap(ac->beer_edge->u, ac->beer_edge->v);

	// compute distB(b, c, G_R)
	swapped = false;
	if (bc->beer_edge->u != b){
		swap(bc->beer_edge->u, bc->beer_edge->v);
		swapped = true;
	}
	bc->beer_edge->distB = min(bc->beer_edge->distB, min(min(bc->weight + b->beer_edge->distB, bc->weight + c->beer_edge->distB),
														 min(ab->beer_edge->distB + ac->weight, ab->weight + ac->beer_edge->distB)));
	if (bc->beer_edge->distB == bc->weight + b->beer_edge->distB)
		bc->beer_edge->pathB = make_tuple(b, bc, 0, "BR5");
	else if (bc->beer_edge->distB == bc->weight + c->beer_edge->distB)
		bc->beer_edge->pathB = make_tuple(c, bc, 1, "BR6");
	else if (bc->beer_edge->distB == ab->beer_edge->distB + ac->weight)
		bc->beer_edge->pathB = make_tuple(a, ab, 0, "BR7");
	else if (bc->beer_edge->distB == ab->weight + ac->beer_edge->distB)
		bc->beer_edge->pathB = make_tuple(a, ac, 1, "BR8");
	else if (swapped) swap(bc->beer_edge->u, bc->beer_edge->v);

	beerDistRoot(ac->twin, ac->twin->incident_face);
	beerDistRoot(bc->twin, bc->twin->incident_face);
}

// compute beer dist for all edge E in subgraph G with R
void computeBeerDistRoot(struct HalfEdge* E, struct Node* R){
	// Base case pre-order traversal: for all v in face R... computed during post-order traversal

	// Base case pre-order traversal: for all edge (u, v) in face R
	for (int i = 0; i < 3; i++){
		struct HalfEdge* uv = E;
		struct HalfEdge* uw = E->prev;
		struct HalfEdge* wv = E->next;
		struct Vertex* v = uv->target;
		struct Vertex* u = uw->target;
		struct Vertex* w = wv->target;

		bool swapped = false;
		if (uv->beer_edge->u != u){
			swap(uv->beer_edge->u, uv->beer_edge->v);
			swapped = true;
		}
		uv->beer_edge->distB = min(uv->beer_edge->distB, min(min(uv->weight + u->beer_edge->distB, uv->weight + v->beer_edge->distB), 
								   						 	 min(uw->beer_edge->distB + wv->weight, uw->weight + wv->beer_edge->distB)));
		if (uv->beer_edge->distB == uv->weight + u->beer_edge->distB)
			uv->beer_edge->pathB = make_tuple(u, uv, 0, "CBR1");
		else if (uv->beer_edge->distB == uv->weight + v->beer_edge->distB)
			uv->beer_edge->pathB = make_tuple(v, uv, 1, "CBR2");
		else if (uv->beer_edge->distB == uw->beer_edge->distB + wv->weight)
			uv->beer_edge->pathB = make_tuple(w, uw, 0, "CBR3");
		else if (uv->beer_edge->distB == uw->weight + wv->beer_edge->distB)
			uv->beer_edge->pathB = make_tuple(w, wv, 1, "CBR4");
		else if (swapped) swap(uv->beer_edge->u, uv->beer_edge->v);

		E = E->next;
	}

	for (int i = 0; i < 3; i++){
		beerDistRoot(E->twin, E->twin->incident_face);
		E = E->next;
	}
}



void print_beer_path(struct HalfEdge* uv){
	printf("beer path edge (%d, %d): %d ", uv->beer_edge->u->data, uv->beer_edge->v->data, uv->beer_edge->u->data);
	print_beer_subpath(uv->beer_edge, uv->beer_edge->u);
	cout << "\n";
}

void print_beer_subpath(struct BEdge* e, struct Vertex* p){
	if (e->pathB == NIL) return;
	
	// u - w - v
	auto [w, subpathB, beerLoc, str] = e->pathB;	
	if (p && p != e->u){
		swap(e->u, e->v);
		get<2>(e->pathB) = beerLoc = !beerLoc;
	}
	printf("[%d %d %d (%d ", e->u->data, w->data, e->v->data, beerLoc); cout << str << ")] ";

	if (w == e->u || w == e->v || e->u == e->v){
		// w = u or v
		if (beerLoc == 0){
			// u = w and u - w is beer subpath
			print_beer_subpath(w->beer_edge, p);
			printf("c10 %d ", e->v->data);
		}
		else{
			// v = w and w -> v is beer subpath
			printf("c11 %d ", w->data);
			print_beer_subpath(w->beer_edge, w);
		}
	}

	else{
		// w != u or v
		if (beerLoc == 0){
			// u - w is the beer subpath
			print_beer_subpath(subpathB->beer_edge, p);
			printf("c20 %d ", e->v->data);
		}
		else{
			// w - v is the beer subpath
			printf("c21 %d ", w->data);
			print_beer_subpath(subpathB->beer_edge, w);
		}
	}
}

