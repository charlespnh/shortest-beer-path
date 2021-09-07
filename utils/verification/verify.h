#ifndef VERIFY_H
#define VERIFY_H

// forward declaration
struct halfedge;
struct vertex;


class verify {
	public:
		static bool verify_sssp(struct vertex* r, struct halfedge* e);
		static bool verify_sssbp(struct vertex* r, struct halfedge* e);

	private: 
		static bool verify_tree(struct vertex* r, struct halfedge* e);
		static bool verify_distance(struct vertex* v);
		static bool verify_predecessor(struct vertex* v);

		static bool verify_distanceB(struct vertex* v);
		static bool verify_predecessorB(struct vertex* v);
};


#endif	//