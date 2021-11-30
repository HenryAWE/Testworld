// Author: HenryAWE
// License: The 3-clause BSD License

#ifndef TESTWORLD_GRAPHIC_NOISE_HPP
#define TESTWORLD_GRAPHIC_NOISE_HPP

#include <cmath>

namespace awe::graphic
{
    template <typename T>
    T Noise(int x, int y)
    {
        int n = x + y * 57;
        n = (n << 13) ^ n;
        return T(1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
    }
    template <typename T>
    T NoiseSmoothed(int x, int y)
    {
        T corners = (Noise<T>(x - 1, y - 1) + Noise<T>(x + 1, y - 1) + Noise<T>(x - 1, y + 1) + Noise<T>(x + 1, y + 1)) / 16;
        T sides = (Noise<T>(x - 1, y) + Noise<T>(x + 1, y) + Noise<T>(x, y - 1) + Noise<T>(x, y + 1)) / 8;
        T center = Noise<T>(x, y) / 4;
        return corners + sides + center;
    }
    template <typename T>
    T CosInterpolated(T a, T b, T x)
    {
        T ft = x * T(3.1415927);
        T f = (1 - cos(ft)) * T(0.5);
        return a * (1 - f) + b * f;
    }
    template <typename T>
    T NoiseInterpolated(T x, T y)
    {
        int integer_X = int(x);
        T fractional_X = x - integer_X;
        int integer_Y = int(y);
        T fractional_Y = y - integer_Y;
        T v1 = NoiseSmoothed<T>(integer_X, integer_Y);
        T v2 = NoiseSmoothed<T>(integer_X + 1, integer_Y);
        T v3 = NoiseSmoothed<T>(integer_X, integer_Y + 1);
        T v4 = NoiseSmoothed<T>(integer_X + 1, integer_Y + 1);
        T i1 = CosInterpolated<T>(v1, v2, fractional_X);
        T i2 = CosInterpolated<T>(v3, v4, fractional_X);
        return CosInterpolated<T>(i1, i2, fractional_Y);
    }

    template <typename T>
    T PerlinNoise(T p, float x, float y)
    {
        T total = 0;
        constexpr int n = 4;
        for (int i = 0; i < n; ++i)
        {
            T frequency = std::pow(T(2), T(i));
            T amplitude = std::pow(p, T(i));
            total = total + NoiseInterpolated(x * frequency, y * frequency) * amplitude;
        }

        return total;
    }
}

#endif
