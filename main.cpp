#include "vcl/vcl.hpp"
#include "interpolation.hpp"
#include "scene_helper.hpp"
#include "model_textures.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "physics.hpp"
#include <iostream>

//sqdmjkfh

using namespace vcl;

user_interaction_parameters user;
scene_environment scene;

buffer<vec3> key_positions;
buffer<float> key_times;
float totalLength = 0;
timer_interval timer;



void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);


void initialize_data();
void display_interface();
void display_frame();


mesh_drawable sphere_current;    // sphere used to display the interpolated value
mesh_drawable sphere_keyframe;   // sphere used to display the key positions
curve_drawable polygon_keyframe; // Display the segment between key positions
curve_drawable rope;
trajectory_drawable trajectory;  // Temporary storage and display of the interpolated trajectory

hierarchy_mesh_drawable hierarchy;
mesh_drawable terrain;
mesh_drawable vagues;
struct rope* rp;

float gScale = 0.7f;
float carlength = 3.0f*gScale;
float carwidth = 1.5f*gScale;
float carheight = 1.2f*gScale;
float wheel_radius = 0.45f*gScale;

vec3 faxisShift = vec3(carlength / 3, 0, -wheel_radius *2/ 3);
vec3 raxisShift = vec3(-carlength / 3, 0, -wheel_radius *2/ 3);
vec3 rightWheelAxisShift = vec3(0, -wheel_radius / 2 - carwidth / 2, 0);
vec3 FRWheelShift = faxisShift + rightWheelAxisShift;
vec3 FRWheelTouch = FRWheelShift + vec3(0, 0, wheel_radius*3/2);
vec3 FMWheelTouch = faxisShift - vec3(0, 0, wheel_radius*3/2);
float centerToTouch=-FMWheelTouch.z;

float bigRadius = 8.0f;
float x, y,z;

float deltaT;


