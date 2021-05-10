#pragma once

#include "vcl/vcl.hpp"


vcl::vec3 evaluate_terrain(float u, float v, float t);
vcl::vec3 evaluate_terrain_bruit(float u, float v, float t);
vcl::vec3 evaluate_terrain_bruit(vcl::vec3 p, float t);
vcl::mesh create_terrain(bool bruit, float t);
std::vector<vcl::vec3> generate_positions_on_terrain(int N);
std::vector<vcl::vec3> generate_positions_on_terrain(int N, float width, std::vector<float> scales);
std::vector<float> generate_scales(int N);
