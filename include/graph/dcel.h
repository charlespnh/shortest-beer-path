#ifndef DCEL_H
#define DCEL_H
#include <vector>
#include <tuple>
#include <utility>

using namespace std;

#include "btree.h"
#include "lca.h"
#include "beer.h"
#include "../src/dag.h"

struct HalfEdge { 
	double weight;
	struct BEdge* beer_edge;
	struct Vertex* target;
	struct Node* incident_face;
	struct HalfEdge* twin;
	struct HalfEdge* next;
	struct HalfEdge* prev;
	pair<struct Vertex*, int> apex;

	HalfEdge() {
		weight = -1.0;
		beer_edge = NULL;
		target = NULL;
		incident_face = NULL;
		twin = next = prev = NULL;
	}
};

struct Vertex {
	int data;
	pair<double, double> point;
	bool beer;
	struct BEdge* beer_edge; 
	struct Chain* chain;
	struct SPEdge* p;
	int col;

	double dist, distB;
	struct Vertex* pred;
	pair<struct HalfEdge*, bool> predB;

	Vertex(double x, double	y, int val) {
		data = val;
		point = make_pair(x, y);
		beer = false;
		beer_edge = NULL;
		chain = NULL;
		p = NULL;
		col = -1;
		dist = distB = 0.0;
		pred = NULL;
	}

	Vertex(int val) {
		data = val;
		point = make_pair(0, 0);
		beer = false;
		beer_edge = NULL;
		chain = NULL;
		p = NULL;
		col = -1;
		dist = distB = 0.0;
		pred = NULL;
	}

	Vertex(){
	}
};

struct Chain{
	vector<pair<struct HalfEdge*, double> > v_chain;
	// vector<double> Av;
	struct Node* root_Av;
	struct lca* lca_chain_Av;
	struct Node* p_chain_cw;
	struct Node* p_chain_ccw;
	struct Node* p_chain_h;

	Chain(){
		vector<pair<struct HalfEdge*, double> > v_chain;
		// vector<double> Av;
		root_Av = NULL;
		lca_chain_Av = NULL;
		p_chain_cw = NULL;
		p_chain_ccw = NULL;
		p_chain_h = NULL;
	}
};



HalfEdge* split_face(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
Vertex* add_vertex(int val, int radius, float beer_prob);
HalfEdge* build_polygon(Node* root, int V, float beer_prob);
void triangulate_polygon(HalfEdge *root_edge, Node* root, int V, float beer_prob);
HalfEdge* check_incident_vertex(Node* face, Vertex* v);
HalfEdge* check_on_chain(Vertex* u, Vertex* v, lca* your_lca);
void preprocess_graph(struct HalfEdge* root_edge, struct lca* your_lca);

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

struct HalfEdge* get_inEdge(Vertex* v);

void vertex_traversal(HalfEdge* root_edge);
void edge_traversal(HalfEdge* root_edge);
void face_traversal(Node* root);

#endif