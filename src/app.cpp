#include <algorithm>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>
#include <boost/iterator/zip_iterator.hpp>

#include "./app.hpp"

double rnd() {
    return static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
}

void App::generate_attractors() {
    constexpr int num_attractors = 1000;
    double x;
    double y;
    attractors_ = std::vector<Point_2>(num_attractors);
    std::vector<int> attractors_indices(num_attractors);

    for (int i = 0; i < num_attractors; ++i) {
        x = rnd() * 2.0 - 1.0;
        y = rnd() * 2.0 - 1.0;
        attractors_[i] = Point_2(x, y);
        attractors_indices[i] = i;
    }

    attractors_tree_.insert(
        boost::make_zip_iterator(
            boost::make_tuple(
                attractors_.begin(),
                attractors_indices.begin()
            )
        ),
        boost::make_zip_iterator(
            boost::make_tuple(
                attractors_.end(),
                attractors_indices.end()
            )
        )
    );
    attractors_tree_.build();
}

void App::setup() {
    generate_attractors();
}

void App::draw_attractors() {
    Point_2 query(0.0, 0.0);
    K_neighbor_search search(attractors_tree_, query, 1);

    Distance tr_dist;
    // loop K through closest points
    for (K_neighbor_search::iterator it = search.begin(); it != search.end(); it++) {
        // it->first is the point, in this case tuple (point, index)
        Point_2 p = boost::get<0>(it->first);
        int index = boost::get<1>(it->first);

        // it->second the transformed distance
        double dist = tr_dist.inverse_of_transformed_distance(it->second);

        std::cout << " d(q, nearest neighbor)=  " << dist << " "
            << p << " " << index << std::endl;

        // delete the point
        attractors_.erase(std::remove(attractors_.begin(), attractors_.end(), p),
            attractors_.end());
        attractors_tree_.remove(it->first);
    }

    glPointSize(10.0f);
    glBegin(GL_POINTS); 
        glColor3f(1.0f, 0.0f, 0.0f);
        for (const auto& v : attractors_) {
            glVertex2d(v.x(), v.y());
        }
    glEnd();
}

void App::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_attractors();
    
    // glFlush();
}

