#include <cmath>

#include "growth/node.hpp"

/**
 * Update width by traversing the tree and computing the width
 * of each node's children.
 */
double growth::node::update_width() {
    if (children.size() == 0) {
        width = base_width;
    } else if (children.size() == 1) {
        width = children[0]->update_width();
    } else {
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
