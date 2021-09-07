#ifndef SSSP_H
#define SSSP_H

#include <vector>

using namespace std;

// forward declaration
struct vertex;
struct halfedge;


class single_source {
	public:
		inline static double sssp_duration;
		inline static double sssbp_duration;

		static void sssp(struct halfedge* ab);
		static void sssbp(struct halfedge* ab);
		static vector<struct vertex*> print_sssp(struct vertex* v);
		static vector<struct vertex*> print_sssbp(struct vertex* v);

	private:
		static void preorder_traversal_sssp(struct halfedge* ab);
		static void preorder_traversal_sssbp(struct halfedge* ab);
		static vector<struct vertex*> print_sssbp_subpath(struct vertex* v);
};

#endif