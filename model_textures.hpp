#pragma once

#include "vcl/vcl.hpp"


vcl::mesh disc_with_texture(float r, vcl::vec3 center);
vcl::mesh cylinder_with_texture(float r, vcl::vec3 p0, float h);
vcl::mesh tore_with_texture(float rmaj, float rmin, vcl::vec3 center);
vcl::mesh ellipsoid_with_texture(vcl::vec3 scale = vcl::vec3{ 1.0f, 1.0f, 1.0f }, vcl::vec3 const& center = { 0,0,0 }, int Nu = 40, int Nv = 20);
