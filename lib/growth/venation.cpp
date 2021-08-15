#include <algorithm>
#include <iostream>
#include <map>

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "growth/venation.hpp"
#include "img.hpp"
#include "util.hpp"

using namespace growth;

venation& venation::configure(unsigned int width, unsigned int height) {
    width_ = width;
    height_ = height;
    aspect_ratio_ = double(width) / double(height);
    return *this;
}

venation& venation::seeds(const std::vector<venation::point2>& seeds) {
    seeds_.clear();
    for (const auto& seed : seeds) {
        seeds_.push_back(venation::point2(seed.x() * aspect_ratio_, seed.y()));
    }

    return *this;
}

venation& venation::mode(const std::string& mode) {
    if (mode.compare("closed") == 0) {
        mode_ = venation::type::closed;
    } else {
        mode_ = venation::type::open;
    }

    return *this;
}

venation& venation::mask(const boost::gil::rgb8_image_t& img) {
    mask_img_ = img;
    mask_given_ = true;
    // Reconfigure. The input image's dimensions trump any configuration.
    configure(mask_img_.width(), mask_img_.height());
    return *this;
}

void venation::scale_to_fit(int window_width, int window_height) {
    auto width = width_;
    auto height = height_;
    
    // scale based on width if needed
    if (window_width < width) {
        width = window_width;
        height = (unsigned int)((double)width / aspect_ratio_);
    }

    // scale based on height if needed
    if (window_height < height) {
        height = window_height;
        width = (unsigned int)((double)height * aspect_ratio_);
    }

    // if the size has changed reconfigure and resize image
    if (width != width_ || height != height_) {
        boost::gil::rgb8_image_t new_mask_img(width, height);
        boost::gil::resize_view(const_view(mask_img_), view(new_mask_img), 
            boost::gil::bilinear_sampler());
        mask_img_ = new_mask_img;
        configure(width, height);
    }
}

/**
 * Converts the image to a vector representation for
 * easy lookup during the simulation.
 */
void venation::prepare_mask() {
    if (!mask_given_) {
        return;
    }

    mask_data_.clear();
    mask_data_.reserve(mask_img_.width() * mask_img_.height());

    // convert the image to black and white
    boost::gil::for_each_pixel(
        boost::gil::const_view(mask_img_), 
        img::BlackAndWhitePixelInserter(&mask_data_, mask_shades_)
    );
}

/**
 * Generates an initial set of attractors
 */
void venation::generate_attractors() {
    double x;
    double y;
    std::vector<venation::point2> attractors;

    // generate random points
    for (int i = 0; i < num_attractors_; ++i) {
        x = (util::rnd() * 2.0 - 1.0) * aspect_ratio_;
        y = util::rnd() * 2.0 - 1.0;
        venation::point2 p(x, y);

        if (mask_data_.size() == 0) {
            attractors.push_back(p);
        } else {
            // Keep the attractor based on the brightness of the
            // mask's corresponding pixel as probability.
            int ix = (int)round((x * 0.5 + 0.5) * (float)width_);
            int iy = (int)round((y * 0.5 + 0.5) * (float)height_);
            float brightness = mask_data_[(height_ - iy) * width_ + ix];
            if (util::rnd() < brightness) {
                attractors.push_back(p);
            }
        }
    }

    // add them to delaunay triangulation graph
    attractors_graph_.insert(attractors.begin(), attractors.end());
}

/**
 * Inserts a single point into the nodes graph.
 */
std::ptrdiff_t venation::insert_node(const venation::point2& p) {
    // vertex with info needs to be inserted to the graph via vector
    std::vector<std::pair<venation::point2, unsigned int>> nodes{
        std::make_pair(p, nodes_.size())
    };
    return nodes_graph_.insert(nodes.begin(), nodes.end());
}

/*
 * Seed the growth.
 */
void venation::create_seeds() {
    if (seeds_.size() == 0) {
        seeds_.push_back(venation::point2(0.0, 0.0));
    }

    nodes_.clear();

    for (const auto& seed : seeds_) {
        // insert node to dilaunay graph
        insert_node(seed);
        // add the node to the node index vector.
        auto dir = util::normalize(venation::vector2(util::rnd(), util::rnd()));
        nodes_.push_back(node::create(seed, dir));
    }
}

/**
 * Returns the current growth radius as the base growth radius
 * multiplied by the 2 to the no grwoth count.
 */
long double venation::growth_radius() {
    return growth_radius_ * pow(2.0, no_growth_count_);
}

/**
 * Returns the current growth rate as the base growth rate
 * multiplied by the 2 to the no grwoth count.
 */
long double venation::growth_rate() {
    return growth_rate_ * pow(2.0, no_growth_count_);
}

