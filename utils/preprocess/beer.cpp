#include <algorithm>
#include <utility>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../../include/preprocess/beer.h"
#include "../../include/outerplanar.h"
#include "../../include/graph/dcel.h"



double beer::weightB(struct halfedge* e)						{ return dcel::edgeB(e)->distB; }
double beer::weightB(struct vertex* v)							{ return dcel::edgeB(v)->distB; }

struct vertex* beer::originB(struct halfedge* e)				{ return dcel::edgeB(e)->u; }
struct vertex* beer::targetB(struct halfedge* e)				{ return dcel::edgeB(e)->v; }
struct vertex* beer::originB(struct vertex* v)					{ return v; }
struct vertex* beer::targetB(struct vertex* v)					{ return v; }

struct b_edge* beer::get_edgeB(struct vertex* u, struct vertex* v){
	if (u == v) return dcel::edgeB(u);

	struct halfedge* uv = graph::get_edge(u, v);
	if (uv) return dcel::edgeB(uv);

	return NULL;
}



// post-order traversal of D(G)
// compute distB(a, b, G\R)
void beer::beerDistNotRoot(struct node* F){
	if (F == NULL) return;
	struct halfedge* ab = dcel::edge(F);
	struct halfedge* ac = dcel::prev(ab);
	struct halfedge* bc = dcel::next(ab);
	beer::beerDistNotRoot(dcel::left(F));
	beer::beerDistNotRoot(dcel::right(F));

	// for all v in face F
	for (int i = 0; i < 3; i++, ab = dcel::next(ab)){
		struct halfedge* uv = ab;
		struct halfedge* vw = bc;
		struct vertex* v = dcel::target(uv);
		struct vertex* u = dcel::origin(uv);
		struct vertex* w = dcel::target(vw);

		v->beer_edge->distB = min(beer::weightB(v), min(2 * dcel::weight(uv) + beer::weightB(u), 
												  2 * dcel::weight(vw) + beer::weightB(w)));
		if (beer::weightB(v) == 2 * dcel::weight(uv) + beer::weightB(u))
			v->beer_edge->pathB = make_pair(u, 0);
		else if (beer::weightB(v) == 2 * dcel::weight(vw) + beer::weightB(w))
			v->beer_edge->pathB = make_pair(w, 0);
	}

	// distB(a, b, G\R)
	struct vertex* a = dcel::target(ac);
	struct vertex* b = dcel::target(ab);
	struct vertex* c = dcel::target(bc);

	if (beer::originB(ab) != a) swap(ab->beer_edge->u, ab->beer_edge->v);
	ab->beer_edge->distB = min(min(dcel::weight(ab) + beer::weightB(a), dcel::weight(ab) + beer::weightB(b)), 
							   min(beer::weightB(ac) + dcel::weight(bc), dcel::weight(ac) + beer::weightB(bc)));
	if (beer::weightB(ab) == dcel::weight(ab) + beer::weightB(a))
		ab->beer_edge->pathB = make_pair(a, 0);
	else if (beer::weightB(ab) == dcel::weight(ab) + beer::weightB(b))
		ab->beer_edge->pathB = make_pair(b, 1);
	else if (beer::weightB(ab) == beer::weightB(ac) + dcel::weight(bc))
		ab->beer_edge->pathB = make_pair(c, 0);
	else ab->beer_edge->pathB = make_pair(c, 1);

}

