#include <iostream>
#include <cstdio>
#include <cmath>
#include <ctime>

using namespace std;

#include "../include/dcel.h"
#include "../include/lca.h"


void preprocess(){

}


int main(){
	int n = 1000000;
	struct lca* your_lca = new lca();
	clock_t start = clock();
	Node* root_node = generate(n);
	cout << "Generating random binary tree took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";

	your_lca->tree_depth = maxDepth(root_node);
	int lDepth = maxDepth(root_node->left);
	int rDepth = maxDepth(root_node->right);
	cout << "Root: " << root_node->data << "\n";
	cout << "Depth: " << your_lca->tree_depth << "\n";
	cout << "lDepth: " << lDepth << "\n";
	cout << "rDepth: " << rDepth << "\n\n";

	// cout << "Preorder traversal: \n";	traversal(root_node);	cout << "\n\n";
	n = inorder_label(root_node, 1) - 1;
	cout << "Number of nodes: " << n << "\n\n";
	cout << "New root after inorder labelling: " << root_node->data << "\n\n";  
	// cout << "Inorder traversal: \n";	inorder_traversal(root_node);	cout << "\n\n";

	start = clock();
	HalfEdge* root_edge = build_polygon(root_node, n + 2);
	cout << "Triangulation took: " << (clock() - start) / (double) CLOCKS_PER_SEC << "\n\n";
	// vertex_traversal(root_edge); cout << "DONE\n\n";
	// face_traversal(root_node); cout << "DONE\n\n";

	precompute_lca(your_lca, root_node);
	your_lca->preprocessing_duration = (clock() - your_lca->preprocessing_start ) / (double) CLOCKS_PER_SEC;
    cout << "LCA preprossing took: " << your_lca->preprocessing_duration << " s\n";

	struct Node *u, *v;
	u = v = root_node->left;

	for (int i = 0; i < 10; i++){
		if (u->left) u = u->left;
		//else continue;

		if (u->right) u = u->right;
		//else continue;
	}

	for (int i = 0; i < 10; i++){
		if (v->right) v = v->right;
		//else continue;

		if (v->left) v = v->left;
		//else continue;
	}

	cout << "Node u: " << u->data << "\n";
	cout << "Node v: " << v->data << "\n";

	your_lca->getting_start = clock();
	Node* res = get_lca(your_lca, u->data, v->data);
	your_lca->getting_duration = (clock() - your_lca->getting_start ) / (double) CLOCKS_PER_SEC;
	cout << "Actual result: " << res->data << " Result should be: " << root_node->left->data << " Getting result took: " << your_lca->getting_duration << "\n\n";
	free_mem(root_node);

	return 0;
}


Vertex* add_vertex(int val, int radius){
	Vertex* new_vertex = new Vertex(val);
	new_vertex->point.first = (double) radius * sin((val + 1) * (2*M_PI/radius));
	new_vertex->point.second = (double) radius * cos((val + 1) * (2*M_PI/radius));
	
	return new_vertex;
}


HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2){
	Vertex* src = h->target;
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	double euclidian_dist = sqrt((dest->point.first - src->point.first) * (dest->point.first - src->point.first) + 
								(dest->point.second - src->point.second) * (dest->point.second - src->point.second));
	h1->weight = h2->weight = euclidian_dist;
	dest->out_edge = h2;
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
	u->out_edge = h1;
	v->out_edge = h2;
	h1->twin = h2;
	h2->twin = h1;
	h1->target = v;
	h2->target = u;
	h1->incident_face = root;
	h2->incident_face = new Node(0);
	h1->incident_face->incident_edge = h1;
	h2->incident_face->incident_edge = h2;
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
		printf("Edge %p  (%d, %d)  %f\n", e, e->prev->target->data, e->target->data, e->weight);
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