int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
	std::cout<<"Initialize data ..."<<std::endl;
	initialize_data();

	std::cout<<"Start animation loop ..."<<std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();
		user.fps_record.update();
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();
		if(user.fps_record.event) {
			std::string const title = "VCL Display - "+str(user.fps_record.fps)+" fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI",NULL,ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if(user.gui.display_frame) draw(user.global_frame, scene);

		display_interface();
		display_frame();


		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


void initialize_data()
{
	// Basic setups of shaders and camera
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_single_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = opengl_texture_to_gpu(image_raw{ 1,1,image_color_type::rgba,{255,255,255,255} });
	curve_drawable::default_shader = shader_single_color;

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({-0.5f,10.0f,1}, {0,0,0}, {0,0,1});
	
	// Definition of the initial data
	//--------------------------------------//
	// Key positions
	key_positions = { {0,10,5}, {5,10,5},{-10,1,5}, {-5,-10,5}, {0,10,5}, {5,10,5} };
	// Key times
	key_times = { 0.0f, 2.0f, 2.5f, 3.0f, 3.5f, 4.1f};
	float key_times_max = key_times[key_times.size() - 1];

	for (int i = 0; i < key_positions.size(); i++) {
		totalLength += norm(key_positions[(i + 1) % key_positions.size()] - key_positions[i]);
	}
	for (int i = 1; i < key_positions.size(); i++) {
		key_times[i] = key_times[i - 1] + norm(key_positions[i] - key_positions[i - 1]) * key_times_max / totalLength;
	}

	// Set timer bounds
	//  You should adapt these extremal values to the type of interpolation
	size_t const N = key_times.size();
	timer.t_min = key_times[0];    // Start the timer at the first time of the keyframe
	timer.t_max = key_times[N - 2];  // Ends the timer at the last time of the keyframe
	timer.t = timer.t_min;
	deltaT= timer.t_max - timer.t_min;


	// Initialize drawable structures
	sphere_keyframe = mesh_drawable(mesh_primitive_sphere(0.2f));
	sphere_current = mesh_drawable(mesh_primitive_sphere(0.3f));
	sphere_keyframe.shading.color = { 0,0,1 };
	sphere_current.shading.color = { 1,1,0 };

	polygon_keyframe = curve_drawable(key_positions);
	polygon_keyframe.color = { 0,0,0 };
	rp = new struct rope(10, 10.0f, 1.0f, 0.1f, { 0,0,0 }, { 5,5,0 });
	rope = curve_drawable(rp->n_positions);
	//Main body
	mesh mbody = ellipsoid_with_texture({ carheight / 2,carwidth / 2,carlength / 2, }, { 0,0,0 });
	mesh mspoiler;
	mspoiler.push_back(mesh_primitive_quadrangle({ -carlength *1 / 10,-carwidth / 5,0 }, { -carlength *4/ 10,-carwidth / 5,0 }, { -carlength *6/10,-carwidth / 5,carheight*20.0f/30.0f}, { -carlength *5 / 10,-carwidth / 5,carheight*17.0f/30.0f }));
	mspoiler.push_back(mesh_primitive_quadrangle({ -carlength * 1 / 10,carwidth / 5,0 }, { -carlength * 4 / 10,carwidth / 5,0 }, { -carlength * 6 / 10,carwidth / 5,carheight * 20.0f / 30.0f }, { -carlength * 5 / 10,carwidth / 5,carheight * 17.0f / 30.0f }));
	mspoiler.push_back(mesh_primitive_quadrangle({ -carlength * 6 / 10,-carwidth*2/3,carheight * 20.0f / 30.0f }, { -carlength * 6 / 10,carwidth *2/ 3,carheight * 20.0f / 30.0f }, { -carlength * 5 / 10,carwidth * 1 / 2,carheight * 17.0f / 30.0f }, { -carlength * 5 / 10, -carwidth * 1 / 2, carheight * 17.0f / 30.0f }));
	mesh_drawable cabin = mesh_drawable(mesh_primitive_ellipsoid({ carlength / 4, carwidth / 4, carheight / 3 }));
	mesh_drawable spoiler = mesh_drawable(mspoiler);
	spoiler.shading.color = { 0.9f,0,0 };
	spoiler.shading.phong.specular = 0.1f;

	cabin.shading.color = { 116.0f / 255.0f, 208.0f / 255.0f, 241.0f / 255.0f };
	cabin.shading.phong.specular = 5.0f;
	cabin.shading.phong.specular_exponent = 5.0f;
	mesh_drawable body = mesh_drawable(mbody);
	body.shading.color = { 0.5f,0.5f,0.5f };
	body.shading.phong.specular = 0.1f;
	body.transform.rotate = rotation({ 0,1,0 }, pi / 2);

	mesh_drawable wheel = mesh_drawable(tore_with_texture(wheel_radius, wheel_radius / 2, { 0,0,0 }));
	wheel.transform.rotate = vcl::rotation({ 1,0,0 }, pi/2);
	wheel.shading.phong.specular = 0.0f;
	wheel.shading.color = { 0.5f,0.5f,0.5f };
	mesh_drawable rim = mesh_drawable(cylinder_with_texture(0.9f*wheel_radius, { 0,0,0 }, 0.9f*wheel_radius));
	rim.transform.rotate = vcl::rotation({ 1,0,0 }, pi / 2);

	//mesh_drawable rimDisc = mesh_drawable(disc_with_texture(0.9f * wheel_radius, { 0,0,0 }));
	//rimDisc.transform.rotate = vcl::rotation({ 1,0,0 }, pi / 2);

	mesh_drawable axis = mesh_drawable(mesh_primitive_cylinder(0.4f * wheel_radius, { 0,-carwidth/2,0 }, { 0,carwidth/2,0 }, 20, 40, true));

	wheel.texture = opengl_texture_to_gpu(image_load_png("../Assets/rubber.png"),
		GL_REPEAT,
		GL_MIRRORED_REPEAT);
	rim.texture = opengl_texture_to_gpu(image_load_png("../assets/rim.png"),
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT);
	body.texture= opengl_texture_to_gpu(image_load_png("../assets/rivets.png"),
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT);
	terrain.texture=opengl_texture_to_gpu(image_load_png("../assets/texture_grass.png"),
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT);
	

	// Build the hierarchy:
	// ------------------------------------------- //
    // Syntax to add element
    //   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy is the body
	hierarchy.add(mesh_drawable(), "laceBody");
	hierarchy.add(mesh_drawable(), "pitchBody", "laceBody");
	hierarchy.add(body, "rollBody", "pitchBody");
	hierarchy.add(spoiler, "Spoiler","rollBody");

	cabin.transform.translate = { carlength / 5,0,carheight / 4 };
	cabin.transform.rotate= rotation({ 0,1,0 }, pi / 10);

	hierarchy.add(cabin, "Cabin", "rollBody");
	hierarchy.add(axis, "F_axis", "rollBody", faxisShift);
	hierarchy.add(axis, "R_axis", "rollBody", raxisShift);

	hierarchy.add(rim, "FR_rim", "F_axis", rightWheelAxisShift);
	hierarchy.add(rim, "FL_rim", "F_axis", -rightWheelAxisShift);
	hierarchy.add(rim, "RR_rim", "R_axis", rightWheelAxisShift);
	hierarchy.add(rim, "RL_rim", "R_axis", -rightWheelAxisShift);

	hierarchy.add(mesh_drawable(), "FR_touch", "F_axis", rightWheelAxisShift - vec3(0, 0, 1.5f * wheel_radius));
	hierarchy.add(mesh_drawable(), "RR_touch", "R_axis", rightWheelAxisShift- vec3(0, 0, 1.5f * wheel_radius));
	hierarchy.add(mesh_drawable(), "FL_touch", "F_axis", -rightWheelAxisShift- vec3(0, 0, 1.5f * wheel_radius));
	hierarchy.add(mesh_drawable(), "RL_touch", "R_axis", -rightWheelAxisShift- vec3(0, 0, 1.5f * wheel_radius));

	wheel.transform.rotate = vcl::rotation({ 1,0,0 }, pi / 2);
	hierarchy.add(wheel, "FR_wheel", "FR_rim");
	hierarchy.add(wheel, "RR_wheel", "RR_rim");
	hierarchy.add(wheel, "FL_wheel", "FL_rim");
	hierarchy.add(wheel, "RL_wheel", "RL_rim");

}



void display_frame()
{
	// TRAJECTORY PART

	// Sanity check
	assert_vcl(key_times.size() == key_positions.size(), "key_time and key_positions should have the same size");

	// Update the current time
	timer.update();
	float const t = timer.t;
	terrain = mesh_drawable(create_terrain(false, t));
	terrain.shading.phong.specular = 0.2f;
	terrain.shading.color = { 0,0,0.8f };

	vagues = mesh_drawable(create_terrain(true, t-dt));
	vagues.shading.phong.specular = 0.0f;
	vagues.shading.color = { 1,1,1 };

	if (t < timer.t_min + 0.1f) // clear trajectory when the timer restart
		trajectory.clear();
	//update_hfixed_rope(rp, t);
	// Display the key positions
	if (user.gui.display_keyposition)
		display_keypositions(sphere_keyframe, key_positions, scene);

	// Display the polygon linking the key positions
	if (user.gui.display_polygon)
		draw(polygon_keyframe, scene);

	// Compute the interpolated position
	vec3 const p = interpolation(t, key_positions, key_times);
	vec3 const p_tdt = interpolation(t + dt, key_positions, key_times);
	vec3 const deltaP = p_tdt - p;
	
	/***
	// Display the interpolated position
	sphere_current.transform.translate = p;
	draw(sphere_current, scene);
	***/
	

	/** *************************************************************  **/
    /** Compute the (animated) transformations applied to the elements **/
    /** *************************************************************  **/

	// update the global coordinates
	x =bigRadius *std::sin(2 * pi * t / deltaT);
	y =bigRadius *std::cos(2 * pi * t / deltaT);

	hierarchy["laceBody"].transform.translate = { p.x,p.y,0 };
	hierarchy["laceBody"].transform.rotate = rotation({ 0,0,1 }, std::atan2(deltaP.y, deltaP.x));
	hierarchy["pitchBody"].transform.rotate = rotation({ 0,1,0 },0);

	hierarchy["rollBody"].transform.rotate = rotation({ 1,0,0 },0);


	hierarchy["R_axis"].transform.rotate = rotation({ 1,0,0 }, 0);
	hierarchy["F_axis"].transform.rotate = rotation({ 1,0,0 }, 0);

	hierarchy.update_local_to_global_coordinates();


	vec3 FR_wheel_pos = evaluate_terrain_bruit(hierarchy["FR_touch"].global_transform.translate, t);
	vec3 FL_wheel_pos = evaluate_terrain_bruit(hierarchy["FL_touch"].global_transform.translate, t);
	vec3 RR_wheel_pos = evaluate_terrain_bruit(hierarchy["RR_touch"].global_transform.translate, t);
	vec3 RL_wheel_pos = evaluate_terrain_bruit(hierarchy["RL_touch"].global_transform.translate, t);

	vec3 F_wheels_pos = (FR_wheel_pos + FL_wheel_pos )/ 2;
	vec3 R_wheels_pos = (RR_wheel_pos + RL_wheel_pos) / 2;

	vec3 Ri_wheels_pos = (FR_wheel_pos + RR_wheel_pos) / 2;
	vec3 Le_wheels_pos = (FL_wheel_pos + RL_wheel_pos) / 2;

	z = 1.0f*centerToTouch + (F_wheels_pos+R_wheels_pos).z/2;
	float deltaZFaxis = FR_wheel_pos.z - FL_wheel_pos.z;
	float deltaZRaxis = RR_wheel_pos.z - RL_wheel_pos.z;
	float FaxisRollAngle = std::asin(double(deltaZFaxis / carwidth));
	float RaxisRollAngle = std::asin(double(deltaZRaxis / carwidth));
	double bodyRollAngle = (FaxisRollAngle + RaxisRollAngle) / 2;
	double bodyPitchAngle = std::asin(double((F_wheels_pos - R_wheels_pos).z / carlength));
	

	hierarchy["pitchBody"].transform.translate = vec3(0, 0, z);
	hierarchy["pitchBody"].transform.rotate = rotation({ 0,1,0 }, -bodyPitchAngle);

	hierarchy["rollBody"].transform.rotate = rotation({ 1,0,0 }, -bodyRollAngle);


	hierarchy["R_axis"].transform.rotate = rotation({ 1,0,0 }, (bodyRollAngle-RaxisRollAngle));
	hierarchy["F_axis"].transform.rotate = rotation({ 1,0,0 }, (bodyRollAngle- FaxisRollAngle));
	
	
	hierarchy["FR_rim"].transform.rotate = rotation({ 0,1,0 }, 2 * pi * bigRadius / wheel_radius * t / deltaT);
	hierarchy["FL_rim"].transform.rotate = rotation({ 0,1,0 }, 2 * pi * bigRadius / wheel_radius * t / deltaT);
	hierarchy["RR_rim"].transform.rotate = rotation({ 0,1,0 }, 2 * pi * bigRadius / wheel_radius * t / deltaT);
	hierarchy["RL_rim"].transform.rotate = rotation({ 0,1,0 }, 2 * pi * bigRadius / wheel_radius * t / deltaT);
	
	//hierarchy["laceBody"].transform.translate = { -0.5f,10.0f,1 };
	hierarchy.update_local_to_global_coordinates();
	
	// Display the trajectory
	trajectory.visual.color = { 1,0,0 };
	trajectory.add(hierarchy["pitchBody"].global_transform.translate, t);
	draw(trajectory, scene);

	// display the hierarchy
	draw(hierarchy, scene);
	
	if (user.gui.display_surface) {
		draw(terrain, scene);
		draw(vagues, scene);
	}
	if (user.gui.display_wireframe) {
		draw_wireframe(hierarchy, scene);
		draw_wireframe(terrain, scene);
	}
}

void display_interface()
{
	ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
	ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 2.0f);
	ImGui::Checkbox("Frame", &user.gui.display_frame);
	ImGui::Checkbox("Display key positions", &user.gui.display_keyposition);
	ImGui::Checkbox("Display polygon", &user.gui.display_polygon);
	ImGui::Checkbox("Display trajectory", &user.gui.display_trajectory);
	bool new_size = ImGui::SliderInt("Trajectory size", &user.gui.trajectory_storage, 2, 500);
	ImGui::Checkbox("Surface", &user.gui.display_surface);
	ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);
	if (new_size) {
		trajectory.clear();
		trajectory = trajectory_drawable(user.gui.trajectory_storage);
	}
}


void window_size_callback(GLFWwindow* , int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi /180.0f;
	float const z_min = 0.1f;
	float const z_max = 100.0f;
	scene.projection = projection_perspective(fov, aspect, z_min, z_max);
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{

	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if (!user.cursor_on_gui && !state.key_shift) {
		if (state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if (state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1 - p0);
		if (state.mouse_click_right)
			camera.manipulator_scale_distance_to_center((p1 - p0).y);
	}
	user.mouse_prev = p1;
}




