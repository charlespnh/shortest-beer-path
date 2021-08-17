#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <limits>
#include <cstdlib>
#include <iostream>

using namespace std;

#include "dag.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"


struct SPEdge* getEdge(struct DAGVertex* u, struct DAGVertex* v){
	if (u->spes[0].first->dest == v->col_vtx)
		return u->spes[0].first;
	else if (u->spes[1].first->dest == v->col_vtx)
		return u->spes[1].first;
	return NULL;
}

// index - position of vertex u
// subpath (u -> v) - travel along the apex's chain with index until reached vertex v
string print_sp_on_chain(struct Vertex* v, struct Vertex* apex, int index){
	string path = "";
	struct Vertex* u;
	if (index < 0){		// ccw
		index = abs(index);
		u = apex->chain->v_chain[index].first->prev->target;
		while(u != v) {
			u = apex->chain->v_chain[--index].first->prev->target;
			cout << u->data << " ";
			path += to_string(u->data) + " ";
		}
	}
	else {				// cw
		u = apex->chain->v_chain[index].first->prev->target;
		while(u != v) {
			u = apex->chain->v_chain[++index].first->prev->target;
			cout << u->data << " ";
			path += to_string(u->data) + " ";
		}
	}
	return path;
}


void print_sp_dag(Vertex* v){
 	if (v->p == NULL) {
 		cout << "shortest path: " << v->data << " ";
 		return;
 	}
 	print_sp_dag(v->p->src);
 
 	struct Vertex* apex = v->p->cross_apex;
 	// cross apex (not on v_chain)
 	if (v->p->cw == MAX){
 		if (apex != v->p->src && apex != v->p->dest)
 			cout << apex->data << " ";
 		cout << v->data << " "; 
 	}
 	// walk along apex's chain
 	else 
 		print_sp_on_chain(v, apex, v->p->cw);
}

void print_sbp_dag(DAGVertex* v){
	auto [w, subPath, beerLoc] = v->b;
	if (w == NULL){
		cout << "shortest beer path: " << v->col_vtx->data << " ";
		return;
	}
	print_sbp_dag(w);

	// just a normal shortest subpath
	if (subPath == NULL){
		struct SPEdge* sub_path = getEdge(w, v);
		struct Vertex* apex = sub_path->cross_apex;
		int index = sub_path->cw;
		if (index == MAX){
 			if (apex != sub_path->src && apex != sub_path->dest)
 				cout << apex->data << " ";
 			cout << v->col_vtx->data << " "; 
 		}
 		else
	 		print_sp_on_chain(v->col_vtx, apex, index);
	}

	// shortest beer subpath
	else{
		auto [c, subPathB, i] = subPath->pathB;
		// "an edge"
		if (i == MAX)
			print_beer_path(subPathB);

		// on chain
		else if (subPathB->apex.first == c){
			cout << "\tDEBUGGING " << w->col_vtx->data << " and " << c->data << " and " << i << "\n";
			print_sp_on_chain(w->col_vtx, c, 0 - i);
			print_beer_path(c->chain->v_chain[abs(i)].first->prev);
			print_sp_on_chain(v->col_vtx, c, i - 1);
		}

		// cross apex
		else{
			// 1st path is beer subpath
			if (i == 0){
				print_beer_path(subPathB);
				cout << v->col_vtx->data;
			}

			// 2nd path is beer subpath
			else{
				cout << w->col_vtx->data;
				print_beer_path(subPathB);
			}
		}
	}
}

double shortest_path(struct Vertex* src, struct Vertex* dest, struct lca* your_lca){
	// cout << "Src " << src->data << " and sink " << dest->data << "\n";
	DAG dag = build_dag(src, dest, your_lca);
	double dist = shortest_path_dag(dag);
	// print_sp_dag(dest);		cout << "\n\n";
	// double distB = shortest_beer_path_dag(dag);
	// print_sbp_dag(dest);		cout << "\n\n";
	return dist;
}

