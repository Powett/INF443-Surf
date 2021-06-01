
#include "terrain.hpp"



using namespace vcl;

float waveHeight = 1.0f; // Global multiplicative factor
std::vector<vec2> p = { {-30.f,-30.f},{-10.0f,-30.f}, {10.f,-30.f},{30.f,-30.f} };
std::vector<float> h = { 2.f, 4.f, 5.0f, 2.f }; // Height at origin
std::vector<float> d0 = { 25.0f, 25.0f, 25.0f, 25.0f }; // Spatial pseudo-period
std::vector<float> d1 = { 15.f, 15.f, 15.f, 15.f }; // Caracteristic damping length
float vit = 5.0f; // Wave speed
float map_size = 30.0f;

const unsigned int N = 200;

vec3 uvToVec(float u, float v) {
    return { map_size * (u - 0.5f),map_size * (v - 0.5f),0 };
}


// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain_bruit(float u, float v, float t, float bruit)
{
    //Sum of h*exp(-d/d0)*cos(2*pi*(d/d1-freq*t)), freq=vit / d1

    float const x = uvToVec(u, v).x;
    float const y = uvToVec(u, v).y;
    float d = 0;
    float z = 0;
    for (size_t i = 0; i < p.size(); i++) {
        d = 0;
        d = norm(vec2(x, y) - p[i]);
        z += waveHeight * h[i] * std::exp(-d / d0[i]) * std::cos(2 * pi * (d / d1[i] - vit * t / d1[i]));
        z += waveHeight * bruit * (1 - 0.9f * noise_perlin({ u, v - vit * t / (2*map_size) }, 5, 0.6f, 5.0f));
    }
    return { x,y,z };
}
vec3 evaluate_terrain_bruit(vec3 p, float t, float bruit) {
    return evaluate_terrain_bruit((p.x) / map_size + 0.5f, (p.y) / map_size + 0.5f, t, bruit);
}
vec3 evaluate_normal(vec3 p, float t, float bruit) {
    float dx = 0.01f;
    vec3 up = p + vec3(0, dx, 0);
    vec3 down = p - vec3(0, dx, 0);
    vec3 right = p + vec3(dx, 0, 0);
    vec3 left = p - vec3(dx, 0, 0);
    vec3 vert = evaluate_terrain_bruit(up, t, bruit) - evaluate_terrain_bruit(down, t, bruit);
    vec3 hor = evaluate_terrain_bruit(right, t, bruit) - evaluate_terrain_bruit(left, t, bruit);
    vec3 u = vec3(vert.y * hor.z - vert.z * hor.y, vert.z * hor.x - vert.x * hor.z, vert.x * hor.y - vert.y * hor.x);
    return u / norm(u);
}

mesh create_terrain_bruit(float t, float bruit)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function

            vec3 const p = evaluate_terrain_bruit(u, v, t, bruit);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = p;
            terrain.uv[kv + N * ku] = { 10 * u,10 * v };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            const uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

buffer<vec3> update_terrain(float waveH, float t, float bruit) {
    waveHeight = waveH;
    buffer<vec3> terrain = buffer<vec3>(N * N);
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function

            vec3 const p = evaluate_terrain_bruit(u, v, t, bruit);

            // Store vertex coordinates
            terrain[kv + N * ku] = p;
        }
    }
    return terrain;
}
