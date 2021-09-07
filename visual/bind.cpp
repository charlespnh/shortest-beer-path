#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace std;

#include "../include/outerplanar.h"
#include "../include/graph/dcel.h"
#include "../include/dag.h"
// #include "../include/sssp.h"


namespace py = pybind11;

// module 
PYBIND11_MODULE(graph_cpp, m) {
	py::class_<vertex>(m, "vertex")
		.def(py::init<>())
		.def_readwrite("data", &vertex::data)
		.def_readwrite("beer", &vertex::is_beer)
		.def_readwrite("coord", &vertex::point)
		.def_readwrite("dist", &vertex::dist)
		.def_readwrite("distB", &vertex::distB)
		.def_readwrite("adjacency", &vertex::adj);

	py::class_<halfedge>(m, "edge")
		.def(py::init<>())
		.def_readwrite("weight", &halfedge::weight)
		.def_readwrite("twin", &halfedge::twin)
		.def_readwrite("next", &halfedge::next)
		.def_readwrite("prev", &halfedge::prev)
		.def_readwrite("target", &halfedge::target);



	py::class_<graph>(m, "graph")
		.def(py::init<int, float>())
		.def("build_dual", &graph::build_dual)
		.def("build_graph", &graph::build_polygon)
		.def("preprocess", &graph::preprocess_graph)

		.def("get_vertex", &graph::get_vertex, py::return_value_policy::reference)
		.def_static("get_edge", &graph::get_edge, py::return_value_policy::reference)

		.def_readwrite("root_edge", &graph::mroot_edge);



	py::class_<dag>(m, "dag")
		.def(py::init<>())
		.def("build", &dag::build_dag)
		.def("reset", &dag::reset)
		.def("shortest_path", &dag::shortest_path_dag)
		.def("shortest_beer_path", &dag::shortest_beer_path_dag)
		.def("print_shortest_path", &dag::print_spsp_dag, py::return_value_policy::reference)
		.def("print_shortest_beer_path", &dag::print_spsbp_dag, py::return_value_policy::reference)

		.def_readwrite("mDAG", &dag::mDAG);
}