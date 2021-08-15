#include <cmath>

#include "growth/node.hpp"
#include "util.hpp"

std::vector<growth::node_ref> growth::node::optimize() {
    std::vector<growth::node_ref> removed;

    if (children.size() == 0) {
        // if there are no children no optimization needed
        return removed;
    }

    // for each child, remove any nodes with a single child
    for (int i = 0; i < children.size(); ++i) {
        auto& child = children[i];
        auto direction = util::normalize(child->position - position);
        auto& current = child;

        // step through the children as long as it is a straight line.
        while (current->children.size() == 1) {
            auto& next = current->children[0];
            auto next_direction = util::normalize(next->position - position);

            if (next_direction != direction) {
                break;
            }

            removed.push_back(current);
            current = next;
        }

        children[i] = current;
    }

    return removed;
}

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
