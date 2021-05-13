#include "meshes.hpp"
#define TREE_RADIUS 0.1f 
#define SHROOM_RADIUS 0.04f 

using namespace vcl;

mesh create_surfboard(float length, float width, float height) {

    mesh surfboard;
    surfboard.position.resize(26);

    surfboard.position[0] = { 0,length / 2,height/2 };
    surfboard.position[1] = { 3.0f/8.0f * width ,length / 3,height/4 };
    surfboard.position[2] = { -3.0f / 8.0f * width ,length / 3,height / 4 };
    surfboard.position[3] = { width/2 ,0,0 };
    surfboard.position[4] = { -width / 2 ,0,0 };
    surfboard.position[5] = { 3.0f / 8.0f * width ,-length / 3,0};
    surfboard.position[6] = { -3.0f / 8.0f * width ,-length / 3,0};
    surfboard.position[7] = { width/8.0f,-length / 2,0 };
    surfboard.position[8] = { -width / 8.0f,-length / 2,0 };

    surfboard.position[9] = { 3.0f / 8.0f * width ,length / 3,-height / 4 };
    surfboard.position[10] = { -3.0f / 8.0f * width ,length / 3,-height / 4 };
    surfboard.position[11] = { width / 2 ,0, -height/2 };
    surfboard.position[12] = { -width / 2 ,0, -height / 2 };
    surfboard.position[13] = { 3.0f / 8.0f * width ,-length / 3,-height / 2 };
    surfboard.position[14] = { -3.0f / 8.0f * width ,-length / 3,-height / 2 };
    surfboard.position[15] = { width / 8.0f,-length / 2,-height / 2 };
    surfboard.position[16] = { -width / 8.0f,-length / 2,-height / 2 };

    surfboard.position[17] = { width / 20, -0.36f * length, -height / 2 };
    surfboard.position[18] = { -width / 20, -0.36f * length, -height / 2 };
    surfboard.position[19] = { width / 20, -0.41f * length, -height / 2 };
    surfboard.position[20] = { -width / 20, -0.41f * length, -height / 2 };
    surfboard.position[21] = { width / 30, -0.37f * length, -3*height / 2 };
    surfboard.position[22] = { -width / 30, -0.37f * length, -3*height / 2 };
    surfboard.position[23] = { width / 30, -0.39f * length, -2.5f*height / 2 };
    surfboard.position[24] = { -width / 30, -0.39f * length, -2.5f*height / 2 };
    surfboard.position[25] = {0, -0.41f * length, -2*height };

    surfboard.connectivity.push_back({ 0,1,2 });
    surfboard.connectivity.push_back({ 1,3,2 });
    surfboard.connectivity.push_back({ 3,4,2 });
    surfboard.connectivity.push_back({ 3,5,4 });
    surfboard.connectivity.push_back({ 5,6,4 });
    surfboard.connectivity.push_back({ 5,7,6 });
    surfboard.connectivity.push_back({ 7,8,6 });

    surfboard.connectivity.push_back({ 0,10,9 });
    surfboard.connectivity.push_back({ 9,10,11 });
    surfboard.connectivity.push_back({ 11,10,12 });
    surfboard.connectivity.push_back({ 13,11,12 });
    surfboard.connectivity.push_back({ 13,12,14 });
    surfboard.connectivity.push_back({ 13,14,15 });
    surfboard.connectivity.push_back({ 15,14,16 });

    surfboard.connectivity.push_back({ 0,9,1 });
    surfboard.connectivity.push_back({ 9,11,1 });
    surfboard.connectivity.push_back({ 11,3,1 });
    surfboard.connectivity.push_back({ 13,3,11 });
    surfboard.connectivity.push_back({ 13,5,3 });
    surfboard.connectivity.push_back({ 13,15,5 });
    surfboard.connectivity.push_back({ 15,7,5 });
    surfboard.connectivity.push_back({ 15,8,7 });
    surfboard.connectivity.push_back({ 15,16,8 });
    surfboard.connectivity.push_back({ 8,16,14 });
    surfboard.connectivity.push_back({ 8,14,6 });
    surfboard.connectivity.push_back({ 6,14,12 });
    surfboard.connectivity.push_back({ 6,12,4 });
    surfboard.connectivity.push_back({ 4,12,10 });
    surfboard.connectivity.push_back({ 4,10,2 });
    surfboard.connectivity.push_back({ 2,10,0 });

    surfboard.connectivity.push_back({ 20,24,22 });
    surfboard.connectivity.push_back({ 20,22,18 });
    surfboard.connectivity.push_back({ 24,25,22 });

    surfboard.connectivity.push_back({ 17,21,23 });
    surfboard.connectivity.push_back({ 17,23,19 });
    surfboard.connectivity.push_back({ 21,25,23 });
    surfboard.connectivity.push_back({ 23,25,24 });
    surfboard.connectivity.push_back({ 19,23,20 });
    surfboard.connectivity.push_back({ 20,23,24 });

    surfboard.connectivity.push_back({ 22,25,21 });
    surfboard.connectivity.push_back({ 18,22,21 });
    surfboard.connectivity.push_back({ 18,21,17 });
    surfboard.flip_connectivity();
    

    surfboard.fill_empty_field();
    return surfboard;
}
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
