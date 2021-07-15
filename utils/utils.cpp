#include <cmath>

using namespace std;

#include "../include/utils.h"
#include "../include/dcel.h"

int random_uniform(int range_to) {
    random_device                  rand_dev;
    mt19937                        generator(rand_dev());
    uniform_int_distribution<int>  distr(0, range_to);
    return distr(generator);
}

int random_bernoulli(float prob){
    random_device                  rand_dev;
    mt19937                        generator(rand_dev());
    bernoulli_distribution         distr(prob);
    return distr(generator);
}

double euclidean_dist(struct Vertex* u, struct Vertex* v){
    return sqrt((u->point.first - v->point.first) * (u->point.first - v->point.first) + 
                (u->point.second - v->point.second) * (u->point.second - v->point.second));
}

bool epsilon_equal(double a, double b, double epsilon){
    return abs(a - b) < epsilon;
}
