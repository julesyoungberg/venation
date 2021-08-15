#pragma once

#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/squared_distance_2.h>

namespace util {

    using kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using point2 = kernel::Point_2;
    using vector2 = kernel::Vector_2;

    inline vector2 normalize(const vector2& p) {
        return p / std::sqrt(p.squared_length());
    }

    inline kernel::FT distance(const point2& a, const point2& b) {
        return std::sqrt(CGAL::squared_distance(a, b));
    }

}
