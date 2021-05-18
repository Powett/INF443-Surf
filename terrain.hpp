#pragma once

#include "vcl/vcl.hpp"


vcl::vec3 evaluate_terrain(float u, float v, float t);
vcl::vec3 evaluate_terrain_bruit(float u, float v, float t);
vcl::vec3 evaluate_terrain_bruit(vcl::vec3 p, float t);
vcl::mesh create_terrain(bool bruit, float t);
vcl::buffer<vcl::vec3> update_terrain(bool bruit, float waveHeight,float t);
