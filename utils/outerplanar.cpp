#include <vector>
#include <unordered_set>
#include <utility>
#include <ctime>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../include/outerplanar.h"
#include "../include/graph/dcel.h"
#include "../include/graph/btree.h"
#include "../include/preprocess/beer.h"
#include "../include/preprocess/lca.h"


// Check if vertex v belongs to the node face
bool graph::incident_vertex2face(struct vertex* v, struct node* face){
	struct halfedge* e = dcel::edge(face);
	if (dcel::target(e) == v)
		return true;
	else if (dcel::origin(e) == v)
		return true;
	else if (dcel::target(dcel::next(e)) == v)
		return true;
	return false;;
}


// Get directed halfedge {u, v}
struct halfedge* graph::get_edge(struct vertex* u, struct vertex* v){
	for (auto e : u->adj){
		if (v == dcel::origin(e)) return dcel::twin(e);
	}

	for (auto e : v->adj){
		if (u == dcel::origin(e)) return e;
	}

	return NULL;
}


struct vertex* graph::get_vertex(int v){
	struct vertex* vertex;
	if (v == -1) vertex = dcel::origin(mroot_edge);
	else if (v == 0) vertex = dcel::target(mroot_edge);
	else vertex = dcel::apex(dcel::edge(get_lca(mdual_lca, v, v)));

	return vertex;
}




void graph::build_dual(int dual_type){
	switch(dual_type){
		case 1: 
			mroot_node = btree::generate_balance_tree(mV - 2);
			break;
		case 2:
			mroot_node = btree::generate_skewed_right(mV - 2);
			break;
		case 3:
			mroot_node = btree::generate(mV - 2);
			break;
		default:
			mroot_node = btree::generate_balance_tree(mV - 2);
			break;
	}

	mV = btree::inorder_label(mroot_node, 1) - 1 + 2;
	return;
}


void graph::triangulate_polygon(struct halfedge* root_edge, struct node* root_node){
	if (root_node == NULL) return;

	struct vertex* v = dcel::add_vertex(dcel::iData(root_node), mV, mbeer_probability);
	struct halfedge* new_root_edge;

	new_root_edge = dcel::add_edge(root_edge, v, root_node, dcel::left(root_node));
	triangulate_polygon(dcel::twin(new_root_edge), dcel::left(root_node));

	new_root_edge = dcel::split_face(new_root_edge, dcel::origin(root_edge), root_node, dcel::right(root_node));
	triangulate_polygon(dcel::twin(new_root_edge), dcel::right(root_node));

	// create adj list for vertex v
	v->adj = {dcel::next(root_edge), dcel::twin(new_root_edge)};
	return;
}


void graph::build_polygon(){
	triangulate_start = clock();
	struct vertex* u = dcel::add_vertex(-1, mV, mbeer_probability);
	struct vertex* v = dcel::add_vertex(0, mV, mbeer_probability);
	struct halfedge* h1 = new halfedge();
	struct halfedge* h2 = new halfedge();
	h1->weight = h2->weight = euclidean_dist(u, v);
	h1->beer_edge = h2->beer_edge = new b_edge(u, v);

	h1->twin = h2;
	h2->twin = h1;
	h1->target = v;
	h2->target = u;
	h1->incident_face = mroot_node;
	h2->incident_face = NULL;
	h1->incident_face->incident_edge = h1;

	h1->next = h2;
	h2->next = h1;
	h1->prev = h2;
	h2->prev = h1;
	
	mroot_edge = h1;
	triangulate_polygon(mroot_edge, mroot_node);
	triangulate_duration = (clock() - triangulate_start) / (double) CLOCKS_PER_SEC;

	u->adj = {dcel::twin(mroot_edge), dcel::prev(mroot_edge)};
	v->adj = {mroot_edge, dcel::twin(dcel::next(mroot_edge))};
	return;
}





void graph::preprocess_chain_spath(struct vertex* apex, struct halfedge* edge){
	struct halfedge* traverse = edge;

	int index_chain = 0;
	double d2cw = 0;

	do {
		// Lemma 4
		apex->v_chain.push_back(make_pair(dcel::origin(traverse), d2cw));
		d2cw += dcel::weight(dcel::prev(traverse));
		traverse->index = index_chain++;

		traverse = dcel::twin(dcel::next(traverse));
	} while(traverse != edge);

	// Get apex's dual endpoint cw 
	apex->cw = dcel::face(traverse);
	// Get apex's dual endpoint ccw
	apex->ccw = dcel::face(dcel::twin(dcel::prev(dcel::twin(traverse))));
	apex->h = get_lca(mdual_lca, dcel::cw(apex), dcel::ccw(apex));

	return;
}


