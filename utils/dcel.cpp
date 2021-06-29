#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>

using namespace std;

#include "../include/btree.h"
#include "../include/lca.h"
#include "../src/sp_dag.h"


// Check if vertex v belongs to the Node face
// Return HalfEdge incident to Node face and whose target == v
HalfEdge* check_incident_vertex(Node* face, Vertex* v){
	HalfEdge* e = face->incident_edge;
	if (e->target == v)
		return e;
	else if (e->prev->target == v)
		return e->prev;
	else if (e->next->target == v)
		return e->next;
	return NULL;
}

// Check if v is on u's chain
// Return HalfEdge whose incident to v and apex == u (i.e. on u_chain)
HalfEdge* check_on_chain(Vertex* u, Vertex* v, lca* your_lca){
	Node* lnq = last_node_query(v, v->out_edge->incident_face, u->out_edge->incident_face, your_lca);
	HalfEdge* dag = lnq->incident_edge;
	if (dag->target == u)
		return dag->prev;
	else if (dag->prev->target == u)
		return dag->next;
	else if (dag->apex.first == u)
		return dag;
	return NULL;
}


void preprocess_graph(HalfEdge* root_edge, lca* your_lca){
	Vertex* curr = root_edge->target;
	HalfEdge* e = root_edge->twin;

	// Traversing each vertex of outerplanar graph
	do {
		HalfEdge* tmp_edge = e->twin;
		Node* dual_chain[2];
		int size = 0;
		int index_chain = 0;
		double dp = 0;
		// For each vertex, get the v_chain of planar graph and p_chain of dual tree
		do {
			if (tmp_edge->twin->incident_face == NULL || tmp_edge->next->twin->incident_face == NULL)
				dual_chain[size++] = tmp_edge->incident_face;

			curr->v_chain.push_back(make_pair(tmp_edge->prev->target, dp));
			dp += tmp_edge->prev->weight;
			tmp_edge->prev->apex.first = curr;
			tmp_edge->prev->apex.second = index_chain++;

			tmp_edge = tmp_edge->next->twin;
		} while (tmp_edge->next != e);
		curr->v_chain.push_back(make_pair(tmp_edge->prev->target, dp));
		// for (auto i : curr->v_chain) printf("Vertex %d | length %f\n", i.first->data, i.second);	cout << "\n";
		curr->p_chain_cw = curr->p_chain_ccw = dual_chain[0];
		if (size == 2){
			curr->p_chain_ccw = dual_chain[1];
			curr->h = get_lca(your_lca, curr->p_chain_cw, curr->p_chain_ccw);
		}
		else curr->h = curr->p_chain_cw;

		e->target->out_edge = e->twin;
		e = e->next;
		curr = e->twin->target;
	} while(curr != root_edge->target);
}


Vertex* add_vertex(int val, int radius){
	Vertex* new_vertex = new Vertex(val);
	new_vertex->point.first = radius * sin((val + 1) * (2*M_PI/radius));
	new_vertex->point.second = radius * cos((val + 1) * (2*M_PI/radius));
	
	return new_vertex;
}


HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2){
	Vertex* src = h->target;
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	double euclidian_dist = sqrt((dest->point.first - src->point.first) * (dest->point.first - src->point.first) + 
								(dest->point.second - src->point.second) * (dest->point.second - src->point.second));
	h1->weight = h2->weight = euclidian_dist;
	h1->twin = h2;
	h2->twin = h1;
	h1->target = dest;
	h2->target = src;
	h1->incident_face = f1;
	h2->incident_face = f2;
	if (f2 != NULL) f2->incident_edge = h2;
	h1->next = h2;
	h2->next = h->next;
	h1->prev = h;
	h2->prev = h1;
	h->next = h1;
	h2->next->prev = h2;

	return h1;
}

HalfEdge* split_face(HalfEdge* h, Vertex* dest, Node* f1, Node* f2){
	Vertex* src = h->target;
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	h1->weight = h2->weight = sqrt((dest->point.first - src->point.first) * (dest->point.first - src->point.first) + 
								   (dest->point.second - src->point.second) * (dest->point.second - src->point.second));
	h1->incident_face = f1;
	h2->incident_face = f2;
	if (f2 != NULL) f2->incident_edge = h2;
	h1->twin = h2;
	h2->twin = h1;
	h1->target = dest;
	h2->target = src;

	h2->next = h->next;
	h2->next->prev = h2;
	h1->prev = h;
	h->next = h1;

	h->prev->prev->next = h2;
	h2->prev = h->prev->prev;
	h1->next = h->prev;
	h1->next->prev = h1;

	return h1;
}


HalfEdge* build_polygon(Node* root, int V){
	Vertex* u = add_vertex(-1, V);
	Vertex* v = add_vertex(0, V);
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	h1->weight = h2->weight = sqrt((u->point.first - v->point.first) * (u->point.first - v->point.first) + 
								(u->point.second - v->point.second) * (u->point.second - v->point.second));
	h1->twin = h2;
	h2->twin = h1;
	h1->target = v;
	h2->target = u;
	h1->incident_face = root;
	h2->incident_face = NULL;
	h1->incident_face->incident_edge = h1;
	h1->next = h2;
	h2->next = h1;
	h1->prev = h2;
	h2->prev = h1;
	
	HalfEdge* root_edge = h1;
	triangulate_polygon(root_edge, root, V);

	return root_edge;
}


void triangulate_polygon(HalfEdge* root_edge, Node* root, int V){
	if (root == NULL) return;

	Vertex* v = add_vertex(root->data, V);
	HalfEdge* new_root_edge;
	// printf("Vertex %p   %d\n", v, v->data);

	new_root_edge = add_edge(root_edge, v, root_edge->incident_face, root->left);
	triangulate_polygon(new_root_edge->twin, root->left, V);

	new_root_edge = split_face(new_root_edge, root_edge->prev->target, root_edge->incident_face, root->right);
	triangulate_polygon(new_root_edge->twin, root->right, V);
}




void vertex_traversal(HalfEdge* root_edge){
	HalfEdge* e = root_edge->twin;
	do {
		printf("Vertex %p  %d\n", e->target, e->target->data);
		printf("Edge %p  (%d, %d)  %.10f\n", e, e->prev->target->data, e->target->data, e->weight);
		printf("Out edge %p  (%d, %d)  %.10f\n", e->target->out_edge, e->target->out_edge->prev->target->data, e->target->out_edge->target->data, 
												 e->target->out_edge->weight);
		printf("Face %p  %d\n\n", e->twin->incident_face, e->twin->incident_face->data);

		e = e->next;
	} while(e->target != root_edge->twin->target);
}


void face_traversal(Node* root){
	if (root == NULL) return;
	HalfEdge* e = root->incident_edge;
	do {
		printf("Vertex %p  %d\n", e->target, e->target->data);
		printf("Edge %p  (%d, %d)  %f\n", e, e->prev->target->data, e->target->data, e->weight);
		printf("Face %p  %d\n\n", e->incident_face, e->incident_face->data);

		e = e->next;
	} while(e->target != root->incident_edge->target);

	face_traversal(root->left);
	face_traversal(root->right);
}