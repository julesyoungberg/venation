#pragma once

#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/property_map.h>

#include "glm/glm.hpp"

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_2                                     Point_2;
typedef boost::tuple<Point_2, int>                          Point_and_int;
typedef CGAL::Search_traits_2<Kernel>                       Traits_base;
typedef CGAL::Search_traits_adapter<Point_and_int,
  CGAL::Nth_of_tuple_property_map<0, Point_and_int>,
  Traits_base>                                              Traits;
typedef CGAL::Orthogonal_k_neighbor_search<Traits>          K_neighbor_search;
typedef K_neighbor_search::Tree                             Tree;
typedef K_neighbor_search::Distance                         Distance;

class App {
    public:
        App() = default;
        ~App() = default;

        void setup();
        void draw();

    private:
        void generate_attractors();
        void draw_attractors();

        std::vector<Point_2> attractors_;
        std::vector<int> attractors_indices_;
        Tree attractors_tree_;
};

