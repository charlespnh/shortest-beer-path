#include <unordered_set>
#include <cmath>
#include <random>
#include <cstdio>
#include <iostream>

using namespace std;

#include "../include/dcel.h"
#include "../include/beer.h"
#include "../include/btree.h"
#include "../include/lca.h"
#include "../include/utils.h"


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


void preprocess_graph(struct HalfEdge* root_edge, struct lca* dual_lca){
	// Beer Distance - post-order traversal of D(G)
	computeBeerDistNotRoot(root_edge, root_edge->incident_face);
	// Beer Distance - pre-order traversal of D(G)
	computeBeerDistRoot(root_edge, root_edge->incident_face);

	struct Vertex* curr = root_edge->target;
	struct HalfEdge* traverse_e = root_edge->twin;
	int cnt = 0;
	// Traversing each vertex of outerplanar graph
	do {
		struct Chain* curr_chain = new Chain();
		struct HalfEdge* tmp_e = traverse_e->twin;
		struct Node* dual_chain[2];
		int size = 0;
		int index_chain = 0;
		double dp = 0;
		// For each vertex, preprocess the v_chain of planar graph and p_chain of dual tree
		do {
			if (tmp_e->twin->incident_face == NULL || tmp_e->next->twin->incident_face == NULL)
				dual_chain[size++] = tmp_e->incident_face;

			curr_chain->v_chain.push_back(make_pair(tmp_e, dp));
			curr_chain->Av.push_back(tmp_e->prev->beer_edge->distB - tmp_e->prev->weight);
			dp += tmp_e->prev->weight;
			tmp_e->prev->apex.first = curr;
			tmp_e->prev->apex.second = index_chain++;

			tmp_e = tmp_e->next->twin;
		} while (tmp_e->next != traverse_e);
		curr_chain->v_chain.push_back(make_pair(tmp_e, dp));

		struct Node* node = build_cartesian_tree(curr_chain->Av);
		struct lca* v_chain_lca = new lca(curr_chain->Av.size());
		precompute_lca(v_chain_lca, node);
		curr_chain->root_Av = node;
		curr_chain->lca_chain_Av = v_chain_lca;
		// cout << "Vertex " << curr->data << ": ";	for (auto i : curr_chain->Av) cout << i << " | ";	cout << "\n";

		curr_chain->p_chain_cw = curr_chain->p_chain_ccw = dual_chain[0];
		if (size == 2){
			curr_chain->p_chain_ccw = dual_chain[1];
			curr_chain->p_chain_h = get_lca(dual_lca, curr_chain->p_chain_cw, curr_chain->p_chain_ccw);
		}
		else curr_chain->p_chain_h = curr_chain->p_chain_cw;

		traverse_e->target->out_edge = traverse_e->twin;
		traverse_e = traverse_e->next;
		curr->chain = curr_chain;
		curr = traverse_e->twin->target;
	} while(curr != root_edge->target);

	/* do {
		struct Node* node = build_cartesian_tree(curr->chain->Av);
		struct lca* v_chain_lca = new lca();
		// precompute_lca(v_chain_lca, node);
		// cout << cnt++ << "\n";
		curr->chain->root_Av = node;
		curr->chain->lca_chain_Av = v_chain_lca;

		traverse_e = traverse_e->next;
		curr = traverse_e->twin->target;
	} while(curr != root_edge->target); */
}


Vertex* add_vertex(int val, int radius, float beer_prob){
	Vertex* new_vertex = new Vertex(val);
	new_vertex->point.first = radius * sin((val + 1) * (2*M_PI/radius));
	new_vertex->point.second = radius * cos((val + 1) * (2*M_PI/radius));
	new_vertex->beer = random_bernoulli(beer_prob);
	new_vertex->beer_edge = new BEdge(new_vertex, new_vertex);
	return new_vertex;
}


