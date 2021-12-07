# Shortest Beer Path on Maximal Outerplanar Graph

The project is a faithful attempt to explain & implement shortest (beer) path in maximal outerplanar graph in the work of J.Bacic, S.Mehrabi and M.Smid [https://arxiv.org/abs/2110.15693]. 

The implementation is mainly on the second half of the paper, i.e computing shortest (beer) path queries and single-source shortest (beer) path on maximal outerplanar beer graph G of size O(n). For queries vertices u, v where |SPB(u, v)| = L; the paper achieves the preprocessing and query time of ```<P(n), Q(n)> = <O(n), O(L)>```

![alt text](https://github.com/charlespnh/shortest-beer-path/blob/master/report/preprocessing.png)

## Hardware
- Window Subsystem for Linux 2 - Ubuntu 20.04
- CPU Intel Core i7-8565U @ 8x 1.992GHz
- RAM 5.5GiB
- Cache 156MiB

- gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)

## Run shortest (beer) path algorithm
### Requirement:
- The requirement is mainly regarding memory to ensure that you can comfortably run the code on any (reasonable) size of the graph.
  + ulimit -s 64000000 (gives enough space for stack due to recursion)
  + free -h            (check if there's at least 2 GiB in memory)
- ```main.cpp``` file in `src` directory (compile & execute from parent directory)
```
g++ -std=c++17 utils/utils.cpp utils/graph/*.cpp utils/preprocess/*.cpp utils/outerplanar.cpp utils/verification/*.cpp test/*.cpp src/*.cpp -o main.out || ./main.out

./main.out
```

## Run visualization
- ```visualize.py``` file in `visual` directory (compile & execute from `visual` directory)
```
g++ -std=c++17 -shared -fPIC $(python3 -m pybind11 --include) ../utils/*.cpp ../utils/graph/*.cpp ../utils/preprocess/*.cpp ../src/dag.cpp ../src/sssp.cpp bind.cpp -o graph_cpp$(python3-config --extension-suffix)

python3 visualize.py
```