void graph::preprocess_chain_bpath(struct vertex* apex, struct halfedge* edge){
	struct halfedge* traverse = edge;

	vector<double> array_Av;
	array_Av.reserve(apex->v_chain.size() - 1);

	do {
		array_Av.push_back(beer::weightB(dcel::prev(traverse)) - dcel::weight(dcel::prev(traverse)));

		traverse = dcel::twin(dcel::next(traverse));
	} while(dcel::twin(dcel::next(traverse)) != edge);

	struct node* cnode = btree::build_cartesian_tree(array_Av);
	struct lca* lca_v_chain = new lca(array_Av.size());
	precompute_lca(lca_v_chain, cnode);
	apex->lca_Av_chain = lca_v_chain;
	
	return;
}


void graph::preprocess_graph(){
	mdual_lca = new lca(mV - 2);
	lca_start = clock();
	precompute_lca(mdual_lca, mroot_node);
    lca_duration = (clock() - lca_start) / (double) CLOCKS_PER_SEC;

	beer_start = clock();
	// Beer Distance - post-order traversal of D(G)
	beer::computeBeerDistNotRoot(mroot_node);
	// Beer Distance - pre-order traversal of D(G)
	beer::computeBeerDistRoot(mroot_node);
	beer_duration = (clock() - beer_start) / (double) CLOCKS_PER_SEC;


	struct halfedge* traverse_e = dcel::twin(mroot_edge);
	struct vertex* curr = dcel::origin(traverse_e);

	chain_start = clock();
	// Traversing each vertex of outerplanar graph
	do {
		struct halfedge* degree_e = dcel::twin(traverse_e);

		// Lemma 4
		graph::preprocess_chain_spath(curr, degree_e);
		// Lemma 
		graph::preprocess_chain_bpath(curr, degree_e);

		traverse_e = dcel::next(traverse_e);
		curr = dcel::origin(traverse_e);
	} while(curr != dcel::target(mroot_edge));
	chain_duration = (clock() - chain_start) / (double) CLOCKS_PER_SEC;

	return;
}





struct node* graph::second_node_query(struct node* u, struct node* v){
	struct halfedge* parent_edge = dcel::edge(u);
	struct node* u_parent = dcel::face(dcel::twin(parent_edge));

	if (dcel::left(u) && get_lca(mdual_lca, v, dcel::left(u)) == dcel::left(u))
		return dcel::left(u);
	else if (dcel::right(u) && get_lca(mdual_lca, v, dcel::right(u)) == dcel::right(u))
		return dcel::right(u);
	else if (u == v)
		return v;
	return u_parent;
}


struct node* graph::last_node_query(struct vertex* vertex, struct node* u, struct node* v){		
	if (u == v) return v;

	else if (graph::incident_vertex2face(vertex, v))
		return v;

	else if (get_lca(mdual_lca, u, v) == v) 
		return dcel::h(vertex);

	else if (get_lca(mdual_lca, u, v) == u){
		struct node* u_child;
		if (dcel::left(u) != NULL && get_lca(mdual_lca, dcel::left(u), v) == dcel::left(u))
			u_child = dcel::left(u);
		else if (dcel::right(u) != NULL && get_lca(mdual_lca, dcel::right(u), v) == dcel::right(u))
			u_child = dcel::right(u);

		struct node* w1 = get_lca(mdual_lca, v, dcel::ccw(vertex));
		struct node* w2 = get_lca(mdual_lca, v, dcel::cw(vertex));

		if (get_lca(mdual_lca, w1, u_child) == u_child)
			return w1;
		else if (get_lca(mdual_lca, w2, u_child) == u_child)
			return w2;
		else return u;
	}

	else {
		struct node* x = get_lca(mdual_lca, u, v);
		struct node* y = dcel::left(x);
		struct node* z;
		if (y != get_lca(mdual_lca, u, y)){
			y = dcel::right(x);
			z = dcel::left(x);
		}
		else z = dcel::right(x);

		if (get_lca(mdual_lca, dcel::h(vertex), y) == y)
			return dcel::h(vertex);
		else if (x != dcel::h(vertex))
			return x;
		else {
			if (dcel::h(vertex) == dcel::ccw(vertex))
				return x;
			else if (dcel::h(vertex) == dcel::cw(vertex))
				return x;
			else {
				if (get_lca(mdual_lca, dcel::ccw(vertex), z) == z)
					return get_lca(mdual_lca, v, dcel::ccw(vertex));
				else if (get_lca(mdual_lca, dcel::cw(vertex), z) == z)
					return get_lca(mdual_lca, v, dcel::cw(vertex));
			}
		}
	}

	return NULL;
}