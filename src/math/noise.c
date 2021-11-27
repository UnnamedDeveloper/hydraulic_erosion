#include "noise.h"

#include <stdlib.h>

// this noise function was mostly copied from this gist:
// https://gist.github.com/nowl/828013

static float random_2d(int seed, int x, int y)
{
    int d = (214013) * (seed * 7852 + x * 4153 + y * y * 6534) + 2531001;
    d = (d >> 16) & 0x7FFF;
    return (float)d / 32767.0f;
}

static float interpolate(float a, float b, float w)
{
    return a + w * (b - a);
}

static float smooth_interpolate(float a, float b, float w)
{
    return interpolate(a, b, w * w * (3.0f - 2.0f * w));
}

static float noise_2d(int seed, float x, float y)
{
    int ix = (int)x;
    int iy = (int)y;

    float sx = x - ix;
    float sy = y - iy;

    float s = random_2d(seed, ix,     iy    );
    float t = random_2d(seed, ix + 1, iy    );
    float u = random_2d(seed, ix,     iy + 1);
    float v = random_2d(seed, ix + 1, iy + 1);

    float low  = smooth_interpolate(s, t, sx);
    float high = smooth_interpolate(u, v, sx);

    return smooth_interpolate(low, high, sy);
}

float perlin_noise_2d(int seed, float x, float y)
{
    float fx = (float)x / 10.0f + 500;
    float fy = (float)y / 10.0f + 500;

    float frequency = 0.2f;
    float amplitude = 4.0f;
    int octaves = 8;

    float ax = fx * frequency;
    float ay = fy * frequency;

    float div = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        div += 1 * amplitude;
        total += noise_2d(seed, ax, ay) * amplitude;

        amplitude /= 2;
        ax *= 2;
        ay *= 2;
    }

    return total / div;
}