void venation::setup() {
    prepare_mask();
    generate_attractors();
    create_seeds();
}

/**
 * Performs the node growth (colonization) step of the algorithm.
 */
void venation::grow(const std::map<unsigned int, venation::vector2>& influences) {
    if (influences.size() == 0) {
        ++no_growth_count_;
        return;
    }

    bool has_grown = false;
    
    std::vector<std::pair<venation::point2, unsigned int>> new_points;
    
    for (const auto& i : influences) {
        node_ref parent = nodes_[i.first];

        // 3. util::normalize each vector sum
        auto d = util::normalize(i.second);
        auto diff = parent->direction - d * -1.0;

        // if the growth direction is the inverse of the previous growth
        // direction we must just continue on, we cannot grow backwards
        if (std::abs(diff.x()) < 0.01 && std::abs(diff.y()) < 0.01) {
            d = parent->direction;
        }

        // 4. add new node
        auto step = d * growth_rate();
        venation::point2 child_pos(
            parent->position.x() + step.x(),
            parent->position.y() + step.y()
        );

        // check if node already exists
        bool exists = false;
        for (const auto& c : parent->children) {
            if (c->position == child_pos) {
                exists = true;
                break;
            }
        }

        if (exists) {
            continue;
        }

        // node does not exist, add it to grow the structure
        auto dir = util::normalize(child_pos - parent->position);
        auto child_node = node::create(child_pos, dir);
        new_points.push_back(std::make_pair(child_pos, nodes_.size()));
        parent->children.push_back(child_node);
        nodes_.push_back(child_node);
        has_grown = true;
    }

    if (has_grown) {
        no_growth_count_ = std::max(0, no_growth_count_ - 1);
        nodes_graph_.insert(new_points.begin(), new_points.end());
    } else {
        ++no_growth_count_;
    }
}

/**
 * Checks if node with id node_id is or any of its children are
 * within the kill radius of attractor at point s.
 */
bool venation::has_consumed(unsigned int node_id, const venation::point2& s) {
    node_ref node = nodes_[node_id];

    // check if the node is within kill util::distance
    if (util::distance(node->position, s) < consume_radius_) {
        return true;
    }

    // otherwise, check if any childen are within kill util::distance
    for (const auto& child : node->children) {
        if (util::distance(s, child->position) < consume_radius_) {
            return true;
        }
    }

    return false;
}

/**
 * Performs a single iteration of the open venation algorithm.
 */
void venation::open_step() {
    // find the closest node to each attractor.
    std::map<unsigned int, venation::vector2> influences;
    std::vector<venation::attractor_handle> influencing_attractors;

    for (auto it = attractors_graph_.finite_vertices_begin();
            it != attractors_graph_.finite_vertices_end(); ++it) {
        // 1. associate every attractor with a growth node
        auto attractor = it->point();
        auto vertex = nodes_graph_.nearest_vertex(attractor);
        auto point = vertex->point();
        auto index = vertex->info();
        auto dist = util::distance(attractor, point);

        if (dist > consume_radius_ * 0.01 && dist < growth_radius()) {
            influencing_attractors.push_back(it);
            // 2. sum the difference vectors for each node
            venation::vector2 d = util::normalize(attractor - point) / dist;
            auto l = influences.find(index);
            if (l == influences.end()) {
                influences[index] = d;
            } else {
                influences[index] = l->second + d;
            }
        }
    }

    // 3 - 4
    grow(influences);

    // 5. remove attractors that have been consumed
    for (const auto& a : influencing_attractors) {
        auto vertex = nodes_graph_.nearest_vertex(a->point());
        auto dst = util::distance(a->point(), vertex->point());

        if (dst < 0.001) {
            attractors_graph_.remove(a);
        }
    }
}

/**
 * Performs a single step of the closed venation algorithm.
 */
