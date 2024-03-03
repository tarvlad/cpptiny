#pragma once

#include <optional>
#include <cmath>
#include <limits>

namespace geom {

template <std::floating_point Float>
class Vector {
    Float x_, y_, z_;

public:
    Vector() = delete;
    Vector(Float x, Float y, Float z) : x_(x), y_(y), z_(z) {}
    Vector(const Vector &v) : x_(v.x_), y_(v.y_), z_(v.z_) {}

    Float x() const { return x_; }
    Float y() const { return y_; }
    Float z() const { return z_; }

    Vector &operator=(const Vector &v) {
        x_ = v.x_;
        y_ = v.y_;
        z_ = v.z_;
        return *this;
    }

    Float dot(const Vector &v) const {
        return x_ * v.x_ + y_ * v.y_ + z_ * v.z_;
    }

    Vector cross(const Vector &v) const {
        return Vector(
            y_ * v.z_ - z_ * v.y_,
            z_ * v.x_ - x_ * v.z_,
            x_ * v.y_ - y_ * v.x_
        );
    }

    Float length() const {
        return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }

    Vector normalized() const {
        Float len = length();
        if (len > std::numeric_limits<Float>::epsilon()) {
            return Vector(x_ / len, y_ / len, z_ / len);
        }
        return *this;
    }

    friend Vector operator+(const Vector &lhs, const Vector &rhs) {
        return Vector(lhs.x_ + rhs.x_, lhs.y_ + rhs.y_, lhs.z_ + rhs.z_);
    }

    friend Vector operator-(const Vector &lhs, const Vector &rhs) {
        return Vector(lhs.x_ - rhs.x_, lhs.y_ - rhs.y_, lhs.z_ - rhs.z_);
    }

    friend Vector operator*(const Vector &v, Float scalar) {
        return Vector(v.x_ * scalar, v.y_ * scalar, v.z_ * scalar);
    }

    friend Vector operator*(Float scalar, const Vector &v) {
        return v * scalar;
    }
};


template <std::floating_point Float>
class Line {
    Vector<Float> point_, direction_;

public:
    Line(const Vector<Float>& point, const Vector<Float>& direction)
    : point_(point), direction_(direction.normalized()) {}

    Line(Float x1, Float y1, Float z1, Float x2, Float y2, Float z2)
    : point_(Vector<Float>(x1, y1, z1)), direction_(Vector<Float>(x2 - x1, y2 - y1, z2 - z1).normalized()) {}

    std::optional<Vector<Float>> intersection_with(const Line<Float>& other) const {
        Vector<Float> dp = other.point_ - point_;
        Vector<Float> cross_dir = direction_.cross(other.direction_);
        Float denom = cross_dir.length();

        if (std::abs(denom) < std::numeric_limits<Float>::epsilon()) { // Lines parallel or coincident
            return std::nullopt;
        }

        Float l = dp.cross(other.direction_).dot(cross_dir) / (denom * denom);
        
        return point_ + direction_ * l;
    }

    Line<Float> perpendicular_at(const Vector<Float>& point) const {
        Vector<Float> non_parallel_vec = direction_.y() != 0 || direction_.z() != 0 ? Vector<Float>(1, 0, 0) : Vector<Float>(0, 1, 0);
        Vector<Float> perp_direction = direction_.cross(non_parallel_vec);
        return Line<Float>(point, perp_direction.normalized());
    }
};

} // namespace geom

