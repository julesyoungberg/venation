#pragma once

#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include "node.hpp"

typedef CGAL::Exact_predicates_inexact_constructions_kernel kernel;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned int, kernel> vertex_index;
typedef CGAL::Triangulation_data_structure_2<vertex_index> vertex_index_data_structure;
typedef kernel::Point_2 point2;
typedef kernel::Vector_2 vector2;
typedef CGAL::Delaunay_triangulation_2<kernel>  delaunay;
typedef CGAL::Delaunay_triangulation_2<kernel, vertex_index_data_structure> delaunay_indexed;

class App {
    public:
        App() = default;
        ~App() = default;

        void setup();
        void update();
        void draw();

    private:
        void generate_attractors();
        void create_seed();
        void draw_attractors();
        void draw_nodes();

        delaunay attractors_graph_;
        
        std::vector<node_ref> nodes_;
        delaunay_indexed nodes_graph_;

};

