#include <algorithm>
#include <vector>
#include <string>
#include <utility>
#include <limits>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../include/dag.h"
#include "../include/outerplanar.h"
#include "../include/graph/dcel.h"
#include "../include/preprocess/beer.h"
#include "../include/preprocess/lca.h"
#include "../include/utils.h"


/*################################## get methods ###################################################*/

struct d_edge* dag::getDEdge(struct vertex* u, struct vertex* v){
	for (auto e : u->dag_edges)
		if (e.first->dest == v) return e.first;
	return NULL;
}


struct d_edge* dag::getDBEdge(struct vertex* u, struct vertex* v){
	for (auto e : u->dag_edges)
		if (e.second->dest == v) return e.second;
	return NULL;
}


double dag::weight(struct vertex* u, struct vertex* v){
	struct d_edge* uv = dag::getDEdge(u, v);
	if (uv) return uv->weight;
	return INF;
}


double dag::weightB(struct vertex* u, struct vertex* v){
	struct d_edge* uv = dag::getDBEdge(u, v);
	if (uv) return uv->weight;
	return INF;
}


pair<struct vertex*, int> dag::omega(struct d_edge* e){
	return e->subpath;
}



/*############################### print shortest path methods ######################################*/

// subpath v1 to v2 - travel along apex's chain
vector<struct vertex*> print_subpath_on_chain(struct vertex* apex, struct vertex* v1, struct vertex* v2){
	vector<struct vertex*> path;
	struct vertex* u = v1;
	int index1 = dcel::apex_index(graph::get_edge(v1, apex));
	int index2 = dcel::apex_index(graph::get_edge(v2, apex));

	if (index1 < index2){		// cw
		while(u != v2){
			u = dcel::neighbour(apex, ++index1);
			path.push_back(u);
		}
	}
	else {						// ccw
		while(u != v2){
			u = dcel::neighbour(apex, --index1);
			path.push_back(u);
		}
	}

	return path;
}


vector<struct vertex*> dag::print_subpath_dag(struct vertex* pred, struct vertex* v){
	vector<struct vertex*> path;
 	struct d_edge* d_edge = dag::getDEdge(pred, v);
 	auto [apex, direction] = d_edge->subpath;
 	
 	// DAG edge is an edge in graph G
 	if (apex == NULL) 
 		path = {v};
 	// 
 	else {
 		// walk cross apex
 		if (direction == 0)
 			path = {apex, v};
 		// walk cw along apex v_chain
 		else
 			path = print_subpath_on_chain(apex, pred, v);
 	}

 	return path;
}


vector<struct vertex*> dag::print_spsp_dag(struct vertex* v){
	vector<struct vertex*> path;

 	if (dcel::pred(v) == NULL){
 		path = {v};
 		return path;
 	}
 	
 	path = dag::print_spsp_dag(dcel::pred(v));
 	vector<struct vertex*> subpath = dag::print_subpath_dag(dcel::pred(v), v);
 	path.insert(path.end(), subpath.begin(), subpath.end());
 	return path;
}



vector<struct vertex*> dag::print_subpathB_dag(struct vertex* v){
	vector<struct vertex*> pathB, subpathB;
	auto [w, beerLoc] = dcel::predB(v);
	if (w == NULL) return pathB;

	// just a normal shortest subpath
	if (beerLoc == 0){
		pathB = dag::print_spsbp_dag(w);
		subpathB = dag::print_subpath_dag(w, v);
	}

	// shortest beer subpath
	else{
		pathB = dag::print_spsp_dag(w);
		auto [b, direction] = dag::omega(dag::getDBEdge(w, v));

		// "an edge" in G
		if (b == NULL)
			subpathB = beer::print_beer_path(graph::get_edge(w, v));

		// on chain apex b
		else if (direction >= 0){
			vector<struct vertex*> beer_edge;
			struct vertex* neigh1 = dcel::neighbour(b, direction);
			struct vertex* neigh2 = dcel::neighbour(b, direction + 1);
			// if ccw
			if (dcel::apex_index(graph::get_edge(w, b)) > dcel::apex_index(graph::get_edge(neigh1, b)))
				// now cw
				swap(neigh1, neigh2);

			subpathB = print_subpath_on_chain(b, w, neigh1);
			beer_edge = beer::print_beer_path(graph::get_edge(neigh1, neigh2));

			subpathB.insert(subpathB.end(), beer_edge.begin(), beer_edge.end());
			pathB.insert(pathB.end(), subpathB.begin(), subpathB.end());

			subpathB = print_subpath_on_chain(b, neigh2, v);
		}

		// cross apex b
		else {
			// 1st edge is beer path
			if (direction == -1){
				subpathB = beer::print_beer_path(graph::get_edge(w, b));
				subpathB.push_back(v);
			}

			// 2nd edge is beer path
			else
				subpathB = beer::print_beer_path(graph::get_edge(b, v));
		}
	}

	pathB.insert(pathB.end(), subpathB.begin(), subpathB.end());
	return pathB;
}


