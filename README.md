# Shortest Beer Path on Maximal Outerplanar Graph

A C++ implementation of shortest path algortihm computing the shortest beer path in maximal outerplanar graph in O(L) - number of vertices on the shortest path

## Run Elaine's modified LCA algorithm
- Run ```test.cpp``` file
```
g++ -std=c++11 utils/btree.cpp utils/lca.cpp test/test.cpp -o main.out || ./main.out
./main.out
```

- Run ```sample_usage.cpp``` file
```
g++ -std=c++11 utils/btree.cpp utils/lca.cpp test/sample_usage.cpp -o main.out || ./main.out
./main.out
```

## Run shortest path
- Run ```main.cpp``` file
```
g++ -std=c++17 utils/btree.cpp utils/lca.cpp utils/dcel.cpp src/sp_dag.cpp src/main.cpp -o main.out || ./main.out
```
