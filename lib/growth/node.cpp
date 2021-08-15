#include <cmath>

#include "growth/node.hpp"

/**
 * Update width by traversing the tree and computing the width
 * of each node's children.
 */
double growth::node::update_width() {
    if (children.size() == 0) {
        // if no children we are a leaf
        width = base_width;
    } else if (children.size() == 1) {
        // if only one child we just inherit its width
        width = children[0]->update_width();
    } else {
        // recursively sum the children's widths
        double sum = 0.0;
        for (const auto& child : children) {
            sum += std::pow(child->update_width(), 3.0);
        }
        width = std::cbrt(sum);
    }

    return width;
}

growth::node_ref growth::node::create(const growth::node::point2& p) {
    return std::make_shared<growth::node>(p);
}
