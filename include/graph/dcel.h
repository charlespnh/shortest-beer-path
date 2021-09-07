#ifndef DCEL_H
#define DCEL_H
#include <vector>
#include <utility>

using namespace std;

#include "../utils.h"

// forward declaration
struct lca;
struct b_edge;
struct d_edge;

typedef vector<pair<struct d_edge*, struct d_edge*> > DAGEdges;


struct vertex {
	// adjacency list of constant size
	vector<struct halfedge*> adj;

	/*************** preprocess data ***************/
	// chain data
	vector<pair<struct vertex*, double> > v_chain;
	struct lca*  lca_Av_chain;
	struct node* cw;
	struct node* ccw;
	struct node* h;

	// beer edge data
	struct b_edge* beer_edge; 
	/***********************************************/

	/********** shortest (beer) path data **********/
	// single source
	double dist, distB;
	struct vertex* pred;
	pair<struct vertex*, bool> predB;

	// directed acyclic graph
	DAGEdges dag_edges;
	/***********************************************/

	// ID
	int data;
	pair<double, double> point;
	bool is_beer;


	vertex(int val) {
		data = val;
		pair<double, double> point;
		is_beer = false;
		beer_edge = NULL;

		vector<struct halfedge*> adj;

		vector<pair<struct vertex*, double> > v_chain;
		lca_Av_chain = NULL;
		cw = ccw = h = NULL;

		dist = distB = 0.0;
		pred = NULL;
		predB = NIL;
		DAGEdges dag_edges;
	}

	vertex(){
	}
};

namespace dcel {
	int data(struct vertex* v);					
	bool beer(struct vertex* v);
	struct node* face(struct vertex* v);			
	struct b_edge* edgeB(struct vertex* v);	
	double dist2cw(struct vertex* v, int i);
	struct vertex* neighbour(struct vertex* v, int i);
	struct node* cw(struct vertex* v);
	struct node* ccw(struct vertex* v);
	struct node* h(struct vertex* v);
	double dist(struct vertex* v);				
	double distB(struct vertex* v);				
	struct vertex* pred(struct vertex* v);
	pair<struct vertex*, bool> predB(struct vertex* v);
}



	/*###############################################################################################################*/



struct halfedge { 
	double weight;
	struct vertex* target;
	struct node* incident_face;
	struct halfedge* twin;
	struct halfedge* next;
	struct halfedge* prev;
	
	// beer edge data
	struct b_edge* beer_edge;

	// index into apex's chain
	int index;

	halfedge() {
		weight = -1.0;
		target = NULL;
		incident_face = NULL;
		twin = next = prev = NULL;
		beer_edge = NULL;
		index = -1;
	}
};

namespace dcel {
	double weight(struct halfedge* e);
	struct vertex* target(struct halfedge* e);
	struct vertex* origin(struct halfedge* e);
	struct node* face(struct halfedge* e);
	struct halfedge* twin(struct halfedge* e);			
	struct halfedge* next(struct halfedge* e);			
	struct halfedge* prev(struct halfedge* e);			
	struct b_edge* edgeB(struct halfedge* e);
	int apex_index(struct halfedge* e);
	struct vertex* apex(struct halfedge* e);
}



	/*###############################################################################################################*/



struct node {
    int iData;
    double dData;
    struct halfedge* incident_edge;
    struct node* left;
    struct node* right;
 
    node (int iVal){
        iData = iVal;
        double dData;
        incident_edge = NULL;
        left = NULL;
        right = NULL;
    }

    node (int iVal, double dVal){
        iData = iVal;
        dData = dVal;
        incident_edge = NULL;
        left = NULL;
        right = NULL;
    }

    node (){
    }
};

namespace dcel {
	int iData(struct node* n);            
	double dData(struct node* n);         
	struct halfedge* edge(struct node* n);
	struct node* left(struct node* n);    
	struct node* right(struct node* n);
} 



	/*###############################################################################################################*/



namespace dcel {
	struct vertex* add_vertex(int val, int radius, float beer_prob);
	struct halfedge* add_edge(struct halfedge* h, struct vertex* dest, struct node* f1, struct node* f2);
	struct halfedge* split_face(struct halfedge* h, struct vertex* dest, struct node* f1, struct node* f2);
}

#endif
