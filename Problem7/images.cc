#include "images.h"

Image cond(const Region &region, const Image &this_way, const Image &that_way) {
    return [=](Point p) { return region(p) ? this_way(p) : that_way(p); };
}

Image lerp(const Blend &blend, const Image &this_way, const Image &that_way) {
    return lift(&Color::weighted_mean, this_way, that_way, blend);
}

Image darken(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant(Colors::black));
}

Image lighten(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant(Colors::white));
}