void venation::closed_step() {
    // 1. associate every attractor with the nearest growth nodes
    std::map<unsigned int, venation::vector2> influences;
    std::vector<
        std::pair<venation::attractor_handle, std::vector<unsigned int>>
    > influencing_attractors;
    // find the relative neighborhood of each attractor
    for (auto it = attractors_graph_.finite_vertices_begin();
            it != attractors_graph_.finite_vertices_end(); ++it) {
        auto s = it->point();
        // add s to the nodes graph to quickly find neighbors
        std::ptrdiff_t n = insert_node(s);
        // auto s_handle = nodes_graph_.finite_vertices_end() - n;
        auto s_handle = nodes_graph_.nearest_vertex(s);

        // get the neighbors for comparison
        std::vector<node_handle> adjacent;
        auto nc = nodes_graph_.incident_vertices(s_handle);
        auto done(nc);
        
        if (nc == 0) {
            nodes_graph_.remove(s_handle);
            continue;
        }
        
        do {
            adjacent.push_back(nc);
        } while (++nc != done);

        // find which neighbors are in the relative neighborhood
        // and influence all of them
        std::vector<unsigned int> influenced_node_ids;
        for (const auto& v_handle : adjacent) {
            auto v = v_handle->point();
            auto v_s = util::distance(v, s);
            bool valid = true;
            
            // point v is in the relative neighborhood if
            // (u in V) ||v - s|| < max{||u - s||, ||v - u||}
            for (const auto& u_handle : adjacent) {
                auto u = u_handle->point();
                if (u == v) {
                    continue;
                }

                auto u_s = util::distance(u, s);
                auto v_u = util::distance(v, u);
                
                if (v_s >= std::max(u_s, v_u)) {
                    valid = false;
                    break;
                }
            }

            if (!valid) {
                continue;
            }

            if (v_s > consume_radius_ * 0.01 && v_s < growth_radius()) {
                // 2. sum the difference vectors for each node
                influenced_node_ids.push_back(v_handle->info());
                venation::vector2 d = util::normalize(s - v) / v_s;
                auto id = v_handle->info();
                auto l = influences.find(id);
                if (l == influences.end()) {
                    influences[id] = d;
                } else {
                    influences[id] = l->second + d;
                }
            }
        }

        nodes_graph_.remove(s_handle);

        if (influenced_node_ids.size() > 0) {
            influencing_attractors.push_back(
                std::make_pair(it, influenced_node_ids)
            );
        }
    }

    // 3 - 4
    grow(influences);
    
    // 5. remove attractors that have been consumed
    for (const auto& pair : influencing_attractors) {
        auto s = pair.first->point();
        bool consumed = true;
        
        // check if every influenced node reached the attractor
        for (const auto id : pair.second) {
            if (!has_consumed(id, s)) {
                consumed = false;
                break;
            }
        }

        // if consumed, remove the attractor
        if (consumed) {
            attractors_graph_.remove(pair.first);  

            // connect the two influenced nodes
            if (pair.second.size() == 2) {
                auto& first = nodes_[pair.second[0]];
                auto& second = nodes_[pair.second[1]];

                if (first->children.size() > 0) {
                    auto& last_child = first->children[first->children.size() - 1];
                    auto dir = util::normalize(second->position - last_child->position);
                    auto child_node = node::create(second->position, dir);
                    last_child->children.push_back(child_node);
                } else if (second->children.size() > 0) {
                    auto& last_child = second->children[second->children.size() - 1];
                    auto dir = util::normalize(first->position - last_child->position);
                    auto child_node = node::create(first->position, dir);
                    last_child->children.push_back(child_node);
                } else {
                    auto dir = util::normalize(second->position - first->position);
                    auto child_node = node::create(second->position, dir);
                    first->children.push_back(child_node);
                }
            }  
        }
    }
}    

void venation::update() {
    if (mode_ == venation::type::open) {
        open_step();
    } else if (mode_ == venation::type::closed) {
        closed_step();
    }

    for (unsigned i = 0; i < seeds_.size(); ++i) {
        std::vector<node_ref> removed = nodes_[i]->optimize();
        
        // remove pruned nodes from node graph
        for (const auto& n : removed) {
            auto vertex = nodes_graph_.nearest_vertex(n->position);
            auto point = vertex->point();

            if (point == n->position) {
                nodes_graph_.remove(vertex);
            }
        }

        nodes_[i]->update_width();
    }
}

void venation::draw_attractors() {
    glPointSize(5.0f);
    glBegin(GL_POINTS); 
        glColor3f(1.0f, 0.0f, 0.0f);
        for (auto it = attractors_graph_.finite_vertices_begin();
                it != attractors_graph_.finite_vertices_end(); ++it) {
            auto v = it->point();
            glVertex2d(v.x() / aspect_ratio_, v.y());
        }
    glEnd();
}

void venation::draw_nodes() {
    // start at each seed
    for (unsigned i = 0; i < seeds_.size(); ++i) {
        // initialize a stack of nodes
        std::vector<node_ref> to_visit;
        to_visit.push_back(nodes_[i]);

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(3.0f);

        // while the stack is not empty
        while (to_visit.size() > 0) {
            // pop from the stack
            auto node = to_visit.back();
            to_visit.pop_back();

            // visit each of the node's children
            for (auto child : node->children) {
                // push them to the stack
                to_visit.push_back(child);

                // draw a line from the parent to the child
                glLineWidth(child->width * 3.0);
                glBegin(GL_LINES);
                    glVertex2d(node->position.x() / aspect_ratio_, node->position.y());
                    glVertex2d(child->position.x() / aspect_ratio_, child->position.y());
                glEnd();
            }
        }
    }
}
