#ifndef SP_DAG_H
#define SP_DAG_H
#include <vector>
#include <utility>

using namespace std;

#include "outerplanar.h"
#include "../include/utils.h"

// forward declaration
struct halfedge;
struct vertex;

typedef vector<pair<struct d_edge*, struct d_edge*> > DAGEdges;


struct d_edge {
	double weight;
	struct vertex* dest;
	pair<struct vertex*, int> subpath;

	d_edge(struct vertex* d){
		double weight;
		dest = d;
		subpath = NIL;
	}

	d_edge(){
	}
};



class dag {
	public:
		vector<struct vertex*> mDAG;
		double build_duration;
		double spsp_duration;
		double get_spsp_duration;
		double spsbp_duration;
		double get_spsbp_duration;

		dag() = default;
		~dag() = default;

		// main methods
		void build_dag(graph& G, struct vertex* src, struct vertex* sink);
		void compute_dag_edges(struct vertex* col_vtx1, struct vertex* col_vtx2, struct halfedge* col_edge);
		void shortest_path_dag();
		void shortest_beer_path_dag();
		vector<struct vertex*> print_spsp_dag(struct vertex* v);
		vector<struct vertex*> print_spsbp_dag(struct vertex* v);

		void reset();

		// get methods
		static pair<struct vertex*, int> omega(struct d_edge* e);
		static struct d_edge* getDEdge(struct vertex* u, struct vertex* v);
		static struct d_edge* getDBEdge(struct vertex* u, struct vertex* v);
		static double weight(struct vertex* u, struct vertex* v);
		static double weightB(struct vertex* u, struct vertex* v);


	private:
		// util methods
		vector<struct vertex*> print_subpath_dag(struct vertex* pred, struct vertex* v);
		vector<struct vertex*> print_subpathB_dag(struct vertex* v);

		static DAGEdges compute_dag_edges_off_chain(struct vertex* col_vtx, struct halfedge* col_edge);
		static DAGEdges compute_dag_edges_on_chain(struct vertex* col_vtx, struct halfedge* col_edge);
		static struct d_edge* compute_spedge_on_chain(struct vertex* apex, struct vertex* u, struct vertex* v);
		static struct d_edge* compute_spbedge_on_chain(struct vertex* apex, struct vertex* u, struct vertex* v);

};

#endif