vector<struct vertex*> dag::print_spsbp_dag(struct vertex* v){
	vector<struct vertex*> pathB = dag::print_subpathB_dag(v);
	return path_union(pathB);
}



/*############################ dynamic programming - get shortest path methods #########################*/

void dag::shortest_path_dag(){
	clock_t start = clock();
	mDAG[1]->dist = dag::weight(mDAG[0], mDAG[1]);
	mDAG[2]->dist = dag::weight(mDAG[0], mDAG[2]);
	mDAG[1]->pred = mDAG[2]->pred = mDAG[0];

	for (int i = 3, j = 0; i < mDAG.size(); i++, j = !j){
		mDAG[i]->dist = min(dcel::dist(mDAG[i - j - 1]) + dag::weight(mDAG[i - j - 1], mDAG[i]),
				    dcel::dist(mDAG[i - j - 2]) + dag::weight(mDAG[i - j - 2], mDAG[i]));

		if (mDAG[i]->dist == dcel::dist(mDAG[i - j - 1]) + dag::weight(mDAG[i - j - 1], mDAG[i]))
			mDAG[i]->pred = mDAG[i - j - 1];
		else mDAG[i]->pred = mDAG[i - j - 2];
	}

	spsp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	return;
}

void dag::shortest_beer_path_dag(){
	clock_t start = clock();
	mDAG[1]->distB = dcel::dist(mDAG[0]) + dag::weightB(mDAG[0], mDAG[1]);
	mDAG[2]->distB = dcel::dist(mDAG[0]) + dag::weightB(mDAG[0], mDAG[2]);
	mDAG[1]->predB = mDAG[2]->predB = make_pair(mDAG[0], 1);

	for (int i = 3, j = 0; i < mDAG.size(); i++, j = !j){
		mDAG[i]->distB = min(min(dcel::distB(mDAG[i - j - 1]) + dag::weight(mDAG[i - j - 1], mDAG[i]), 
					 dcel::dist(mDAG[i - j - 1]) + dag::weightB(mDAG[i - j - 1], mDAG[i])),
				     min(dcel::distB(mDAG[i - j - 2]) + dag::weight(mDAG[i - j - 2], mDAG[i]), 
					 dcel::dist(mDAG[i - j - 2]) + dag::weightB(mDAG[i - j - 2], mDAG[i])));

		if (dcel::distB(mDAG[i]) == dcel::distB(mDAG[i - j - 1]) + dag::weight(mDAG[i - j - 1], mDAG[i]))
			mDAG[i]->predB = make_pair(mDAG[i - j - 1], 0);
		else if (dcel::distB(mDAG[i]) == dcel::distB(mDAG[i - j - 2]) + dag::weight(mDAG[i - j - 2], mDAG[i]))
			mDAG[i]->predB = make_pair(mDAG[i - j - 2], 0);
		else if (dcel::distB(mDAG[i]) == dcel::dist(mDAG[i - j - 1]) + dag::weightB(mDAG[i - j - 1], mDAG[i]))
			mDAG[i]->predB = make_pair(mDAG[i - j - 1], 1);
		else mDAG[i]->predB = make_pair(mDAG[i - j - 2], 1);

	}

	spsbp_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	return;
}



/*################################ build dag methods #####################################################*/

struct d_edge* dag::compute_spedge_on_chain(struct vertex* apex, struct vertex* u, struct vertex* v){
	int index1 = dcel::apex_index(graph::get_edge(u, apex));
	int index2 = dcel::apex_index(graph::get_edge(v, apex));
	struct d_edge* sp_e = new d_edge(v);

