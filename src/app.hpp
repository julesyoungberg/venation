#pragma once

#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include "glm/glm.hpp"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Delaunay_triangulation_2<K>  DT2;

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
        DT2 dt2;

};

