# Shortest Beer Path on Maximal Outerplanar Graph

A C++ implementation of shortest path algortihm computing the shortest beer path in maximal outerplanar graph in O(L) - number of vertices on the shortest path

## Run shortest (beer) path
- Run ```main.cpp``` file from `src` directory (stay in `dev` directory)
```
g++ -std=c++17 utils/utils.cpp utils/graph/*.cpp utils/preprocess/*.cpp utils/outerplanar.cpp utils/verification/*.cpp test/*.cpp src/*.cpp -o main.out || ./main.out

./main.out
```

## Run visualization
- Run ```visualize.py``` file from `visual` directory (stay in `visual` directory)
```
g++ -std=c++17 -shared -fPIC $(python3 -m pybind11 --include) ../utils/*.cpp ../utils/graph/*.cpp ../utils/preprocess/*.cpp ../src/dag.cpp ../src/sssp.cpp bind.cpp -o graph_cpp$(python3-config --extension-suffix)

python3 visualize.py
```