double shortest_path_dag(DAG& dag){
	dag[1]->dist = dag[0]->spes[0].first->sp_weight;
	dag[1]->col_vtx->p = dag[0]->spes[0].first;
	dag[2]->dist = dag[0]->spes[1].first->sp_weight;
	dag[2]->col_vtx->p = dag[0]->spes[1].first;

	int j = 0;
	for (int i = 3; i < dag.size(); i++){
		int prev_col = dag[i]->col_vtx->col - 1;
		dag[i]->dist = min(dag[2*prev_col-1]->dist + dag[2*prev_col-1]->spes[j].first->sp_weight,
						   dag[2*prev_col]->dist + dag[2*prev_col]->spes[j].first->sp_weight);
		if (dag[i]->dist == dag[2*prev_col-1]->dist + dag[2*prev_col-1]->spes[j].first->sp_weight)
			dag[i]->col_vtx->p = dag[2*prev_col-1]->spes[j].first;
		else dag[i]->col_vtx->p = dag[2*prev_col]->spes[j].first;

		j = !j;
	}

	return 0.0;
}

double shortest_beer_path_dag(DAG& dag){
	dag[0]->b = make_tuple((struct DAGVertex*) NULL, (struct SPBEdge*) NULL, 0);
	dag[1]->distB = dag[0]->dist + dag[0]->spes[0].second->sp_bweight;
	dag[1]->b = make_tuple(dag[0], (struct SPBEdge*) NULL, 0);
	dag[2]->distB = dag[0]->dist + dag[0]->spes[1].second->sp_bweight;
	dag[2]->b = make_tuple(dag[0], (struct SPBEdge*) NULL, 0);

	int j = 0;
	for (int i = 3; i < dag.size(); i++){
		int prev_col = dag[i]->col_vtx->col - 1;
		dag[i]->distB = min(min(dag[2*prev_col-1]->distB + dag[2*prev_col-1]->spes[j].first->sp_weight, dag[2*prev_col-1]->dist + dag[2*prev_col-1]->spes[j].second->sp_bweight),
						    min(dag[2*prev_col]->distB + dag[2*prev_col]->spes[j].first->sp_weight, dag[2*prev_col]->dist + dag[2*prev_col]->spes[j].second->sp_bweight));

		if (dag[i]->distB == dag[2*prev_col-1]->distB + dag[2*prev_col-1]->spes[j].first->sp_weight)
			dag[i]->b = make_tuple(dag[2*prev_col-1], (struct SPBEdge*) NULL, 0);
		else if (dag[i]->distB == dag[2*prev_col]->distB + dag[2*prev_col]->spes[j].first->sp_weight)
			dag[i]->b = make_tuple(dag[2*prev_col], (struct SPBEdge*) NULL, 0);
		else if (dag[i]->distB == dag[2*prev_col-1]->dist + dag[2*prev_col-1]->spes[j].second->sp_bweight)
			dag[i]->b = make_tuple(dag[2*prev_col-1], dag[2*prev_col-1]->spes[j].second, 1);
		else dag[i]->b = make_tuple(dag[2*prev_col], dag[2*prev_col]->spes[j].second, 1);

		j = !j;
	}

	printf("Src %d -> sink %d\n", dag[0]->col_vtx->data, dag.back()->col_vtx->data);
	print_sp_dag(dag.back()->col_vtx);	cout << "\n";
	print_sbp_dag(dag.back()); cout << "\n\n";
	for (auto v : dag){
		printf("DAGVertex %d | col %d | p (%d, %d) | ", v->col_vtx->data, v->col_vtx->col, 
													(v->col_vtx->p) ? v->col_vtx->p->src->data : MAX,
													(v->col_vtx->p) ? v->col_vtx->p->dest->data : MAX);
		printf("b (%d, (%d, %d), %d)\n", (get<0>(v->b)) ? get<0>(v->b)->col_vtx->data : MAX, 
										 (get<1>(v->b)) ? get<1>(v->b)->src->data : MAX, (get<1>(v->b)) ? get<1>(v->b)->dest->data : MAX, get<2>(v->b));
		for (auto e : v->spes){
			printf("SPEdge (%d, %d) | w %f | on chain vtx %d | index %d\n", e.first->src->data, e.first->dest->data, 
																	   e.first->sp_weight, e.first->cross_apex->data, e.first->cw);
			printf("SPBEdge (%d, %d) | w %f | on chain vtx %d | index %d | ", e.second->src->data, e.second->dest->data,
																		 e.second->sp_bweight, get<0>(e.second->pathB)->data, get<2>(e.second->pathB));
			print_beer_path(get<1>(e.second->pathB));	cout << "\n";
		}
		cout << "\n";
	}
	return dag.back()->distB;
}


