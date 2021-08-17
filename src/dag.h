#ifndef SP_DAG_H
#define SP_DAG_H
#include <vector>
#include <tuple>
#include <utility>
#include <limits>

using namespace std;

#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"

typedef vector<pair<struct SPEdge*, struct SPBEdge*> > DAGEdges;
typedef vector<struct DAGVertex*> DAG;

template <typename T, typename X, typename Y>
struct Omega {
	T w;
	X subPath;
	Y beerLoc;

	Omega(T v, X s, Y b){
		w = v;
		subPath = s;
		beerLoc = b;
	}

	Omega(){
	}
};

struct SPEdge {
	double sp_weight;
	struct Vertex* src;
	struct Vertex* dest;
	struct Vertex* cross_apex;
	int cw;

	SPEdge(Vertex* s, Vertex* d){
		src = s;
		dest = d;
		cross_apex = NULL;
		cw = MAX;
	}

	SPEdge(){
	}
};

struct SPBEdge {
	double sp_bweight;
	struct Vertex* src;
	struct Vertex* dest;
	tuple<struct Vertex*, struct HalfEdge*, int> pathB;

	SPBEdge(Vertex* s, Vertex* d){
		src = s;
		dest = d;
		tuple<struct Vertex*, struct HalfEdge*, int> pathB;
	}

	SPBEdge(){
	}
};


struct DAGVertex {
	struct Vertex* col_vtx;
	double dist, distB;
	DAGEdges spes;
	tuple<struct DAGVertex*, struct SPBEdge*, bool> b;

	DAGVertex(Vertex* v, DAGEdges e){
		col_vtx = v;
		spes = e;
		dist = distB = 0.0;
		tuple<struct DAGVertex*, struct SPBedge*, bool> b;
	}	

	DAGVertex(Vertex* v){
		col_vtx = v;
		DAGEdges spes;
		dist = distB = 0.0;
		tuple<struct DAGVertex*, struct SPBedge*, bool> b;
	}

	DAGVertex(){
	}
};


struct Node* second_node_query(struct Node* u, struct Node* v, struct lca* your_lca);
struct Node* last_node_query(struct Vertex* vertex, struct Node* u, Node* v, struct lca* your_lca);
DAG build_dag(struct Vertex* src, struct Vertex* sink, struct lca* your_lca);
double shortest_path(struct Vertex* src, struct Vertex* dest, struct lca* your_lca);
double shortest_path_dag(DAG& dag);
double shortest_beer_path_dag(DAG& dag);
void print_sp_dag(struct Vertex* v);
void print_sbp_dag(struct DAGVertex* v);

#endif