// compute beer dist for edge E in subgraph G not R
void beer::computeBeerDistNotRoot(struct node* R){
	struct halfedge* E = dcel::edge(R);
	struct halfedge* traverse_e = dcel::twin(E);
	struct vertex *u, *v, *w;
	struct halfedge *uv, *vw; 
	// Base case post-order traversal: for all v in V(G)
	do {
		v = dcel::target(traverse_e);
		if (dcel::beer(v))
			v->beer_edge->distB = 0;
		//    v->beer_edge->beer = NIL;
		// else v->beer->distB = INF;
		traverse_e = dcel::next(traverse_e);
	} while(traverse_e != dcel::twin(E));

	// Base case post-order traversal: for all exterior edge (u, v) in E(G)
	do {
		uv = traverse_e;
		u = dcel::origin(uv);
		v = dcel::target(uv);

		if (dcel::beer(u) || dcel::beer(v))
			uv->beer_edge->distB = dcel::weight(uv);
			// uv->beer_edge->pathB = NIL;
		// else uv->beer_edge->distB = INF;

		traverse_e = dcel::next(traverse_e);
	} while(traverse_e != dcel::twin(E));

	// Recurrence: for all interior edges in face R
	for (int i = 0; i < 3; i++, E = dcel::next(E))
		beer::beerDistNotRoot(dcel::face(dcel::twin(E)));

	// Base case pre-order traversal: for all v in face R
	for (int i = 0; i < 3; i++, E = dcel::next(E)){
		uv = E;
		vw = dcel::next(E);
		v = dcel::target(uv);						/*  (u -> v)          v     */
		u = dcel::origin(uv);						/*  (w -> u)        / R \   */
		w = dcel::target(vw);						/*  (v -> w)       w --- u  */

		v->beer_edge->distB = min(beer::weightB(v), min(2 * dcel::weight(uv) + beer::weightB(u),
												  2 * dcel::weight(vw) + beer::weightB(w)));
		if (beer::weightB(v) == 2 * dcel::weight(uv) + beer::weightB(u))
			v->beer_edge->pathB = make_pair(u, 0);
		else if (beer::weightB(v) == 2 * dcel::weight(vw) + beer::weightB(w))
			v->beer_edge->pathB = make_pair(w, 0);
	}

}




// pre-order traversal of D(G)
void beer::beerDistRoot(struct node* F){
	// distB(a, b, G_R) is already computed (in previous recursive call during pre-order traversal)
	if (F == NULL) return;
	struct halfedge* ab = dcel::edge(F);
	struct halfedge* ac = dcel::prev(ab);
	struct halfedge* bc = dcel::next(ab);
	struct vertex* a = dcel::target(ac);
	struct vertex* b = dcel::target(ab);
	struct vertex* c = dcel::target(bc);
	double tmp;

	c->beer_edge->distB = min(beer::weightB(c), min(2 * dcel::weight(ac) + beer::weightB(a), 
											  2 * dcel::weight(bc) + beer::weightB(b)));
	if (beer::weightB(c) == 2 * dcel::weight(ac) + beer::weightB(a))
		c->beer_edge->pathB = make_pair(a, 0);
	else if	(beer::weightB(c) == 2 * dcel::weight(bc) + beer::weightB(b)) 
		c->beer_edge->pathB = make_pair(b, 0);

	// compute distB(c, a, G_R)
	bool relax = true;
	tmp = beer::weightB(ac);
	ac->beer_edge->distB = min(beer::weightB(ac), min(min(dcel::weight(ac) + beer::weightB(a), dcel::weight(ac) + beer::weightB(c)), 
												min(beer::weightB(ab) + dcel::weight(bc), dcel::weight(ab) + beer::weightB(bc))));
	if (beer::weightB(ac) == tmp) relax = false;
	else if (beer::weightB(ac) == dcel::weight(ac) + beer::weightB(a))
		ac->beer_edge->pathB = make_pair(a, 1);
	else if	(beer::weightB(ac) == dcel::weight(ac) + beer::weightB(c))
		ac->beer_edge->pathB = make_pair(c, 0);
	else if	(beer::weightB(ac) == beer::weightB(ab) + dcel::weight(bc))
		ac->beer_edge->pathB = make_pair(b, 1);
	else
		ac->beer_edge->pathB = make_pair(b, 0);

	if (relax && beer::originB(ac) != c)
		swap(ac->beer_edge->u, ac->beer_edge->v);


	// compute distB(b, c, G_R)
	relax = true;
	tmp = beer::weightB(bc);
	bc->beer_edge->distB = min(beer::weightB(bc), min(min(dcel::weight(bc) + beer::weightB(b), dcel::weight(bc) + beer::weightB(c)),
												min(beer::weightB(ab) + dcel::weight(ac), dcel::weight(ab) + beer::weightB(ac))));
	if (beer::weightB(bc) == tmp) relax = false;
	else if (beer::weightB(bc) == dcel::weight(bc) + beer::weightB(b))
		bc->beer_edge->pathB = make_pair(b, 0);
	else if (beer::weightB(bc) == dcel::weight(bc) + beer::weightB(c))
		bc->beer_edge->pathB = make_pair(c, 1);
	else if (beer::weightB(bc) == beer::weightB(ab) + dcel::weight(ac))
		bc->beer_edge->pathB = make_pair(a, 0);
	else
		bc->beer_edge->pathB = make_pair(a, 1);

	if (relax && beer::originB(bc) != b)
		swap(bc->beer_edge->u, bc->beer_edge->v);

	beer::beerDistRoot(dcel::left(F));
	beer::beerDistRoot(dcel::right(F));
}

