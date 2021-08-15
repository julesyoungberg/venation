#pragma once

#include <memory>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

namespace growth {

    typedef std::shared_ptr<struct node> node_ref;

    /**
     * Structure for representing a node that represents whatever
     * is growing in the simulation. It can be considered as the veines
     * or the tree that is being generated.
     */
    struct node {
        // types from CGAL for representing the points with robust numerical predicates.
        using kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
        using point2 = kernel::Point_2;

        // trivial constructor and destructor
        node(const point2& p, double base_width = 0.1)
            : position(p), base_width(base_width), width(base_width) {}

        ~node() = default;

        /**
         * Computes the width the structure should be at the node's point
         * as the recursive sum of its childrens' widths.
         * Returns the computed width.
         */
        double update_width();

        /**
         * Creates a node at the given point.
         * Returns a smart pointer that references it.
         */
        static node_ref create(const point2& p);

        // data members
        std::vector<node_ref> children;
        point2 position;
        double width;
        double base_width;

    };

}
