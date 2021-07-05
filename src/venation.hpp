#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include "node.hpp"

class venation {
    public:
        typedef CGAL::Exact_predicates_inexact_constructions_kernel kernel;
        typedef CGAL::Triangulation_vertex_base_with_info_2<
            unsigned int,
            kernel
        > vertex_index;
        typedef CGAL::Triangulation_data_structure_2<
            vertex_index
        > vertex_index_data_structure;
        typedef kernel::Point_2 point2;
        typedef kernel::Vector_2 vector2;
        typedef CGAL::Delaunay_triangulation_2<kernel>  delaunay;
        typedef CGAL::Delaunay_triangulation_2<
            kernel, 
            vertex_index_data_structure
        > delaunay_indexed;
        typedef delaunay::Vertex_handle attractor_handle;
        typedef delaunay_indexed::Vertex_handle node_handle;
        typedef delaunay_indexed::Vertex_circulator node_circulator;

        enum type { open, closed };

        venation(type mode = type::open): mode_(mode) {}
        ~venation() = default;

        delaunay& get_attractors() { return attractors_graph_; }
        std::vector<node_ref>& get_nodes() { return nodes_; }

        void set_mode(type mode) { mode_ = mode; }

        void configure(unsigned int width, unsigned int height,
            unsigned int num_attractors, std::string mode, 
            long double growth_radius, long double growth_rate,
            long double consume_radius);

        void generate_attractors();
        void create_seed();
        void update();
        void draw_attractors();
        void draw_nodes();

    private:
        std::ptrdiff_t insert_node(const point2&);
        void grow(const std::map<unsigned int, vector2>&);
        bool has_consumed(unsigned int, const point2&);
        void open_step();
        void closed_step();

        type mode_;

        delaunay attractors_graph_;

        std::vector<node_ref> nodes_;
        delaunay_indexed nodes_graph_;

        unsigned int width_ = 512;
        unsigned int height_ = 512;
        unsigned int num_attractors_ = 5000;
        long double growth_radius_ = 0.1;
        long double growth_rate_ = 0.002;
        long double consume_radius_ = 0.0005;

};