DAGEdges compute_dag_edges_on_chain(Vertex* col_vtx, HalfEdge* next_col_edge){
	// DAG shortest path edges - computing spes[0] and spes[1]
	struct SPEdge* sp_e1 = new SPEdge(col_vtx, next_col_edge->target);
	struct SPEdge* sp_e2 = new SPEdge(col_vtx, next_col_edge->prev->target);
	sp_e1->sp_weight = next_col_edge->next->weight;
	sp_e2->sp_weight = next_col_edge->prev->weight;
	sp_e1->cross_apex = sp_e2->cross_apex = col_vtx;

	// DAG shortest path beer edges
	struct SPBEdge* sp_be1 = new SPBEdge(col_vtx, next_col_edge->target);
	struct SPBEdge* sp_be2 = new SPBEdge(col_vtx, next_col_edge->prev->target);
	sp_be1->sp_bweight = next_col_edge->next->beer_edge->distB;
	sp_be2->sp_bweight = next_col_edge->prev->beer_edge->distB;
	// sp_be1->cross_apex = sp_be2->cross_apex = col_vtx;
	sp_be1->pathB = make_tuple(col_vtx, next_col_edge->next, MAX);
	sp_be2->pathB = make_tuple(col_vtx, next_col_edge->prev, MAX);

	return {make_pair(sp_e1, sp_be1), make_pair(sp_e2, sp_be2)};
}


DAGEdges compute_dag_edges_off_chain(Vertex* col_vtx, HalfEdge* col_edge, HalfEdge* next_col_edge){
	struct SPEdge* sp_e1 = new SPEdge(col_vtx, next_col_edge->target);
	struct SPEdge* sp_e2 = new SPEdge(col_vtx, next_col_edge->prev->target);
	struct SPBEdge* sp_be1 = new SPBEdge(col_vtx, next_col_edge->target);
	struct SPBEdge* sp_be2 = new SPBEdge(col_vtx, next_col_edge->prev->target);
	int index1 = col_edge->prev->apex.second;		// "src" - (u_i <- u_i+1) - vtx u_i on chain at index i = index1
	int index2 = next_col_edge->apex.second;		// "dest" - (a_i <- a_i+1) - vtx a_i on chain at index i = index2
	double path_weight_on_chain, path_weight_cross_apex;
	struct Chain* chain = next_col_edge->apex.first->chain;

	/*DAG shortest path edge - computing spes[2]*/
	path_weight_on_chain = chain->v_chain[index1].second - chain->v_chain[index2].second; 
	path_weight_cross_apex = col_edge->weight + next_col_edge->next->weight;
	sp_e1->sp_weight = min(abs(path_weight_on_chain), path_weight_cross_apex);
	if (sp_e1->sp_weight == abs(path_weight_on_chain))
		sp_e1->cw = (index1 < index2) ? index1 : -index1;

	/*DAG shortest path beer edge*/
	struct lca* lca_chain = chain->lca_chain_Av;
	struct Node* index;
	// ccw on chain
	if (sp_e1->cw < 0) 
		index = get_lca(lca_chain, index1 - 1, index2);
	// cw on chain
	else index = get_lca(lca_chain, index1, index2 - 1);
	// index = get_lca(lca_chain, index1 - 1, index2);
	path_weight_on_chain += index->dData;
	path_weight_cross_apex = min(col_edge->beer_edge->distB + next_col_edge->next->weight,
								 col_edge->weight + next_col_edge->next->beer_edge->distB);
	sp_be1->sp_bweight = min(abs(path_weight_on_chain), path_weight_cross_apex);
	sp_be1->pathB = make_tuple(next_col_edge->apex.first, (struct HalfEdge*) NULL, MAX);
	if (sp_be1->sp_bweight == abs(path_weight_on_chain)){
		get<1>(sp_be1->pathB) = chain->v_chain[index->iData].first->prev;		// get the beer edge "on chain" path that SPB took
		get<2>(sp_be1->pathB) = (index1 < index2) ? index->iData : -index->iData;
	}	
	else{
		if (sp_be1->sp_bweight == col_edge->beer_edge->distB + next_col_edge->next->weight){
			get<1>(sp_be1->pathB) = col_edge;		// get the beer edge "cross apex" path that SPB took
			get<2>(sp_be1->pathB) = 0;				// 1st edge "cross apex" is the beer path
		}
		else{
			get<1>(sp_be1->pathB) = next_col_edge->next;
			get<2>(sp_be1->pathB) = 1;				// 2nd edge "cross apex" is the beer path
		}
	}
	
	/*DAG shortest path edge - computing spes[3]*/
	path_weight_on_chain = chain->v_chain[index1].second - chain->v_chain[index2 + 1].second;
	path_weight_cross_apex = col_edge->weight + next_col_edge->prev->weight;
	sp_e2->sp_weight = min(abs(path_weight_on_chain), path_weight_cross_apex);
	if (sp_e2->sp_weight == abs(path_weight_on_chain))
		sp_e2->cw = (index1 < index2 + 1) ? index1 : -index1;

	/*DAG shortest path beer edge*/
	if (sp_e2->cw < 0)
		index = get_lca(lca_chain, index1 - 1, index2 + 1);
	else index = get_lca(lca_chain, index1, index2);
	// index = get_lca(lca_chain, index1 - 1, index2);
	path_weight_on_chain += index->dData;
	path_weight_cross_apex = min(col_edge->beer_edge->distB + next_col_edge->next->weight,
								 col_edge->weight + next_col_edge->next->beer_edge->distB);
	sp_be2->sp_bweight = min(abs(path_weight_on_chain), path_weight_cross_apex);
	sp_be2->pathB = make_tuple(next_col_edge->apex.first, (struct HalfEdge*) NULL, MAX);
	if (sp_be2->sp_bweight == abs(path_weight_on_chain)){
		get<1>(sp_be2->pathB) = chain->v_chain[index->iData].first->prev;
		get<2>(sp_be2->pathB) = (index1 < index2) ? index->iData : -index->iData;
	}	
	else{
		if (sp_be2->sp_bweight == col_edge->beer_edge->distB + next_col_edge->next->weight){
			get<1>(sp_be2->pathB) = col_edge;
			get<2>(sp_be2->pathB) = 0;
		}
		else{
			get<1>(sp_be2->pathB) = next_col_edge->next;
			get<2>(sp_be2->pathB) = 1;
		}
	}

	sp_e1->cross_apex = sp_e2->cross_apex = next_col_edge->apex.first;
	// sp_be1->cross_apex = sp_be2->cross_apex = next_col_edge->apex.first;
	return {make_pair(sp_e1, sp_be1), make_pair(sp_e2, sp_be2)};
}


