#ifndef BEER_H
#define BEER_H
#include <utility>

using namespace std;

#include "../utils.h"

// forward declaration
struct vertex;
struct halfedge;
struct node;


struct b_edge {
	struct vertex* u;
	struct vertex* v;
	double distB;
	pair<struct vertex*, bool> pathB;

	b_edge(struct vertex* a, struct vertex* b){
		u = a;
		v = b;
		distB = INF;
		pathB = NIL;
	}

	b_edge(){
	}
};


class beer {
	public:
		// main methods
		static void computeBeerDistNotRoot(struct node* R);
		static void computeBeerDistRoot(struct node* R);
		static vector<struct vertex*> print_beer_path(struct halfedge* e);
		static vector<struct vertex*> print_beer_path(struct vertex* v);

		// get methods
		static struct b_edge* get_edgeB(struct vertex* u, struct vertex* v);
		static double weightB(struct vertex* v);
		static double weightB(struct halfedge* e);
		static struct vertex* originB(struct halfedge* e);
		static struct vertex* targetB(struct halfedge* e);
		static struct vertex* originB(struct vertex* v);
		static struct vertex* targetB(struct vertex* v);

	private:
		// utils methods
		static void beerDistNotRoot(struct node* F);
		static void beerDistRoot(struct node* F);
		static vector<struct vertex*> print_beer_path(struct b_edge* e);
		static vector<struct vertex*> print_beer_subpath(struct b_edge* e);

};


#endif

