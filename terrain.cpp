
#include "terrain.hpp"



using namespace vcl;

float waveHeight=1.0f;

vec3 uvToVec(float u, float v) {
    return { 20 * (u - 0.5f),20 * (v - 0.5f),0 };
}

 
// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, float t)
{
    //Sum of h*exp(-d/d0)*cos(2*pi*(d/d1-freq*t))

    std::vector<vec2> p = { {-30.f,-30.f},{-10.0f,-30.f}, {10.f,-30.f},{30.f,-30.f} };
    std::vector<float> h = { 1.f, 2.f, 2.5f, 1.f };
    std::vector<float> d0 = { 25.0f, 25.0f, 25.0f, 25.0f };
    std::vector<float> d1 = { 5.f, 5.f, 5.f, 5.f };
    float vit = 1.0f;
    float const x = uvToVec(u, v).x;
    float const y = uvToVec(u, v).y;


    float d = 0;

    float z = 0;
    for (size_t i = 0; i < p.size(); i++) {
        d = 0;
        d=norm(vec2(x, y) - p[i]);
        z += waveHeight*h[i] * std::exp(-pow(d/d0[i],2)) * std::cos(2*pi*(d/d1[i] - vit*t/d1[i])); 
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
vec3 evaluate_terrain(vec3 p, float t) {
    return evaluate_terrain((p.x) / 20 + 0.5f, (p.y) / 20 + 0.5f, t);
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

buffer<vec3> update_terrain(bool bruit, float waveH, float t) {
    int N = 100;
    waveHeight = waveH;
    buffer<vec3> terrain = buffer<vec3>(N*N);
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function

            vec3 const p = bruit ? evaluate_terrain_bruit(u, v, t) : evaluate_terrain(u, v, t);

            // Store vertex coordinates
            terrain[kv + N * ku] = p;
        }
    }
    return terrain;
}
