#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cmath>
#include <random>
#include <limits>

using namespace std;

// forward declaration
struct vertex;

// #define M_PI 3.14159265359
const int MAX = numeric_limits<int>::max();
const double INF = numeric_limits<double>::infinity();
const pair<struct vertex*, bool> NIL = make_pair((struct vertex*) NULL, 1);


double euclidean_dist(struct vertex* u, struct vertex* v);

int random_uniform(int range_to);
int random_bernoulli(float probability);

bool epsilon_equal(double a, double b, double epsilon = 0.0000001);

vector<struct vertex*> path_union(vector<struct vertex*>& arr);


#endif