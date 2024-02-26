#include "geom.h"
#include <iostream>
#include <random>
#include <cassert>
#include <cstddef>

int main() {
    using namespace geom;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1000.0, 1000.0); 

    // Deterministic Tests
    // Test 1: Vector addition
    Vector<double> v1(1.0, 2.0, 3.0);
    Vector<double> v2(4.0, 5.0, 6.0);
    Vector<double> v3 = v1 + v2;
    assert(v3.x() == 5.0 && v3.y() == 7.0 && v3.z() == 9.0);

    // Test 2: Vector dot product
    double dot = v1.dot(v2);
    assert(dot == 32.0);

    // Test 3: Vector cross product
    Vector<double> cross = v1.cross(v2);
    assert(cross.x() == -3.0 && cross.y() == 6.0 && cross.z() == -3.0);

    // Test 4: Vector length
    double length = v1.length();
    assert(std::abs(length - std::sqrt(14.0)) < std::numeric_limits<double>::epsilon());

    // Test 5: Line intersection
    Line<double> line1(0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    Line<double> line2(0.0, 1.0, 0.0, 0.0, -1.0, 1.0);
    std::optional<Vector<double>> intersection = line1.intersection_with(line2);
    assert(intersection && intersection->x() == 0.0 && intersection->y() == 0.0 && intersection->z() == 0.0);

    // Stochastic Tests
    for (size_t i = 0; i < 1000; ++i) { // Adjust number of iterations as needed
        // Random Vectors
        Vector<double> rv1(dis(gen), dis(gen), dis(gen));
        Vector<double> rv2(dis(gen), dis(gen), dis(gen));

        // Test random vector addition
        Vector<double> radd = rv1 + rv2;
        assert(radd.x() == rv1.x() + rv2.x() && radd.y() == rv1.y() + rv2.y() && radd.z() == rv1.z() + rv2.z());

        // Test random vector dot product
        double rdot = rv1.dot(rv2);
        double expected_dot = rv1.x() * rv2.x() + rv1.y() * rv2.y() + rv1.z() * rv2.z();
        assert(std::abs(rdot - expected_dot) < std::numeric_limits<double>::epsilon());

        // Test random vector length
        double rlength = rv1.length();
        double expected_length = std::sqrt(rv1.x() * rv1.x() + rv1.y() * rv1.y() + rv1.z() * rv1.z());
        assert(std::abs(rlength - expected_length) < std::numeric_limits<double>::epsilon());
    }

    std::cout << "All tests passed." << std::endl;
    return 0;
}
