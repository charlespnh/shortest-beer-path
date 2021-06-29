#ifndef SP_DAG_H
#define SP_DAG_H
#include <vector>
#include <limits>

using namespace std;

#include "../include/dcel.h"
#include "../include/lca.h"
#include "../include/btree.h"

typedef vector<struct SPEdge*> DAGEdges;
typedef vector<struct DAGVertex*> DAG;

const int INF = numeric_limits<int>::max();

struct SPEdge {
	double sp_weight;
	struct Vertex* src;
	struct Vertex* dest;
	struct Vertex* cross_apex;
	int cw;

	SPEdge(Vertex* s, Vertex* d, double w){
		src = s;
		dest = d;
		sp_weight = w;
		cross_apex = NULL;
		cw = INF;
	}

	SPEdge(){
	}
};

struct DAGVertex {
	struct Vertex* col_vtx;
	DAGEdges spes;

	DAGVertex(Vertex* v, DAGEdges e){
		col_vtx = v;
		spes = e;
	}	

	DAGVertex(Vertex* v){
		col_vtx = v;
		DAGEdges spes;
	}

	DAGVertex(){
	}
};


Node* second_node_query(Node* u, Node* v, lca* your_lca);
Node* last_node_query(Vertex* vertex, Node* u, Node* v, lca* your_lca);
DAG build_dag(Vertex* src, Vertex* sink, lca* your_lca);
double shortest_path_dag(DAG dag);
void shortest_path(Vertex* src, Vertex* dest);
void print_sp_dag(Vertex* v);

#endif