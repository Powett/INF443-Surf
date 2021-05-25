#pragma once

#include "vcl/vcl.hpp"


vcl::mesh create_tree_trunk_cylinder(float radius, float height);
vcl::mesh create_cone(float radius, float height, float z_offset);
vcl::mesh create_tree();
vcl::mesh create_shroom();
vcl::mesh create_surfboard(float length, float width, float height);
vcl::mesh create_kite(float size);
vcl::mesh create_kite2(float size);


