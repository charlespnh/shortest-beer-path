#include <cmath>
#include <random>

using namespace std;

#include "../../include/graph/dcel.h"
#include "../../include/preprocess/beer.h"
#include "../../include/utils.h"


namespace dcel {
	struct vertex* add_vertex(int val, int radius, float beer_prob){
		struct vertex* new_vertex = new vertex(val);
		new_vertex->point.first = radius * sin((val + 1) * (2*M_PI/radius));
		new_vertex->point.second = radius * cos((val + 1) * (2*M_PI/radius));
		new_vertex->is_beer = (val != 0 && val % 13 == 0) ? true : false; // random_bernoulli(beer_prob);
		new_vertex->beer_edge = new b_edge(new_vertex, new_vertex);
		return new_vertex;
	}



	struct halfedge* add_edge(struct halfedge* h, struct vertex* dest, struct node* f1, struct node* f2){
		struct vertex* src = h->target;
		struct halfedge* h1 = new halfedge();
		struct halfedge* h2 = new halfedge();
		h1->weight = h2->weight = euclidean_dist(src, dest);
		h1->beer_edge = h2->beer_edge = new b_edge(src, dest);

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



	struct halfedge* split_face(struct halfedge* h, struct vertex* dest, struct node* f1, struct node* f2){
		struct vertex* src = h->target;
		struct halfedge* h1 = new halfedge();
		struct halfedge* h2 = new halfedge();
		h1->weight = h2->weight = euclidean_dist(src, dest);
		h1->beer_edge = h2->beer_edge = new b_edge(src, dest);
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





	int data(struct vertex* v)										{ return v->data; }
	bool beer(struct vertex* v)										{ return v->is_beer; }
	struct node* face(struct vertex* v)								{ return (face(v->adj[0])) ? face(v->adj[0]) : face(v->adj[1]); }
	struct b_edge* edgeB(struct vertex* v)							{ return v->beer_edge; }
	double dist2cw(struct vertex* v, int i)							{ return v->v_chain[i].second; }
	struct vertex* neighbour(struct vertex* v, int i)				{ return v->v_chain[i].first; }
	struct node* cw(struct vertex* v)								{ return v->cw; }
	struct node* ccw(struct vertex* v)								{ return v->ccw; }
	struct node* h(struct vertex* v)								{ return v->h; }
	double dist(struct vertex* v)									{ return v->dist; }
	double distB(struct vertex* v)									{ return v->distB; }
	struct vertex* pred(struct vertex* v)							{ return v->pred; }
	pair<struct vertex*, bool> predB(struct vertex* v)				{ return v->predB; }



	double weight(struct halfedge* e)								{ return e->weight; }
	struct vertex* target(struct halfedge* e)						{ return e->target; }
	struct vertex* origin(struct halfedge* e)						{ return e->twin->target; }
	struct node* face(struct halfedge* e)							{ return e->incident_face; }
	struct halfedge* twin(struct halfedge* e)						{ return e->twin; }			
	struct halfedge* next(struct halfedge* e)						{ return e->next; }			
	struct halfedge* prev(struct halfedge* e)						{ return e->prev; }			
	struct b_edge* edgeB(struct halfedge* e)						{ return e->beer_edge; }
	int apex_index(struct halfedge* e)								{ return e->index; }
	struct vertex* apex(struct halfedge* e)							{ return e->next->target; }



	int iData(struct node* n)               						{ return n->iData; }
	double dData(struct node* n)            						{ return n->dData; }
	struct halfedge* edge(struct node* n)   						{ return n->incident_edge; }
	struct node* left(struct node* n)       						{ return n->left; }
	struct node* right(struct node* n)      						{ return n->right; }


}	// ending namespace dcel

