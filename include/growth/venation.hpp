#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include "node.hpp"

namespace growth {

    /*
     * A class simulating venation growth using a space colonization algorithm.
     * It supports both open and closed venation styles, and has a number
     * of configurable parameters.
     */
    class venation {
        public:

            // types from CGAL used for math
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

            // the types of venation
            enum type { open, closed };

            // trivial constructor and destructor
            venation(type mode = type::open): mode_(mode) {}
            ~venation() = default;

            /**
             * Scales the simulation to fit within the provided with & height.
             * If a mask is given, it will also be scaled down.
             * Must be called before setup().
             */
            void scale_to_fit(int window_width, int window_height);

            /**
             * Initializes the simulation's state by sampling the mask if present,
             * then generating the attractors as well as the seeds.
             */
            void setup();

            /**
             * Updates the simulation, equivalent to a single timestep.
             */
            void update();

            /**
             * Draw the attractors to the screen. Useful for debugging.
             */
            void draw_attractors();

            /**
             * Draw the growth nodes to the screen.
             */
            void draw_nodes();

            /**
             * Sets the width and height of the simulation.
             */
            venation& configure(unsigned int width, unsigned int height);

            // setters
            venation& seeds(const std::vector<point2>& seeds);
            venation& num_attractors(unsigned int n) { num_attractors_ = n; return *this; }
            venation& mode(type mode) { mode_ = mode; return *this; }
            venation& mode(const std::string& m);
            venation& growth_radius(long double r) { growth_radius_ = r; return *this; }
            venation& growth_rate(long double r) { growth_rate_ = r; return *this; }
            venation& consume_radius(long double r) { consume_radius_ = r; return *this; }
            venation& mask_shades(unsigned int n) { mask_shades_ = n; return *this; }
            venation& mask(const boost::gil::rgb8_image_t& img);

            // getters
            delaunay& attractors() { return attractors_graph_; }
            std::vector<node_ref>& nodes() { return nodes_; }
            unsigned int width() { return width_; }
            unsigned int height() { return height_; }

        private:

            void prepare_mask();
            void generate_attractors();
            void create_seeds();
            
            long double growth_radius();
            long double growth_rate();

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
            unsigned int num_attractors_ = 1000;
            long double growth_radius_ = 0.5;
            long double growth_rate_ = 0.002;
            long double consume_radius_ = 0.002;
            unsigned int mask_shades_ = 2;
            bool mask_given_ = false;
            int no_growth_count_ = 0;

            boost::gil::rgb8_image_t mask_img_;
            std::vector<float> mask_data_;

    };

}