	double path_weight_on_chain, path_weight_cross_apex;

	/* DAG shortest path edge */
	path_weight_on_chain = abs(dcel::dist2cw(apex, index1) - dcel::dist2cw(apex, index2));
	path_weight_cross_apex = dcel::weight(graph::get_edge(u, apex)) + dcel::weight(graph::get_edge(apex, v));
	sp_e->weight = min(path_weight_on_chain, path_weight_cross_apex);

	if (sp_e->weight == path_weight_on_chain)
		sp_e->subpath = make_pair(apex, 1);					// 1 if SP is on apex's chain
	else sp_e->subpath = make_pair(apex, 0);				// 0 is SP is crossing apex

	return sp_e;
}


struct d_edge* dag::compute_spbedge_on_chain(struct vertex* apex, struct vertex* u, struct vertex* v){
	struct halfedge* u2apex = graph::get_edge(u, apex);
	struct halfedge* v2apex = graph::get_edge(v, apex);
	int index1 = dcel::apex_index(u2apex);
	int index2 = dcel::apex_index(v2apex);

	struct d_edge* sp_be = new d_edge(v);

	/* DAG shortest path beer edge */
	struct lca* lca_chain = apex->lca_Av_chain;
	struct node* Av_smallest;
	// cw on chain
	if (index1 < index2)
		Av_smallest = get_lca(lca_chain, index1, index2 - 1); 
	// ccw on chain
	else Av_smallest = get_lca(lca_chain, index1 - 1, index2);


	double path_weight_on_chain, path_weight_cross_apex;
	path_weight_on_chain = abs(dcel::dist2cw(apex, index1) - dcel::dist2cw(apex, index2)) + dcel::dData(Av_smallest);
	path_weight_cross_apex = min(beer::weightB(u2apex) + dcel::weight(v2apex),
								 dcel::weight(u2apex) + beer::weightB(v2apex));
	sp_be->weight = min(path_weight_on_chain, path_weight_cross_apex);

	// if SP is on chain
	if (sp_be->weight == path_weight_on_chain)
		sp_be->subpath = make_pair(apex, dcel::iData(Av_smallest));

	// else SP is crossing apex
	else{
		if (sp_be->weight == beer::weightB(u2apex) + dcel::weight(v2apex))
			sp_be->subpath = make_pair(apex, -1);				// 1st edge "cross apex" is the beer path
		else
			sp_be->subpath = make_pair(apex, -2);				// 2nd edge "cross apex" is the beer path
	}

	return sp_be;
}


DAGEdges dag::compute_dag_edges_on_chain(struct vertex* col_vtx, struct halfedge* col_edge){
	struct vertex* apexv = dcel::apex(col_edge);

	struct d_edge* sp_e1 = compute_spedge_on_chain(apexv, col_vtx, dcel::target(col_edge));
	struct d_edge* sp_be1 = compute_spbedge_on_chain(apexv, col_vtx, dcel::target(col_edge));
	
	struct d_edge* sp_e2 = compute_spedge_on_chain(apexv, col_vtx, dcel::origin(col_edge));
	struct d_edge* sp_be2 = compute_spbedge_on_chain(apexv, col_vtx, dcel::origin(col_edge));

	return {make_pair(sp_e1, sp_be1), make_pair(sp_e2, sp_be2)};
}


DAGEdges dag::compute_dag_edges_off_chain(struct vertex* col_vtx, struct halfedge* col_edge){
	// DAG shortest path edges
	struct d_edge* sp_e1 = new d_edge(dcel::target(col_edge));
	struct d_edge* sp_e2 = new d_edge(dcel::origin(col_edge));
	sp_e1->weight = dcel::weight(dcel::next(col_edge));
	sp_e2->weight = dcel::weight(dcel::prev(col_edge));

	// DAG shortest path beer edges
	struct d_edge* sp_be1 = new d_edge(dcel::target(col_edge));
	struct d_edge* sp_be2 = new d_edge(dcel::origin(col_edge));
	sp_be1->weight = beer::weightB(dcel::next(col_edge));
	sp_be2->weight = beer::weightB(dcel::prev(col_edge));

	return {make_pair(sp_e1, sp_be1), make_pair(sp_e2, sp_be2)};
}



