#pragma once

#include "vcl/vcl.hpp"


vcl::vec3 evaluate_terrain_bruit(float u, float v, float t, float bruit);
vcl::vec3 evaluate_terrain_bruit(vcl::vec3 p, float t, float bruit);
vcl::mesh create_terrain_bruit(float t, float bruit);
vcl::buffer<vcl::vec3> update_terrain(float waveHeight,float t, float bruit);
