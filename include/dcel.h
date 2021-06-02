#include <utility>

using namespace std;

#include "btree.h"


struct HalfEdge { 
	double weight;
	struct Vertex* target;
	struct Node* incident_face;
	struct HalfEdge* twin;
	struct HalfEdge* next;
	struct HalfEdge* prev;

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
	struct Vertex* v_chain_cw;
	struct Vertex* v_chain_ccw;
	struct Node* p_chain_cw;
	struct Node* p_chain_ccw;
	struct Node* h;

	Vertex(double x, double	 y, int val) {
		data = val;
		point = make_pair(x, y);
		out_edge = NULL;
		v_chain_cw = v_chain_cw = NULL;
		p_chain_cw = p_chain_ccw = NULL;
		h = NULL;
	}

	Vertex(int val) {
		data = val;
		point = make_pair(0, 0);
		out_edge = NULL;
		v_chain_cw = v_chain_cw = NULL;
		p_chain_cw = p_chain_ccw = NULL;
		h = NULL;
	}

	Vertex(){

	}
};


HalfEdge* split_face(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2);
Vertex* add_vertex(int val, int radius);
HalfEdge* build_polygon(Node* root, int V);
void triangulate_polygon(HalfEdge *root_edge, Node* root, int V);

void vertex_traversal(HalfEdge* root_edge);
void face_traversal(Node* root);