// Adding DAG column vertices at ith column  -  col_edge is at (i+1)th col
void dag::compute_dag_edges(struct vertex* col_vtx1, struct vertex* col_vtx2, struct halfedge* col_edge){
	if (col_vtx1 == dcel::apex(col_edge)){		// halfedge col_edge now faces vertex col_vtx1
		col_vtx1->dag_edges = dag::compute_dag_edges_off_chain(col_vtx1, col_edge);			//
		col_vtx2->dag_edges = dag::compute_dag_edges_on_chain(col_vtx2, col_edge);			//
	}
	else{	
		col_vtx1->dag_edges = dag::compute_dag_edges_on_chain(col_vtx1, col_edge);
		col_vtx2->dag_edges = dag::compute_dag_edges_off_chain(col_vtx2, col_edge);
	} 

	mDAG.push_back(col_vtx1);
	mDAG.push_back(col_vtx2);
} 



void dag::build_dag(graph& G, struct vertex* src, struct vertex* sink){
	struct node* u = dcel::face(src);
	struct node* v = dcel::face(sink);
	struct vertex* col_vtx1 = src;
	struct vertex* col_vtx2 = src;
	struct halfedge* col_edge = NULL; 
	struct halfedge* sink2vtx = NULL;

	clock_t start = clock();
	do {
		// Answer last label query and second node query
		struct node* lnq = G.last_node_query(col_vtx1, u, v);
		if (col_vtx1 != col_vtx2 && graph::incident_vertex2face(col_vtx2, lnq))
			lnq = G.last_node_query(col_vtx2, u, v);
		struct node* snq = G.second_node_query(lnq, v);
		u = snq;


		// Get the next (i+1)th DAG column edge
		if (dcel::face(dcel::twin(dcel::edge(lnq))) == snq)
			col_edge = dcel::edge(lnq);
		else col_edge = dcel::twin(dcel::edge(snq));


		// Build ith DAG column with col_vt1 & col_vtx2
		if (col_vtx1 == col_vtx2){						// edge case: 0th col in DAG
			src->dag_edges = dag::compute_dag_edges_off_chain(src, col_edge);
			mDAG.push_back(src);
		}
		else dag::compute_dag_edges(col_vtx1, col_vtx2, col_edge);		// ith col in DAG


		// Update col_vt1 & col_vtx2: Get the next (i+1)th DAG column vertices
		col_vtx1 = dcel::target(col_edge);
		col_vtx2 = dcel::origin(col_edge);
	} while(!(sink2vtx = graph::get_edge(sink, col_vtx1)) && !(sink2vtx = graph::get_edge(sink, col_vtx2)));

	// Build (L-1)th DAG column with col_vtx1 & col_vt2
	if (col_vtx1 != dcel::target(sink2vtx)){
		swap(col_vtx1, col_vtx2);
		col_edge = dcel::twin(col_edge);
	}

	// DAG shortest path edge
	struct d_edge* sp_e1 = new d_edge(sink);
	struct d_edge* sp_be1 = new d_edge(sink);
	sp_e1->weight = dcel::weight(sink2vtx);
	sp_be1->weight = beer::weightB(sink2vtx);
	sp_e1->subpath = sp_be1->subpath = NIL;

	// DAG shortest path beer edge
	struct d_edge* sp_e2 = dag::compute_spedge_on_chain(col_vtx1, col_vtx2, sink);
	struct d_edge* sp_be2 = dag::compute_spbedge_on_chain(col_vtx1, col_vtx2, sink);

	col_vtx1->dag_edges = {make_pair(sp_e1, sp_be1)};
	col_vtx2->dag_edges = {make_pair(sp_e2, sp_be2)};
	mDAG.push_back(col_vtx1);
	mDAG.push_back(col_vtx2);

	// Add last Lth DAG column with sink
	mDAG.push_back(sink);

	build_duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	return;
}



void dag::reset(){
	for (int i = 0; i < mDAG.size(); i++){
		mDAG[i]->pred = NULL;
		mDAG[i]->predB = NIL;

		// for (int j = 0; j < mDAG[i]->dag_edges.size(); j++){
		// 	delete mDAG[i]->dag_edges[j].first;
		// 	delete mDAG[i]->dag_edges[j].second;
		// }
	}

	mDAG.clear();
	return;
}

