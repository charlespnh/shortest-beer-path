
using namespace std;

#include "../include/utils.h"
#include "../include/graph/dcel.h"


double euclidean_dist(struct vertex* u, struct vertex* v){
    return sqrt((u->point.first - v->point.first) * (u->point.first - v->point.first) + 
                (u->point.second - v->point.second) * (u->point.second - v->point.second));
}


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


bool epsilon_equal(double a, double b, double epsilon){
    return abs(a - b) < epsilon;
}


vector<struct vertex*> path_union(vector<struct vertex*>& arr){
    vector<struct vertex*> union_arr;
    union_arr.reserve(arr.size());
    for (int i = 0; i < arr.size(); i++){
        if (arr[i] == arr[i + 1])
            union_arr.push_back(arr[++i]);
        else union_arr.push_back(arr[i]);
    }

    union_arr.shrink_to_fit();
    return union_arr;
}