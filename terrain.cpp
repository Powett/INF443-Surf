
#include "terrain.hpp"



using namespace vcl;


// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, float t)
{

    std::vector<vec2> p = { {0.f,0.f},{0.5f,0.5f}, {0.2f,0.7f},{0.8f,0.7f} };
    std::vector<float> h = { 2.f*cos(2*pi*t), -1.f*cos(2 * pi * (t-1/2)), 1.f, 1.8f*cos(2 * pi * t) };
    std::vector<float> sigma = { 0.5f,0.15f,0.2f,0.2f };

    float const x = 20*(u-0.5f);
    float const y = 20*(v-0.5f);

    vec2 const u0 = {0.5f, 0.5f};

    float d = 0;

    float z = 0;
    for (size_t i = 0; i < p.size(); i++) {
        d=norm(vec2(u, v) - p[i]) / sigma[i];
        z += h[i] * std::exp(-(d*d)); 
    }

    return {x,y,z};
}
vec3 evaluate_terrain_bruit(float u, float v, float t) {
    vec3 p = evaluate_terrain(u, v,t);
    p.z*=( 1+0.2F*noise_perlin({ u, v }, 2, 1.2f, 10.0f));
    p.z -= 0.3f;
    return p;
}
vec3 evaluate_terrain_bruit(vec3 p, float t) {
    return evaluate_terrain_bruit((p.x) / 20 + 0.5f, (p.y) / 20 + 0.5f, t);
}
vec3 evaluate_normal(vec3 p, float t) {
    float dx = 0.01f;
    vec3 up = p + vec3(0, dx, 0);
    vec3 down = p - vec3(0, dx, 0);
    vec3 right = p + vec3(dx, 0, 0);
    vec3 left = p - vec3(dx,0, 0);
    vec3 vert = evaluate_terrain_bruit(up,t) - evaluate_terrain_bruit(down, t);
    vec3 hor = evaluate_terrain_bruit(right, t) - evaluate_terrain_bruit(left, t);
    vec3 u = vec3(vert.y * hor.z - vert.z * hor.y, vert.z * hor.x - vert.x * hor.z, vert.x * hor.y - vert.y * hor.x);
    return u / norm(u);
}

mesh create_terrain(bool bruit, float t)
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for(unsigned int ku=0; ku<N; ++ku)
    {
        for(unsigned int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute the local surface function
            
            vec3 const p = bruit ? evaluate_terrain_bruit(u,v,t): evaluate_terrain(u, v, t);

            // Store vertex coordinates
            terrain.position[kv+N*ku] = p;
            terrain.uv[kv + N * ku] = { 10*u,10*v };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(size_t ku=0; ku<N-1; ++ku)
    {
        for(size_t kv=0; kv<N-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

	terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}


std::vector<vec3> generate_positions_on_terrain(int N, float width, std::vector<float> scales) {
    std::vector<vec3> positions = {};
    float sc=1.f;
    int attempts = 0;
    for (unsigned int i = 0; i < N; ++i) {
        if (!scales.empty()) {sc  = scales[i]; }
        vec3 u = evaluate_terrain_bruit(rand_interval(0, 1), rand_interval(0, 1),0);
        u.z -= 0.01f*sc;
        int distance = 0.f;
        attempts = 0;
        while (distance < 2*width*sc && attempts<5) {
            u = evaluate_terrain_bruit(rand_interval(0, 1), rand_interval(0, 1),0);
            distance = 10.f;
            for (vec3 v : positions) {
                distance = std::min(float(distance), norm(v - u));
            }
            attempts++;
        }
        positions.push_back(u);
    }
    return positions;
}
std::vector<vec3> generate_positions_on_terrain(int N) {
    return generate_positions_on_terrain(N, 0.5f, {});
}

std::vector<float> generate_scales(int N) {
    std::vector<float> scales = {};
    for (unsigned int i = 0; i < N; ++i) {
        scales.push_back(rand_interval(0.5f, 2.f));
    }
    return scales;
}
