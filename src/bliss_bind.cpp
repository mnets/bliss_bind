#include <functional>
#include <vector>
#include <tuple>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/function.h>


#include <abstractgraph.hh>
#include <graph.hh>
#include <digraph.hh>

namespace nb = nanobind;

using namespace nb::literals;

template <typename T>
void abstract_graph_methods(nb::class_<T>& c) {
  c.def("add_edge", &T::add_edge, "source"_a, "target"_a)
   .def("add_vertex", &T::add_vertex, "color"_a = 0)
   .def("canonical_form", [](
            T& self, const std::function<void(const std::vector<unsigned int>&)>& callback){
          std::function<void(unsigned int, const unsigned int*)> report_raw = nullptr;
          if (callback != nullptr)
            report_raw = [&callback](
                unsigned int n, const unsigned int* aut){
              callback(std::vector<unsigned int>(aut, aut + n));
            };
          bliss::Stats stats;
          auto b = self.canonical_form(stats, report_raw);
          return std::make_tuple(std::vector(b, b + self.get_nof_vertices()), stats);
       }, "callback"_a = nb::none())
   .def("change_color", &T::change_color, "vertex"_a, "color"_a)
   .def("cmp", &T::cmp, "other"_a)
   .def("copy", &T::copy)
   .def("find_automorphisms", [](
            T& self, const std::function<void(const std::vector<unsigned int>&)>& callback){
          auto report_raw = [&callback](unsigned int n, const unsigned int* aut) {
            callback(std::vector<unsigned int>(aut, aut + n));
          };
          bliss::Stats stats;
          self.find_automorphisms(stats, report_raw);

          return stats;
        }, "callback"_a)
   .def("get_color", &T::get_color, "vertex"_a)
   .def("get_hash", &T::get_hash)
   .def("get_nof_vertices", &T::get_nof_vertices)
   .def("is_automorphism", 
         nb::overload_cast<
           const std::vector<unsigned int>&>(
             &T::is_automorphism, nb::const_), "aut"_a)
   .def("permute",
         nb::overload_cast<
           const std::vector<unsigned int>&>(
             &T::permute, nb::const_), "aut"_a)
   .def("set_component_recursion", &T::set_component_recursion, "active"_a)
   .def("set_failure_recording", &T::set_failure_recording, "active"_a)
   .def("set_long_prune_activity", &T::set_long_prune_activity, "active"_a)
   .def("set_splitting_heuristic", &T::set_splitting_heuristic, "shs"_a)
   .def("set_verbose_level", &T::set_verbose_level, "level"_a)
   ;
}

NB_MODULE(bliss_bind_ext, m) {
  nb::class_<bliss::Stats>(m, "Stats")
    .def(nb::init<>())
    .def("get_group_size_approx", &bliss::Stats::get_group_size_approx)
    .def("get_nof_nodes", &bliss::Stats::get_nof_nodes)
    .def("get_nof_leaf_nodes", &bliss::Stats::get_nof_leaf_nodes)
    .def("get_nof_bad_nodes", &bliss::Stats::get_nof_bad_nodes)
    .def("get_nof_canupdates", &bliss::Stats::get_nof_canupdates)
    .def("get_nof_generators", &bliss::Stats::get_nof_generators)
    .def("get_max_level", &bliss::Stats::get_max_level)
    ;

    auto graph = nb::class_<bliss::Graph>(m, "Graph")
      .def(nb::init<>())
      .def(nb::init<const unsigned int>());
    abstract_graph_methods<bliss::Graph>(graph);
    nb::enum_<bliss::Graph::SplittingHeuristic>(graph, "SplittingHeuristic")
      .value("shs_f", bliss::Graph::SplittingHeuristic::shs_f)
      .value("shs_fs", bliss::Graph::SplittingHeuristic::shs_fs)
      .value("shs_fl", bliss::Graph::SplittingHeuristic::shs_fl)
      .value("shs_fm", bliss::Graph::SplittingHeuristic::shs_fm)
      .value("shs_fsm", bliss::Graph::SplittingHeuristic::shs_fsm)
      .value("shs_flm", bliss::Graph::SplittingHeuristic::shs_flm)
    ;
    

    auto digraph = nb::class_<bliss::Digraph>(m, "Digraph")
      .def(nb::init<>())
      .def(nb::init<const unsigned int>());
    abstract_graph_methods<bliss::Digraph>(digraph);
    nb::enum_<bliss::Digraph::SplittingHeuristic>(digraph, "SplittingHeuristic")
      .value("shs_f", bliss::Digraph::SplittingHeuristic::shs_f)
      .value("shs_fs", bliss::Digraph::SplittingHeuristic::shs_fs)
      .value("shs_fl", bliss::Digraph::SplittingHeuristic::shs_fl)
      .value("shs_fm", bliss::Digraph::SplittingHeuristic::shs_fm)
      .value("shs_fsm", bliss::Digraph::SplittingHeuristic::shs_fsm)
      .value("shs_flm", bliss::Digraph::SplittingHeuristic::shs_flm)
    ;
}
