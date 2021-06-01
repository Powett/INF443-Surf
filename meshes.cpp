#include "meshes.hpp"
#define TREE_RADIUS 0.1f 
#define SHROOM_RADIUS 0.04f 

using namespace vcl;

mesh create_surfboard(float length, float width, float height) {
    // Plots a surfboard with given dimensions
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


mesh create_kite(float size) {
    // Plot kite sail using given size

    mesh cyl = mesh_primitive_cylinder(size, vec3(0, 0, 25.45f*size), vec3(size * 10, 0, 25.45f * size), 100, 100, false);
    for (int i = 1; i < 11; i++) {
        cyl.push_back(mesh_primitive_cylinder(size, vec3(0, 25.45f * size * std::sin(i * 2 * 3.14f / 80), 25.45f * size * std::cos(i * 2 * 3.14f / 80)), vec3(10 * size * std::cos(i * 3 * 3.14f / 80), 25.45f * size * std::sin(i * 2 * 3.14f / 80), 25.45f * size * std::cos(i * 2 * 3.14f / 80)), 100, 100, false));
        cyl.push_back(mesh_primitive_cylinder(size, vec3(0, -25.45f * size * std::sin(i * 2 * 3.14f / 80), 25.45f * size * std::cos(i * 2 * 3.14f / 80)), vec3(10 * size * std::cos(i * 3 * 3.14f / 80), -25.45f * size * std::sin(i * 2 * 3.14f / 80), 25.45f * size * std::cos(i * 2 * 3.14f / 80)), 100, 100, false));
        i--;
        cyl.push_back(mesh_primitive_quadrangle({ 0, 26.45f * size * std::sin(i * 2 * 3.14f / 80), 26.45f * size * std::cos(i * 2 * 3.14f / 80) }, { 0, 26.45f * size * std::sin((i + 1) * 2 * 3.14f / 80), 26.45f * size * std::cos((i + 1) * 2 * 3.14f / 80) }, { 10 * size * std::cos((i+1) * 3 * 3.14f / 80), 26.45f * size * std::sin((i + 1) * 2 * 3.14f / 80), 26.45f * size * std::cos((i + 1) * 2 * 3.14f / 80) }, { 10 * size * std::cos(i * 3 * 3.14f / 80), 26.45f * size * std::sin(i * 2 * 3.14f / 80), 26.45f * size * std::cos(i * 2 * 3.14f / 80) }));
        cyl.push_back(mesh_primitive_quadrangle({ 0, -26.45f * size * std::sin(i * 2 * 3.14f / 80), 26.45f * size * std::cos(i * 2 * 3.14f / 80) }, { 0, -26.45f * size * std::sin((i + 1) * 2 * 3.14f / 80), 26.45f * size * std::cos((i + 1) * 2 * 3.14f / 80) }, { 10 * size * std::cos((i + 1) * 3 * 3.14f / 80), -26.45f * size * std::sin((i + 1) * 2 * 3.14f / 80), 26.45f * size * std::cos((i + 1) * 2 * 3.14f / 80) }, { 10 * size * std::cos(i * 3 * 3.14f / 80), -26.45f * size * std::sin(i * 2 * 3.14f / 80), 26.45f * size * std::cos(i * 2 * 3.14f / 80) }));
        i++;
    }
    cyl.push_back(mesh_primitive_cylinder(size / 50, vec3(0, 24.45f * size * std::sin(10 * 2 * 3.14f / 80), 24.45f * size * std::cos(10 * 2 * 3.14f / 80)), vec3(0,0,0), 100, 100, true));
    cyl.push_back(mesh_primitive_cylinder(size / 50, vec3( 0, -24.45f * size * std::sin(10 * 2 * 3.14f / 80), 24.45f * size * std::cos(10 * 2 * 3.14f / 80) ), vec3(0,0,0), 100, 100, true));
    cyl.push_back(mesh_primitive_cylinder(size / 50, vec3(10 * size * std::cos(10 * 3 * 3.14f / 80), 26.45f * size * std::sin(10 * 2 * 3.14f / 80), 26.45f * size * std::cos(10 * 2 * 3.14f / 80)), vec3(0,0,0), 100, 100, true));
    cyl.push_back(mesh_primitive_cylinder(size / 50, vec3(10 * size * std::cos(10 * 3 * 3.14f / 80), -26.45f * size * std::sin(10 * 2 * 3.14f / 80), 26.45f * size * std::cos(10 * 2 * 3.14f / 80)), vec3(0,0,0), 100, 100, true));
    
    return cyl;
}

