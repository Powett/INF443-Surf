#include "scene_helper.hpp"

using namespace vcl;

void display_keypositions(mesh_drawable& sphere, buffer<vec3> const& key_positions, scene_environment const& scene)
{
	size_t const N = key_positions.size();
	for (size_t k = 0; k < N; ++k) {
		sphere.shading.color = { 0,1,0 };
		sphere.transform.translate = key_positions[k];
		draw(sphere, scene);
	}
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}