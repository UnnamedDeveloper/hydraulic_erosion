#include "noise.h"

#include <stdlib.h>

// this noise function was mostly copied from this gist:
// https://gist.github.com/nowl/828013

static float interpolate(float a, float b, float w)
{
    return a + w * (b - a);
}

static float smooth_interpolate(float a, float b, float w)
{
    return interpolate(a, b, w * w * (3 - 2 * w));
}

static float random_2d(int x, int y)
{
    int h = x * 374761393 + y * 668265263;
    h = (h ^ (h >> 13)) * 1274126177;
    return h ^ (h >> 16);
}

static float noise_2d(float x, float y)
{
    int ix = (int)x;
    int iy = (int)y;

    float sx = x - ix;
    float sy = y - iy;

    int s = random_2d(ix,     iy    );
    int t = random_2d(ix + 1, iy    );
    int u = random_2d(ix,     iy + 1);
    int v = random_2d(ix + 1, iy + 1);

    int low  = smooth_interpolate(s, t, sx);
    int high = smooth_interpolate(u, v, sx);

    return smooth_interpolate(low, high, sy);
}

float perlin_noise_2d(uint32_t x, uint32_t y)
{
    float fx = (float)x / 10.0f;
    float fy = (float)y / 10.0f;

    float frequency = 0.1f;
    float amplitude = 4.0f;
    int octaves = 8;

    float ax = fx * frequency;
    float ay = fy * frequency;

    float div = 0.0f;
    float total = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        div += 256 * amplitude;
        total += noise_2d(ax, ay) * amplitude;

        amplitude /= 2;
        ax *= 2;
        ay *= 2;
    }

    return total / div / 100000;
}
