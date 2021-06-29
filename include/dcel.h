#ifndef DCEL_H
#define DCEL_H
#include <vector>
#include <utility>

using namespace std;

#include "btree.h"
#include "lca.h"
#include "../src/sp_dag.h"

// typedef pair<double, double> Coord;
// typedef vector<pair<struct Vertex*, double> > Chain;

struct HalfEdge { 
	double weight;
	struct Vertex* target;
	struct Node* incident_face;
	struct HalfEdge* twin;
	struct HalfEdge* next;
	struct HalfEdge* prev;
	pair<Vertex*, int> apex;

	HalfEdge() {
		weight = -1.0;
		target = NULL;
		incident_face = NULL;
		twin = next = prev = NULL;
	}
};

struct Vertex {
	int data;
	pair<double, double> point;
	struct HalfEdge* out_edge;
	vector<pair<struct Vertex*, double> > v_chain;
	struct Node* p_chain_cw;
	struct Node* p_chain_ccw;
	struct Node* h;
	struct SPEdge* p;
	int col;

	Vertex(double x, double	y, int val) {
		data = val;
		point = make_pair(x, y);
		out_edge = NULL;
		p_chain_cw = p_chain_ccw = NULL;
		h = NULL;
		p = NULL;
		col = -1;
	}

	Vertex(int val) {
		data = val;
		point = make_pair(0, 0);
		out_edge = NULL;
		p_chain_cw = p_chain_ccw = NULL;
		h = NULL;
		p = NULL;
		col = -1;
	}

	Vertex(){

	}
};


HalfEdge* split_face(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
Vertex* add_vertex(int val, int radius);
HalfEdge* build_polygon(Node* root, int V);
void triangulate_polygon(HalfEdge *root_edge, Node* root, int V);
HalfEdge* check_incident_vertex(Node* face, Vertex* v);
HalfEdge* check_on_chain(Vertex* u, Vertex* v, lca* your_lca);
void preprocess_graph(HalfEdge* root_edge, lca* your_lca);

double w(HalfEdge* e);
Node* face(HalfEdge* e);
Vertex* target(HalfEdge* e);
HalfEdge* twin(HalfEdge* e);
HalfEdge* next(HalfEdge* e);
HalfEdge* prev(HalfEdge* e);
Vertex* apex(HalfEdge* e);
int apex_index(HalfEdge* e);

int data(Vertex* v);
double x_coord(Vertex* v);
double y_coord(Vertex* v);
HalfEdge* out(Vertex* v);
Node* cw(Vertex* v);
Node* ccw(Vertex* v);
Node* h(Vertex* v);

void vertex_traversal(HalfEdge* root_edge);
void face_traversal(Node* root);

#endif