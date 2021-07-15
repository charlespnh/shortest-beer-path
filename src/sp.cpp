#include <vector>
#include <limits>
#include <iostream>
#include <cstdlib>

using namespace std;

#include "sp.h"
#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"


void print_sp_dag(Vertex* v){
 	if (v->p == NULL) {
 		cout << "shortest path: " << v->data << " ";
 		return;
 	}
 	print_sp_dag(v->p->src);
 
 	Vertex* apex = v->p->cross_apex;
 	// cross apex (not on v_chain)
 	if (v->p->cw == MAX){
 		if (apex != v->p->src && apex != v->p->dest)
 			cout << apex->data << " ";
 		cout << v->data << " "; 
 	}
 	// walk along apex's chain
 	else{
 		Vertex* u;
 		int index = v->p->cw;
 		if (index < 0){		// ccw
 			index = abs(index);
 			do {
 				u = apex->chain->v_chain[--index].first->prev->target;
 				cout << u->data << " ";
 			} while(u != v->p->dest);
 		}
 		else {				// cw
 			do {
 				u = apex->chain->v_chain[++index].first->prev->target;
 				cout << u->data << " ";
 			} while(u != v->p->dest);
 		}
 	}
}

double shortest_path(struct Vertex* src, struct Vertex* dest, struct lca* your_lca){
	// cout << "Src " << src->data << " and sink " << dest->data << "\n";
	DAG dag = build_dag(src, dest, your_lca);
	double dist = shortest_path_dag(dag);
	// print_sp_dag(dest);		cout << "\n\n";
	return dist;
}

double shortest_path_dag(DAG& dag){
	double dist1 = dag[0]->spes[0]->sp_weight;
	double dist2 = dag[0]->spes[1]->sp_weight;
	double dp[2];
	dag[1]->col_vtx->p = dag[0]->spes[0];
	dag[2]->col_vtx->p = dag[0]->spes[1];
	for (int i = 3, j = 0; i < dag.size(); i++, j++){
		int prev_col = dag[i]->col_vtx->col - 1;
		if (dist1 + dag[2*prev_col-1]->spes[j]->sp_weight < dist2 + dag[2*prev_col]->spes[j]->sp_weight){
			dp[j] = dist1 + dag[2*prev_col-1]->spes[j]->sp_weight;
			dag[i]->col_vtx->p = dag[2*prev_col-1]->spes[j];
		}
		else{
			dp[j] = dist2 + dag[2*prev_col]->spes[j]->sp_weight;
			dag[i]->col_vtx->p = dag[2*prev_col]->spes[j];
		}
		if (j == 1){
			j = -1;
			dist1 = dp[0], dist2 = dp[1];
		}
	}

	/* for (auto v : dag){
		printf("Vertex %d | col %d | p (%d, %d)\n", v->col_vtx->data, v->col_vtx->col, 
													(v->col_vtx->p) ? v->col_vtx->p->src->data : INF,
													(v->col_vtx->p) ? v->col_vtx->p->dest->data : INF);
		for (auto e : v->spes){
			printf("Edge (%d, %d) | w %f | on chain? %d | index %d\n", e->src->data, e->dest->data, e->sp_weight, e->cross_apex->data, e->cw);
		}
		cout << "\n";
	} */
	return dp[0];
}


DAGEdges compute_dag_edges_on_chain(Vertex* col_vtx, HalfEdge* next_col_edge){
	// Computing spes[0] and spes[1]
	SPEdge* sp_e1 = new SPEdge(col_vtx, next_col_edge->target, 0);
	SPEdge* sp_e2 = new SPEdge(col_vtx, next_col_edge->prev->target, 0);
	sp_e1->sp_weight = next_col_edge->next->weight;
	sp_e2->sp_weight = next_col_edge->prev->weight;
	sp_e1->cross_apex = sp_e2->cross_apex = col_vtx;

	return {sp_e1, sp_e2};
}


