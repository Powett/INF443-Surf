#pragma once


#include "vcl/vcl.hpp"

struct scene_environment
{
	vcl::camera_around_center camera;
	vcl::mat4 projection;
	vcl::mat4 projection_inverse;
	vcl::vec3 light;
};

struct picking_structure {
	bool active; 
	int index;
};

struct gui_parameters {
	bool display_surface = true;
	bool display_wireframe = false;
	bool display_frame = false;
	bool display_polygon = false;
	bool display_keyposition = true;
	bool display_trajectory = false;
	int trajectory_storage = 0;
};

struct user_interaction_parameters {
	vcl::vec2 mouse_prev;
	vcl::timer_fps fps_record;
	vcl::mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};


void display_keypositions(vcl::mesh_drawable& sphere, vcl::buffer<vcl::vec3> const& key_positions, scene_environment const& scene);

void opengl_uniform(GLuint shader, scene_environment const& current_scene);