// HalfEdge dag is the one to be put into DAG column!
void compute_dag_edges(DAG& dag_graph, HalfEdge* dag, HalfEdge* next_dag, Vertex* sink = NULL){
	Vertex* apex_u = dag->target;
	Vertex* apex_v = dag->prev->target;
	// next_dag (now) belongs to apex_u's chain
	bool swapped = false;
	if (next_dag->apex.first != apex_u){
		swap(apex_u, apex_v);
		dag = dag->twin;
		swapped = true;
	}
	
	DAGEdges sp_edges1 = compute_dag_edges_on_chain(apex_u, next_dag);
	DAGEdges sp_edges2 = compute_dag_edges_off_chain(apex_v, dag, next_dag);
	if (sink){
		SPEdge* sp_e1 = new SPEdge(apex_u, sink);
		SPEdge* sp_e2 = new SPEdge(apex_v, sink);
		SPBEdge* sp_be1 = new SPBEdge(apex_u, sink);
		SPBEdge* sp_be2 = new SPBEdge(apex_v, sink);	
		if (next_dag->prev->target == sink){	
			next_dag = next_dag->next;
			sp_e2 = sp_edges2[1].first;
			sp_be2 = sp_edges2[1].second;
		}
		else{
			sp_e2 = sp_edges2[0].first;
			sp_be2 = sp_edges2[0].second;
		}
		sp_e1->sp_weight = next_dag->next->weight;
		sp_be1->sp_bweight = next_dag->next->beer_edge->distB;
		sp_e1->cross_apex = apex_u;
		sp_be1->pathB = make_tuple(apex_u, next_dag->next, MAX);
		sp_edges1 = {make_pair(sp_e1, sp_be1)};
		sp_edges2 = {make_pair(sp_e2, sp_be2)};
	}
	
	dag_graph.push_back(new DAGVertex(apex_u, sp_edges1));
	dag_graph.push_back(new DAGVertex(apex_v, sp_edges2));
	if (swapped)
		swap(dag_graph.rbegin()[0], dag_graph.rbegin()[1]);
} 