HalfEdge* add_edge(HalfEdge* h, Vertex* dest, Node* f1, Node* f2){
	Vertex* src = h->target;
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	h1->weight = h2->weight = euclidean_dist(src, dest);
	h1->beer_edge = h2->beer_edge = new BEdge(src, dest);
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
	h1->weight = h2->weight = euclidean_dist(src, dest);
	h1->beer_edge = h2->beer_edge = new BEdge(src, dest);
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


HalfEdge* build_polygon(Node* root, int V, float beer_prob){
	Vertex* u = add_vertex(-1, V, beer_prob);
	Vertex* v = add_vertex(0, V, beer_prob);
	HalfEdge* h1 = new HalfEdge();
	HalfEdge* h2 = new HalfEdge();
	h1->weight = h2->weight = euclidean_dist(u, v);
	h1->beer_edge = h2->beer_edge = new BEdge(u, v);
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
	triangulate_polygon(root_edge, root, V, beer_prob);

	return root_edge;
}


void triangulate_polygon(HalfEdge* root_edge, Node* root, int V, float beer_prob){
	if (root == NULL) return;

	Vertex* v = add_vertex(root->data, V, beer_prob);
	HalfEdge* new_root_edge;

	new_root_edge = add_edge(root_edge, v, root_edge->incident_face, root->left);
	triangulate_polygon(new_root_edge->twin, root->left, V, beer_prob);

	new_root_edge = split_face(new_root_edge, root_edge->prev->target, root_edge->incident_face, root->right);
	triangulate_polygon(new_root_edge->twin, root->right, V, beer_prob);
}




void vertex_traversal(HalfEdge* root_edge){
	HalfEdge* e = root_edge->twin;
	do {
		printf("Vertex %d | beer? %d\n", e->target->data, e->target->beer);
		printf("Edge (%d, %d)  %f\n", e->prev->target->data, e->target->data, e->weight);
		printf("BEdge %d %f\n", e->target->data, e->target->beer_edge->distB);
		printf("BEdge (%d, %d) %f\n", e->beer_edge->u->data, e->beer_edge->v->data, e->beer_edge->distB);
		printf("Face %d\n\n", e->twin->incident_face->data);

		e = e->next;
	} while(e->target != root_edge->twin->target);
}

void face_traversal(Node* root){
	if (root == NULL) return;
	HalfEdge* e = root->incident_edge;

	printf("Face %d\n", e->incident_face->data);
	for (int i = 0; i < 3; i++){
		printf("Vertex %d | beer? %d\n", e->target->data, e->target->beer);
		printf("BEdge %d %f\n", e->target->data, e->target->beer_edge->distB);
		printf("Edge (%d, %d)  %f\n", e->prev->target->data, e->target->data, e->weight);
		printf("BEdge (%d, %d) %f\n", e->beer_edge->u->data, e->beer_edge->v->data, e->beer_edge->distB);
		e = e->next;
	}
	cout << "\n";

	face_traversal(root->left);
	face_traversal(root->right);
}

void edge_traversal(HalfEdge* root_edge){
	unordered_set<HalfEdge*> visited;
	HalfEdge* traverse = root_edge->twin;
	do {
		Vertex* curr = traverse->target;
		printf("\nVertex %d | beer? %d\n", traverse->target->data, traverse->target->beer);
		printf("BEdge %d %f\n", traverse->target->data, traverse->target->beer_edge->distB);
		for (auto i : curr->chain->v_chain){
			auto found = visited.find(i.first);
			auto found_twin = visited.find(i.first->twin);
			if (found != visited.end() || found_twin != visited.end()) continue;

			visited.insert(i.first);
			printf("Edge (%d, %d)  %f\n", i.first->prev->target->data, i.first->target->data, i.first->weight);
			printf("BEdge (%d, %d) %f\n", i.first->beer_edge->u->data, i.first->beer_edge->v->data, i.first->beer_edge->distB);
			print_beer_path(i.first);
		}
		traverse = traverse->next;
	} while (traverse != root_edge->twin);
}