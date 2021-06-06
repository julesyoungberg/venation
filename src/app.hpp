#pragma once

#include <vector>

// choose the kernel
#include <CGAL/Simple_cartesian.h>

typedef CGAL::Simple_cartesian<double> K;

// typedefs for the traits and the algorithm
#include <CGAL/Segment_Delaunay_graph_hierarchy_2.h>
#include <CGAL/Segment_Delaunay_graph_filtered_traits_2.h>

typedef CGAL::Segment_Delaunay_graph_filtered_traits_2<K> Gt;

typedef CGAL::Segment_Delaunay_graph_hierarchy_2<Gt> SDG2;

#include "glm/glm.hpp"

class App {
    public:
        App() = default;
        ~App() = default;

        void setup();
        void draw();

    private:
        void generate_attractors();
        void draw_attractors();

        std::vector<Gt::Point_2> attractors_;
        SDG2 attractors_sdg_;

};

