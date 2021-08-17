#ifndef SSSP_H
#define SSSP_H

#include <string>
#include <vector>

using namespace std;

#include "../include/dcel.h"
#include "../include/btree.h"
#include "../include/beer.h"

void preorder_traversal_sssp(struct HalfEdge* ab);
void sssp(struct HalfEdge* ab);
void preorder_traversal_sssbp(struct HalfEdge* ab);
void sssbp(struct HalfEdge* ab);
vector<struct Vertex*> print_sssp(struct Vertex* v);
vector<struct Vertex*> print_sssbp(struct Vertex* v);
vector<struct Vertex*> print_sssbp_subpath(struct Vertex* v);

#endif