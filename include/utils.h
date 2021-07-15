#ifndef UTILS_H
#define UTILS_H
#include <tuple>
#include <string>
#include <random>
#include <limits>

using namespace std;

// #define M_PI 3.14159265359

const int MAX = numeric_limits<unsigned int>::max();
const double INF = numeric_limits<double>::infinity();
const tuple<struct Vertex*, struct HalfEdge*, bool, string> NIL = make_tuple((struct Vertex*) NULL, (struct HalfEdge*) NULL, 0, "\0");

int random_uniform(int range_to);
int random_bernoulli(float probability);
double euclidean_dist(struct Vertex* u, struct Vertex* v);
bool epsilon_equal(double a, double b, double epsilon = 0.0000001);

#endif