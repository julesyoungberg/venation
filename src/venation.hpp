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
        using kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
        using vertex_index = CGAL::Triangulation_vertex_base_with_info_2<
            unsigned int,
            kernel
        >;
        using vertex_index_data_structure = CGAL::Triangulation_data_structure_2<
            vertex_index
        >;
        using point2 = kernel::Point_2;
        using vector2 = kernel::Vector_2;
        using delaunay = CGAL::Delaunay_triangulation_2<kernel>;
        using delaunay_indexed = CGAL::Delaunay_triangulation_2<
            kernel, 
            vertex_index_data_structure
        >;
        using attractor_handle = delaunay::Vertex_handle;
        using node_handle = delaunay_indexed::Vertex_handle;
        using node_circulator = delaunay_indexed::Vertex_circulator;

        enum type { open, closed };

        venation(type mode = type::open): mode_(mode) {}
        ~venation() = default;

        void generate_attractors();
        void create_seeds();
        void update();
        void draw_attractors();
        void draw_nodes();

        venation& configure(unsigned int width, unsigned int height);
        venation& seeds(const std::vector<point2>& seeds);
        venation& num_attractors(unsigned int n) { num_attractors_ = n; return *this; }
        venation& mode(type mode) { mode_ = mode; return *this; }
        venation& mode(const std::string& m);
        venation& growth_radius(long double r) { growth_radius_ = r; return *this; }
        venation& growth_rate(long double r) { growth_rate_ = r; return *this; }
        venation& consume_radius(long double r) { consume_radius_ = r; return *this; }
        venation& mask_data(const std::vector<float>& d) { mask_data_ = d; return *this; }

        delaunay& get_attractors() { return attractors_graph_; }
        std::vector<node_ref>& get_nodes() { return nodes_; }

    private:
        std::ptrdiff_t insert_node(const point2&);
        void grow(const std::map<unsigned int, vector2>&);
        bool has_consumed(unsigned int, const point2&);
        void open_step();
        void closed_step();

        type mode_;

        delaunay attractors_graph_;

        std::vector<point2> seeds_;
        std::vector<node_ref> nodes_;
        delaunay_indexed nodes_graph_;

        unsigned int width_ = 512;
        unsigned int height_ = 512;
        long double aspect_ratio_ = 1.0;
        unsigned int num_attractors_ = 5000;
        long double growth_radius_ = 0.1;
        long double growth_rate_ = 0.002;
        long double consume_radius_ = 0.0005;

        std::vector<float> mask_data_;

};

