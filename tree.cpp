#include "tree.hpp"
#define TREE_RADIUS 0.1f 
#define SHROOM_RADIUS 0.04f 

using namespace vcl;


mesh create_tree_trunk_cylinder(float radius, float height)
{
    const unsigned int N = 100;

    vec3 p;

    mesh tree;
    tree.position.resize(2*N);

    // Fill tree geometry
    for (unsigned int i = 0; i < N; ++i)
    {
        p = { radius*std::cos(2*pi*i/N),radius * std::sin(2 * pi * i / N), 0.f};
        tree.position[2*i] = p;
        p = { radius * std::cos(2 * pi * i / N),radius * std::sin(2 * pi * i / N), height };
        tree.position[2*i +1] = p;
    }
    for (size_t i = 0; i < 2*N; i+=2)
    {
        const uint3 triangle_1= { i, (i+1)%(2*N), (i+2) % (2 * N) };
        const uint3 triangle_2 = { i+1, (i + 3) % (2 * N), (i + 2) % (2 * N) };
        tree.connectivity.push_back(triangle_1);
        tree.connectivity.push_back(triangle_2);
    }
    tree.fill_empty_field();
    return tree;
}

mesh create_cone(float radius, float height, float z_offset)
{
    const unsigned int N = 100;

    vec3 p;
    mesh m;
    m.position.resize(N+2);
    m.position[0] = { 0,0,z_offset };
    m.position[1] = { 0,0,z_offset+height };

    // Fill m geometry
    for (unsigned int i = 2; i < N+2; ++i)
    {
        p = { radius * std::cos(2 * pi * (i-2) / N),radius * std::sin(2 * pi * (i-2) / N), z_offset };
        m.position[i] = p;
    }
    for (size_t i = 2; i < N+1; ++i)
    {
        const uint3 triangle_1 = {1,i,i+1};
        //const uint3 triangle_2 = { i,0,i + 1 };
        m.connectivity.push_back(triangle_1);
       // m.connectivity.push_back(triangle_2);
    }
    const uint3 triangle_1 = { 1,N+1,2 };
    const uint3 triangle_2 = { N+1,0,2 };
    m.connectivity.push_back(triangle_1);
    //m.connectivity.push_back(triangle_2);
    m.fill_empty_field();
    return m;
}

mesh create_tree()
{
    float const h = 0.7f; // trunk height
    float const r = TREE_RADIUS; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composted of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);
    return tree;
}
mesh create_shroom() {
    float const h = 0.07f; // shaft height
    float const r = SHROOM_RADIUS/4; // shaft radius
    mesh shaft = create_tree_trunk_cylinder(r, h);
    shaft.color.fill({ 1, 1, 1 });
    mesh hat = create_cone(6 * r, 3 * r, 0.0f);
    hat.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    hat.color.fill({ 0.9f, 0.1f, 0.1f });
    mesh shroom = shaft;
    shroom.push_back(hat);
    return shroom;
}