// compute beer dist for all edge E in subgraph G with R
void beer::computeBeerDistRoot(struct node* R){
	// Base case pre-order traversal: for all v in face R... computed during post-order traversal

	// Base case pre-order traversal: for all edge (u, v) in face R
	struct halfedge* E = dcel::edge(R);
	for (int i = 0; i < 3; i++, E = dcel::next(E)){
		struct halfedge* uv = E;
		struct halfedge* uw = dcel::prev(E);
		struct halfedge* wv = dcel::next(E);
		struct vertex* v = dcel::target(uv);
		struct vertex* u = dcel::target(uw);
		struct vertex* w = dcel::target(wv);
		double tmp;

		bool relax = true;
		tmp = beer::weightB(uv);
		uv->beer_edge->distB = min(beer::weightB(uv), min(min(dcel::weight(uv) + beer::weightB(u), dcel::weight(uv) + beer::weightB(v)), 
													min(beer::weightB(uw) + dcel::weight(wv), dcel::weight(uw) + beer::weightB(wv))));
		if (beer::weightB(uv) == tmp) relax = false;
		else if (beer::weightB(uv) == dcel::weight(uv) + beer::weightB(u))
			uv->beer_edge->pathB = make_pair(u, 0);
		else if (beer::weightB(uv) == dcel::weight(uv) + beer::weightB(v))
			uv->beer_edge->pathB = make_pair(v, 1);
		else if (beer::weightB(uv) == beer::weightB(uw) + dcel::weight(wv))
			uv->beer_edge->pathB = make_pair(w, 0);
		else
			uv->beer_edge->pathB = make_pair(w, 1);

		if (relax && beer::originB(uv) != u)
			swap(uv->beer_edge->u, uv->beer_edge->v);

	}

	for (int i = 0; i < 3; i++, E = dcel::next(E))
		beer::beerDistRoot(dcel::face(dcel::twin(E)));

}





vector<struct vertex*> beer::print_beer_path(struct vertex* v){
	return print_beer_subpath(dcel::edgeB(v));
}


vector<struct vertex*> beer::print_beer_path(struct halfedge* uv){
	vector<struct vertex*> pathB = beer::print_beer_path(dcel::edgeB(uv));
	if (dcel::origin(uv) != pathB.front())
		reverse(pathB.begin(), pathB.end());

	return pathB;
}



vector<struct vertex*> beer::print_beer_path(struct b_edge* uv){
	vector<struct vertex*> pathB = {uv->u};
	vector<struct vertex*> subPathB = beer::print_beer_subpath(uv);
	pathB.insert(pathB.end(), subPathB.begin(), subPathB.end());

	return pathB;
}

vector<struct vertex*> beer::print_beer_subpath(struct b_edge* e){
	if (dcel::beer(e->u) || dcel::beer(e->v))
		return {e->v};

	// u - w - v
	vector<struct vertex*> path;
	auto [w, beerLoc] = e->pathB;

	if (beerLoc == 0){
		struct b_edge* subpathB = beer::get_edgeB(e->u, w);
		if (subpathB->u != e->u && subpathB->v != w){
			swap(subpathB->u, subpathB->v);
			subpathB->pathB.second = !subpathB->pathB.second;
		}

		// u - w is the beer subpath
		path = print_beer_subpath(subpathB);
		path.push_back(e->v);
	}
	else{
		struct b_edge* subpathB = beer::get_edgeB(w, e->v);
		if (subpathB->u != w && subpathB->v != e->v){
			swap(subpathB->u, subpathB->v);
			subpathB->pathB.second = !subpathB->pathB.second;
		}

		// w - v is the beer subpath
		vector<struct vertex*> sub_path = beer::print_beer_subpath(subpathB);
		path = {w};
		path.insert(path.end(), sub_path.begin(), sub_path.end());
	}

	return path;
}