DAG build_dag(struct Vertex* src, struct Vertex* sink, struct lca* your_lca){
	DAG dag_graph;
	DAGEdges spes;
	struct Node *u = src->chain->v_chain[0].first->incident_face;
	struct Node *v = sink->chain->v_chain[0].first->incident_face;
	struct Vertex *col_vtx1 = src, *col_vtx2 = src;
	struct HalfEdge *prev_col_edge = NULL, *col_edge = NULL;
	int ith_col = 0;

	do {
		// Answer last label query and second node query
		struct Node* lnq = last_node_query(col_vtx1, u, v, your_lca);
		if (col_vtx1 != col_vtx2 && check_incident_vertex(lnq, col_vtx2))
			lnq = last_node_query(col_vtx2, u, v, your_lca);
		struct Node* snq = second_node_query(lnq, v, your_lca);
		u = snq;

		// Get the next DAG column edge
		if (lnq->incident_edge->twin->incident_face == snq)
			col_edge = lnq->incident_edge;
		else col_edge = snq->incident_edge->twin;

		// Building DAG column from above vertices
		if (ith_col == 0){
			src->col = ith_col++;
			spes = compute_dag_edges_on_chain(src, col_edge);
			dag_graph.push_back(new DAGVertex(src, spes));
		}
		else compute_dag_edges(dag_graph, prev_col_edge, col_edge);
		
		col_vtx1 = col_edge->target;
		col_vtx2 = col_edge->prev->target;
		col_vtx1->col = col_vtx2->col = ith_col++;
		prev_col_edge = col_edge;
	} while(!(col_edge = check_on_chain(col_vtx1, sink, your_lca)) && !(col_edge = check_on_chain(col_vtx2, sink, your_lca)));
	
	sink->col = ith_col;
	compute_dag_edges(dag_graph, prev_col_edge, col_edge, sink);
	dag_graph.push_back(new DAGVertex(sink));
	return dag_graph;
}


struct Node* second_node_query(Node* u, Node* v, lca* your_lca){
	struct HalfEdge* parent_edge = u->incident_edge;
	struct Node* u_parent = parent_edge->twin->incident_face;

	if (u->left && get_lca(your_lca, v, u->left) == u->left)
		return u->left;
	else if (u->right && get_lca(your_lca, v, u->right) == u->right)
		return u->right;
	else if (u == v)
		return v;
	return u_parent;
}


struct Node* last_node_query(Vertex* vertex, Node* u, Node* v , lca* your_lca){
	if (u == v) return v;

	else if (check_incident_vertex(v, vertex))
		return v;

	else if (get_lca(your_lca, u, v) == v) 
		return vertex->chain->p_chain_h;

	else if (get_lca(your_lca, u, v) == u){
		Node* u_child;
		if (u->left != NULL && get_lca(your_lca, u->left, v) == u->left)
			u_child = u->left;
		else if (u->right != NULL && get_lca(your_lca, u->right, v) == u->right)
			u_child = u->right;

		Node* w1 = get_lca(your_lca, v, vertex->chain->p_chain_ccw);
		Node* w2 = get_lca(your_lca, v, vertex->chain->p_chain_cw);

		if (get_lca(your_lca, w1, u_child) == u_child)
			return w1;
		else if (get_lca(your_lca, w2, u_child) == u_child)
			return w2;
		else return u;
	}

	else {
		Node* x = get_lca(your_lca, u, v);
		Node* y = x->left;
		Node* z;
		if (y != get_lca(your_lca, u, y)){
			y = x->right;
			z = x->left;
		}
		else z = x->right;

		if (get_lca(your_lca, vertex->chain->p_chain_h, y) == y)
			return vertex->chain->p_chain_h;
		else if (x != vertex->chain->p_chain_h)
			return x;
		else {
			if (vertex->chain->p_chain_h == vertex->chain->p_chain_ccw)
				return x;
			else if (vertex->chain->p_chain_h == vertex->chain->p_chain_cw)
				return x;
			else {
				if (get_lca(your_lca, vertex->chain->p_chain_ccw, z) == z)
					return get_lca(your_lca, v, vertex->chain->p_chain_ccw);
				else if (get_lca(your_lca, vertex->chain->p_chain_cw, z) == z)
					return get_lca(your_lca, v, vertex->chain->p_chain_cw);
			}
		}
	}

	return NULL;
}