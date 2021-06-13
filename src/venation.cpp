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

/*
 * Seed the growth.
 */
void venation::create_seed() {
    point2 seed_point(0.0, 0.0);

    // vertex with info needs to be inserted to the graph via vector
    std::vector<std::pair<point2, unsigned int>> seeds;
    seeds.push_back(std::make_pair(seed_point, nodes_.size()));
    nodes_graph_.insert(seeds.begin(), seeds.end());

    // add the node to the node index vector.
    node_ref seed_node = node::create(seed_point);
    nodes_.push_back(seed_node);
}

venation::vector2 normalize(venation::vector2 p) {
    return p / std::sqrt(p.squared_length());
}

void venation::update() {
    // find the closest node to each attractor.
    std::map<unsigned int, vector2> influences;
    std::vector<point2> influencing_attractors;
    for (auto it = attractors_graph_.finite_vertices_begin();
            it != attractors_graph_.finite_vertices_end(); ++it) {
        // 1. associate every attractor with a growth node
        auto query = it->point();
        auto nearest_v = nodes_graph_.nearest_vertex(query);
        auto nearest_p = nearest_v->point();
        auto nearest_i = nearest_v->info();
        auto dst = std::sqrt(CGAL::squared_distance(query, nearest_p));

        if (dst < 0.1) {
            influencing_attractors.push_back(query);
            // 2. sum the difference vectors for each node
            vector2 d = normalize(query - nearest_p);
            auto l = influences.find(nearest_i);
            if (l == influences.end()) {
                influences[nearest_i] = d;
            } else {
                influences[nearest_i] = l->second + d;
            }
        }
    }

    std::vector<std::pair<venation::point2, unsigned int>> new_points;
    for (auto& i : influences) {
        // 3. normalize each vector sum
        i.second = normalize(i.second);
    
        // 4. add new node
        auto& parent = nodes_[i.first];
        vector2 step = i.second * 0.002;
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

    // 5. remove attractors that have been consumed
    for (const auto& a : influencing_attractors) {
        auto nearest_v = nodes_graph_.nearest_vertex(a);
        auto nearest_p = nearest_v->point();
        auto dst = std::sqrt(CGAL::squared_distance(a, nearest_p));
        
        if (dst < 0.001) {
            attractors_graph_.remove(attractors_graph_.nearest_vertex(a));
        }
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
