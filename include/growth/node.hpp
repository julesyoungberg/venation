#pragma once

#include <memory>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

namespace growth {

typedef std::shared_ptr<struct node> node_ref;

struct node {

    using kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
    using point2 = kernel::Point_2;

    node(const point2& p, double base_width = 0.1)
        : position(p), base_width(base_width), width(base_width) {}

    ~node() = default;

    double update_width();

    static node_ref create(const point2& p) {
        return std::make_shared<node>(p);
    }

    std::vector<node_ref> children;
    point2 position;
    double width;
    double base_width;

};

}
