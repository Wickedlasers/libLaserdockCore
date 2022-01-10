//
// Created by Darren Otgaar on 2018/10/16.
//

#include "ldRazer.h"

namespace ldRazer
{

    uint32_t randomColour()
    {
        static ldRandom<float> rnd;
        auto p = (NO_RED_LASER ? 0.f : rnd.random1()), q = rnd.random1();
        if (p + q >= 1) {
            p = 1 - p;
            q = 1 - q;
        }
        auto r = 1.f - (p + q);

        // Bias towards brighter & greater hue
        if (p > q && p > r) p = bias(.8f, p);
        else if (p < q && p < r) p = bias(.2f, p);

        if (q > p && q > r) q = bias(.8f, q);
        else if (q < p && q < r) q = bias(.2f, q);

        if (r > p && r > q) r = bias(.8f, r);
        else if (r < p && r < q) r = bias(.2f, r);

        return makeARGB(byte(255 * clamp(p, 0.f, 1.f)), byte(255 * clamp(q, 0.f, 1.f)), byte(255 * clamp(r, 0.f, 1.f)));
    }

    uint32_t randomColour(const vec2s &redRange, const vec2s &greenRange, const vec2s &blueRange)
    {
        static ldRandom<int16_t> rnd;

        return makeARGB(
                rnd.random1(redRange.x, redRange.y),
                rnd.random1(greenRange.x, greenRange.y),
                rnd.random1(blueRange.x, blueRange.y)
        );
    }

    size_t findInterval(const std::vector<float> &seq, float t)
    {
        if (seq.empty() || t < seq[0]) return 0;

        for (size_t i = 0, end = seq.size() - 1; i != end; ++i) {
            if (seq[i] <= t && t < seq[i + 1]) return i;
        }

        return seq.size() - 1;
    }

}
