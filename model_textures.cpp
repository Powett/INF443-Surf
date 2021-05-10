
#include "vcl/math/math.hpp"
#include "model_textures.hpp"

using namespace vcl;

mesh tore_with_texture(float a, float b, vec3 p0)
{

    // Number of samples of the terrain is N x N
    const unsigned int N = 50;

    mesh tore; // temporary terrain storage (CPU only)
    tore.position.resize(N * N);
    tore.uv.resize(N * N);

    // Fill terrain geometry
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function

            vec3 const p = {
                    (a + b * std::cos(2 * pi * u)) * std::sin(2 * pi * v),
                    (a + b * std::cos(2 * pi * u))* std::cos(2 * pi * v),
                    b * std::sin(2 * pi * u) };


            // Store vertex coordinates
            tore.position[kv + N * ku] = p+p0;
            tore.uv[kv + N * ku] = { -15*v, 2*u };
        }
    }


    // Generate triangle organization
    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku;

            const uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            const uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            tore.connectivity.push_back(triangle_1);
            tore.connectivity.push_back(triangle_2);
        }
    }

    tore.fill_empty_field();
    return tore;
}


mesh cylinder_with_texture(float r, vec3 p0, float h)
{

    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh cylinder; // temporary terrain storage (CPU only)
    cylinder.position.resize(2 * N);
    cylinder.uv.resize(2 * N);
    
    for (unsigned int i = 0; i < N; ++i)
    {
        vec3 p = { r * std::cos(2 * pi * i / N),r * std::sin(2 * pi * i / N), -h/2 };
        cylinder.position[2 * i] = p;
        cylinder.uv[2 * i] = { 0.8f * p.x,0.8f * p.y };
        p = vec3( r * std::cos(2 * pi * i / N),r * std::sin(2 * pi * i / N), h/2 );
        cylinder.position[2 * i + 1] = p;
        cylinder.uv[2 * i + 1] = { 0.8f * p.x,0.8f * p.y };
    }
    // Generate triangle organization
    for (size_t i = 0; i < 2 * N; i += 2)
    {
        const uint3 triangle_1 = { i, (i + 1) % (2 * N), (i + 2) % (2 * N) };
        const uint3 triangle_2 = { i + 1, (i + 3) % (2 * N), (i + 2) % (2 * N) };
        cylinder.connectivity.push_back(triangle_1);
        cylinder.connectivity.push_back(triangle_2);
    }
    cylinder.fill_empty_field();
    cylinder.push_back(disc_with_texture(r, -h / 2 * vec3(0, 0, 1)));
    cylinder.push_back(disc_with_texture(r, +h / 2 * vec3(0, 0, 1)));
    return cylinder;
}


mesh disc_with_texture(float r, vec3 p0)
{
    mesh disc;
    size_t N = 100;

    for (size_t k = 0; k < size_t(N); ++k)
    {
        float const u = k / (N - 1.0f);
        vec3 const p = r * vec3(std::cos(2 * pi * u), std::sin(2 * pi * u), 0.0f);
        disc.position.push_back(p+p0);
        disc.uv.push_back({(p.x/r+1.0f)*1/2,(p.y/r +1.0f)*1/2});

    }
    // middle point
    disc.position.push_back(p0);
    disc.uv.push_back({ 0.5f,0.5f });

    for (size_t k = 0; k < size_t(N - 1); ++k)
        disc.connectivity.push_back(uint3{ unsigned(N), unsigned(k), unsigned(k + 1) });

    disc.fill_empty_field();
    return disc;
}

mesh ellipsoid_with_texture(vec3 scale, vec3 const& center, int Nu, int Nv)
{
//Most of the code comes from vcl::mesh_primitive_ellipsoid()
    assert_vcl(scale.x > 0 && scale.y > 0 && scale.z > 0, "Ellipsoid radius should be > 0");
    assert_vcl(Nu > 2 && Nv > 2, "Sphere samples should be > 2");

    mesh shape;
    for (size_t ku = 0; ku < size_t(Nu); ++ku) {
        for (size_t kv = 0; kv < size_t(Nv); ++kv) {
            float const u = ku / (Nu - 1.0f);
            float const alpha = kv / (Nv - 1.0f);
            float const v = 1.0f / (Nv + 1.0f) * (1 - alpha) + alpha * Nv / (Nv + 1.0f);

            float const theta = 2.0f * pi * (u - 0.5f);
            float const phi = pi * (v - 0.5f);

            // spherical coordinates
            vec3 const n = {
                std::cos(phi) * std::cos(theta),
                std::cos(phi) * std::sin(theta),
                std::sin(phi) };
            vec3 const p = scale * n + center;
            vec2 const uv = { u*5,v*4 };

            shape.position.push_back(p);
            shape.normal.push_back(n);
            shape.uv.push_back(uv);
        }
    }
    buffer<uint3> connectivity;
    for (size_t ku = 0; ku < Nu - 1; ++ku) {
        for (size_t kv = 0; kv < Nv - 1; ++kv) {
            unsigned int k00 = static_cast<unsigned int>(kv + Nv * ku);
            unsigned int k10 = static_cast<unsigned int>(kv + 1 + Nv * ku);
            unsigned int k01 = static_cast<unsigned int>(kv + Nv * (ku + 1));
            unsigned int k11 = static_cast<unsigned int>(kv + 1 + Nv * (ku + 1));

            connectivity.push_back(uint3{ k00, k10, k11 });
            connectivity.push_back(uint3{ k00, k11, k01 });
        }
    }
    shape.connectivity = connectivity;


    // poles
    for (size_t ku = 0; ku < size_t(Nu - 1); ++ku)
    {
        shape.position.push_back(center + scale * vec3{ 0,0,-1.0f });
        shape.normal.push_back(vec3{ 0,0,-1.0f });
        shape.uv.push_back({ ku / (Nu - 1.0f),0.0f });
    }
    for (size_t ku = 0; ku < size_t(Nu - 1); ++ku)
        shape.connectivity.push_back({ Nu * Nv + ku, Nv * ku, Nv * (ku + 1) });

    for (size_t ku = 0; ku < size_t(Nu - 1); ++ku)
    {
        shape.position.push_back(center + scale * vec3{ 0,0,1.0f });
        shape.normal.push_back(vec3{ 0,0,1.0f });
        shape.uv.push_back({ 5*ku / (Nu - 1.0f),1.0f });
    }
    for (size_t ku = 0; ku < size_t(Nu - 1); ++ku)
        shape.connectivity.push_back({ Nu * Nv + Nu - 1 + ku, Nv - 1 + Nv * (ku + 1), Nv - 1 + Nv * ku });



    shape.fill_empty_field();
    shape.flip_connectivity();
    return shape;
}
