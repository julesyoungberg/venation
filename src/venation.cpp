#include <algorithm>
#include <iostream>
#include <map>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include <CGAL/squared_distance_2.h>
#include <GLFW/glfw3.h>

#include "venation.hpp"

double rnd() {
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

/**
 * Generates an initial set of attractors
 */
void venation::generate_attractors() {
    constexpr int num_attractors = 5000;
    double x;
    double y;
    std::vector<point2> attractors(num_attractors);

    // generate random points
    for (int i = 0; i < num_attractors; ++i) {
        x = rnd() * 2.0 - 1.0;
        y = rnd() * 2.0 - 1.0;
        attractors[i] = point2(x, y);
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
void venation::create_seed() {
    point2 seed_point(0.0, 0.0);

    insert_node(seed_point);

    // add the node to the node index vector.
    node_ref seed_node = node::create(seed_point);
    nodes_.push_back(seed_node);
}

venation::vector2 normalize(const venation::vector2& p) {
    return p / std::sqrt(p.squared_length());
}

venation::kernel::FT distance(const venation::point2& a, 
        const venation::point2& b) {
    return std::sqrt(CGAL::squared_distance(a, b));
}

/**
 * Performs the node growth (colonization) step of the algorithm.
 */
void venation::grow(const std::map<unsigned int, venation::vector2>& influences) {
    std::vector<std::pair<venation::point2, unsigned int>> new_points;
    
    for (const auto& i : influences) {
        // 3. normalize each vector sum
        auto d = normalize(i.second);

        // 4. add new node
        node_ref parent = nodes_[i.first];
        vector2 step = d * 0.002;
        point2 child_pos = point2(
            parent->position.x() + step.x(),
            parent->position.y() + step.y()
        );
        node_ref child_node = node::create(child_pos);
        new_points.push_back(std::make_pair(child_pos, nodes_.size()));
        parent->children.push_back(child_node);
        nodes_.push_back(child_node);
    }

    nodes_graph_.insert(new_points.begin(), new_points.end());
}

/**
 * Checks if node with id node_id is or any of its children are
 * within the kill radius of attractor at point s.
 */
bool venation::has_consumed(unsigned int node_id, const venation::point2& s) {
    node_ref node = nodes_[node_id];
    double kill_dist = 0.0005;

    // check if the node is within kill distance
    if (distance(node->position, s) < kill_dist) {
        return true;
    }

    // otherwise, check if any childen are within kill distance
    for (const auto& child : node->children) {
        if (distance(s, child->position) < kill_dist) {
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
    std::map<unsigned int, vector2> influences;
    std::vector<venation::attractor_handle> influencing_attractors;

    for (auto it = attractors_graph_.finite_vertices_begin();
            it != attractors_graph_.finite_vertices_end(); ++it) {
        // 1. associate every attractor with a growth node
        auto attractor = it->point();
        auto vertex = nodes_graph_.nearest_vertex(attractor);
        auto point = vertex->point();
        auto index = vertex->info();
        auto dist = distance(attractor, point);

        if (dist > 0.0 && dist < 0.1) {
            influencing_attractors.push_back(it);
            // 2. sum the difference vectors for each node
            vector2 d = normalize(attractor - point);
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
        auto dst = distance(a->point(), vertex->point());

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
    std::map<unsigned int, vector2> influences;
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
            auto v_s = distance(v, s);
            bool valid = true;
            
            // point v is in the relative neighborhood if
            // (u in V) ||v - s|| < max{||u - s||, ||v - u||}
            for (const auto& u_handle : adjacent) {
                auto u = u_handle->point();
                if (u == v) {
                    continue;
                }

                auto u_s = distance(u, s);
                auto v_u = distance(v, u);
                
                if (v_s >= std::max(u_s, v_u)) {
                    valid = false;
                    break;
                }
            }

            if (!valid) {
                continue;
            }

            if (v_s > 0.0 && v_s < 0.1) {
                // 2. sum the difference vectors for each node
                influenced_node_ids.push_back(v_handle->info());
                vector2 d = normalize(s - v);
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
        }
    }
}    

void venation::update() {
    if (mode_ == venation::type::open) {
        open_step();
    } else if (mode_ == venation::type::closed) {
        closed_step();
    }
}

void venation::draw_attractors() {
    glPointSize(5.0f);
    glBegin(GL_POINTS); 
        glColor3f(1.0f, 0.0f, 0.0f);
        for (auto it = attractors_graph_.finite_vertices_begin();
                it != attractors_graph_.finite_vertices_end(); ++it) {
            auto v = it->point();
            glVertex2d(v.x(), v.y());
        }
    glEnd();
}

void venation::draw_nodes() {
    std::vector<node_ref> to_visit;
    to_visit.push_back(nodes_[0]);

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(3.0f);

    while (to_visit.size() > 0) {
        auto node = to_visit.back();
        to_visit.pop_back();

        for (auto child : node->children) {
            to_visit.push_back(child);
            glBegin(GL_LINES);
                glVertex2d(node->position.x(), node->position.y());
                glVertex2d(child->position.x(), child->position.y());
            glEnd();
        }
    }
}
