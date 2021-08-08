#ifndef IMAGES_H
#define IMAGES_H

#include "coordinate.h"
#include "color.h"
#include "functional.h"

#include <functional>
#include <cassert>
#include <cmath>

using Fraction = double;
template<typename T>
using Base_image = std::function<T(const Point)>;
using Region = Base_image<bool>;
using Image = Base_image<Color>;
using Blend = Base_image<Fraction>;

namespace Detail {
    static Point add(const Point &p, const Vector &vector) {
        return p.is_polar ? add(from_polar(p), vector) : Point{p.first - vector.first,
                                                               p.second - vector.second, false};
    }

    static Point rotate(const Point &p, double phi) {
        return p.is_polar ? Point{p.first, p.second - phi, true} : rotate(
                to_polar(p), phi);
    }

    static Point scale(const Point &p, double s) {
        return p.is_polar ? scale(from_polar(p), s) : Point(p.first / s, p.second / s, false);
    }

    static bool in_circle(const Point &p, const Point &q, double r) {
        return p.is_polar ? in_circle(from_polar(p), q, r) : distance(p, q) <= r;
    }

    static bool is_this_checker(const Point &p, double d) {
        return p.is_polar ? is_this_checker(from_polar(p), d) :
               (int) (floor(p.first / d) + floor(p.second / d)) % 2;
    }

    static Point convert_polar_checker(const Point &p, int n, double d) {
        return p.is_polar ? Point(p.first, p.second * (double(n) * d) / (2 * M_PI))
                          : convert_polar_checker(to_polar(p), n, d);
    }

    static bool is_this_ring(const Point &p, const Point &q, double d) {
        return p.is_polar ? is_this_ring(from_polar(p), q, d) : q.is_polar ? is_this_ring(p, from_polar(q), d) :
                                                                (int) floor(distance(p, q) / d) % 2 == 0;
    }

    static bool is_stripe(const Point &p, double d) {
        return p.is_polar ? is_stripe(from_polar(p), d) : std::abs(p.first) <= d / 2;
    }
}

template<typename T>
Base_image<T> constant(const T &t) {
    return [=](Point) { return t; };
}

template<typename T>
Base_image<T> rotate(const Base_image<T> &baseImage, double phi) {
    return [=](Point p) { return compose(Detail::rotate, baseImage)(p, phi); };
}

template<typename T>
Base_image<T> translate(const Base_image<T> &baseImage, const Vector &vector) {
    return [=](Point p) { return compose(Detail::add, baseImage)(p, vector); };
}

template<typename T>
Base_image<T> scale(const Base_image<T> &baseImage, double s) {
    assert(s != 0);

    return [=](Point p) { return compose(Detail::scale, baseImage)(p, s); };
}

template<typename T>
Base_image<T> circle(const Point &q, double r, const T &inner, const T &outer) {
    return [=](Point p) { return Detail::in_circle(p, q, r) ? inner : outer; };
}

template<typename T>
Base_image<T> checker(double d, const T &this_way, const T &that_way) {
    assert(d != 0);

    return [=](Point p) { return Detail::is_this_checker(p, d) ? that_way : this_way; };
}

template<typename T>
Base_image<T> polar_checker(double d, int n, const T &this_way, const T &that_way) {
    assert(d != 0);

    return [=](Point p) {
        return compose(Detail::convert_polar_checker, checker(d, this_way, that_way))(p, n, d);
    };
}

template<typename T>
Base_image<T> rings(const Point &q, double d, const T &this_way, const T &that_way) {
    return [=](Point p) { return Detail::is_this_ring(p, q, d) ? this_way : that_way; };
}

template<typename T>
Base_image<T> vertical_stripe(double d, const T &this_way, const T &that_way) {
    return [=](Point p) { return Detail::is_stripe(p, d) ? this_way : that_way; };
}

Image cond(const Region &region, const Image &this_way, const Image &that_way);

Image lerp(const Blend &blend, const Image &this_way, const Image &that_way);

Image darken(const Image &image, const Blend &blend);

Image lighten(const Image &image, const Blend &blend);

#endif //IMAGES_H
