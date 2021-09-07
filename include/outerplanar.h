#ifndef OUTERPLANAR_H
#define OUTERPLANAR_H
#include <vector>
#include <utility>
#include <ctime>

using namespace std;

// forward declaration
struct vertex;
struct halfedge;
struct node;
struct lca;

class graph {
	public:
		clock_t triangulate_start;
		double triangulate_duration;
		clock_t lca_start;
		double lca_duration;
		clock_t beer_start;
		double beer_duration;
		clock_t chain_start;
		double chain_duration;

		int mV;
		float mbeer_probability;
		struct halfedge* mroot_edge;
		struct node* mroot_node;
		struct lca* mdual_lca;

		graph(int V = 0, float beer_prob = 0.03){
			mV = V;
			mbeer_probability = beer_prob;
			mroot_edge = NULL;
			mroot_node = NULL;
			mdual_lca = NULL;
		}
		~graph() = default;

		void build_dual(int dual_t);
		void build_polygon();
		void triangulate_polygon(struct halfedge *root_edge, struct node* root_node);

		void preprocess_graph();

		struct node* last_node_query(struct vertex* vertex, struct node* u, struct node* v);
		struct node* second_node_query(struct node* u, struct node* v);

		struct vertex* get_vertex(int v);
		static struct halfedge* get_edge(struct vertex* u, struct vertex* v);
		static bool incident_vertex2face(struct vertex* u, struct node* f);

	private:
		void preprocess_chain_spath(struct vertex* apex, struct halfedge* edge);
		void preprocess_chain_bpath(struct vertex* apex, struct halfedge* edge);
};


#endif