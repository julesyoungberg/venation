#pragma once

#include <memory>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef std::shared_ptr<struct node> node_ref;

struct node {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel kernel;
    typedef kernel::Point_2 point2;

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