DAGEdges compute_dag_edges_off_chain(Vertex* col_vtx, HalfEdge* col_edge, HalfEdge* next_col_edge){
	SPEdge* sp_e1 = new SPEdge(col_vtx, next_col_edge->target, 0);
	SPEdge* sp_e2 = new SPEdge(col_vtx, next_col_edge->prev->target, 0);
	int index1 = col_edge->prev->apex.second;
	int index2 = next_col_edge->apex.second;
	double path_weight_on_chain, path_weight_cross_apex;

	// Computing spes[2]
	path_weight_on_chain = next_col_edge->apex.first->chain->v_chain[index1].second - next_col_edge->apex.first->chain->v_chain[index2].second; 
	path_weight_cross_apex = col_edge->weight + next_col_edge->next->weight;
	if (abs(path_weight_on_chain) < path_weight_cross_apex){
		sp_e1->sp_weight = abs(path_weight_on_chain);
		sp_e1->cw = (path_weight_on_chain < 0) ? index1 : -index1;
	}
	else sp_e1->sp_weight = path_weight_cross_apex;

	// Computing spes[3]
	path_weight_on_chain = next_col_edge->apex.first->chain->v_chain[index1].second - next_col_edge->apex.first->chain->v_chain[index2 + 1].second;
	path_weight_cross_apex = col_edge->weight + next_col_edge->prev->weight;
	if (abs(path_weight_on_chain) < path_weight_cross_apex){
		sp_e2->sp_weight = abs(path_weight_on_chain);
		sp_e2->cw = (path_weight_on_chain < 0) ? index1 : -index1;
	}
	else sp_e2->sp_weight = path_weight_cross_apex;

	sp_e1->cross_apex = sp_e2->cross_apex = next_col_edge->apex.first;
	return {sp_e1, sp_e2};
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
		SPEdge* sp_e1 = new SPEdge(apex_u, sink, 0);
		SPEdge* sp_e2 = new SPEdge(apex_v, sink, 0);
		if (next_dag->prev->target == sink){
			next_dag = next_dag->next;
			sp_e2 = sp_edges2[1];
		}
		else sp_e2 = sp_edges2[0];
		sp_e1->sp_weight = next_dag->next->weight;
		sp_e1->cross_apex = apex_u;
		sp_edges1 = {sp_e1};
		sp_edges2 = {sp_e2};
	}
	
	dag_graph.push_back(new DAGVertex(apex_u, sp_edges1));
	dag_graph.push_back(new DAGVertex(apex_v, sp_edges2));
	if (swapped){
		swap(dag_graph.rbegin()[0], dag_graph.rbegin()[1]);
	}
} 


DAG build_dag(Vertex* src, Vertex* sink, lca* your_lca){
	DAG dag_graph;
	DAGEdges spes;
	Node *u = src->out_edge->incident_face;
	Node *v = sink->out_edge->incident_face;
	Vertex *col_vtx1 = src, *col_vtx2 = src;
	HalfEdge *prev_col_edge = NULL, *col_edge = NULL;
	int ith_col = 0;
	do {
		// printf("Face v %p  %d\n", v, v->data);
		// printf("Face u %p  %d\n", u, u->data);
		// Answer last label query and second node query
		Node* lnq = last_node_query(col_vtx1, u, v, your_lca);
		if (col_vtx1 != src && check_incident_vertex(lnq, col_vtx2))
			lnq = last_node_query(col_vtx2, u, v, your_lca);
		// printf("Queried last face: %p  %d\n", lnq, lnq->data);
		Node* snq = second_node_query(lnq, v, your_lca);
		// printf("Queried second face: %p  %d\n", snq, snq->data);
		u = snq;

		// Get the next DAG column edge
		if (lnq->incident_edge->twin->incident_face == snq)
			col_edge = lnq->incident_edge;
		else col_edge = snq->incident_edge->twin;

		// cout << "Vertex " << col_edge->target->data << " and Vertex " << col_edge->prev->target->data << "\n\n";

		// Building DAG column from above vertices
		if (prev_col_edge == NULL){
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


Node* second_node_query(Node* u, Node* v, lca* your_lca){
	HalfEdge* parent_edge = u->incident_edge;
	Node* u_parent = parent_edge->twin->incident_face;

	if (u->left && get_lca(your_lca, v, u->left) == u->left)
		return u->left;
	else if (u->right && get_lca(your_lca, v, u->right) == u->right)
		return u->right;
	else if (u == v)
		return v;
	return u_parent;
}


Node* last_node_query(Vertex* vertex, Node* u, Node* v , lca* your_lca){
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