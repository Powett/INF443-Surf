#include "vcl/vcl.hpp"
#include "interpolation.hpp"
#include "scene_helper.hpp"
#include "model_textures.hpp"
#include "terrain.hpp"
#include "meshes.hpp"
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

int recalcterrain = 0;


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);


void initialize_data();
void display_interface();
void display_frame();


mesh_drawable sphere_current;    // sphere used to display the interpolated value
mesh_drawable sphere_keyframe;   // sphere used to display the key positions
curve_drawable polygon_keyframe; // Display the segment between key positions
curve_drawable rope;

mesh_drawable board;

hierarchy_mesh_drawable hierarchy;
mesh_drawable terrain;
mesh_drawable vagues;
mesh_drawable test_sphere;
struct rope* rp;

float bigRadius = 10.0f;

float gScale = 0.3f;
float bodyHeight = 1.0f * gScale;
float bodyWidth = 0.5f * gScale;

float feetSpread;
float thighLength;
float effectifeThighLength;
float averageLegLength;

float waveH = 0.7f;
float terrain_bruit = 0.1f;
float vagues_bruit = 0.2f;
float physics_bruit = 0.0f;

float x, y, z;

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

	std::cout << "Initialize data ..." << std::endl;
	initialize_data();

	std::cout << "Start animation loop ..." << std::endl;
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
		if (user.fps_record.event) {
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if (user.gui.display_frame) draw(user.global_frame, scene);

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
	scene.camera.look_at({ -0.5f,10.0f,1 }, { 0,0,0 }, { 0,0,1 });

	// Definition of the initial data
	//--------------------------------------//
	// Key positions
	key_positions = { {0,10,5}, {5,10,5},{-10,1,5}, {-5,-10,5}, {0,10,5}, {5,10,5} };
	// Key times
	key_times = { 0.0f, 2.0f, 2.5f, 3.0f, 3.5f, 10.1f };
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
	deltaT = timer.t_max - timer.t_min;


	// Initialize drawable structures
	sphere_keyframe = mesh_drawable(mesh_primitive_sphere(0.2f));
	sphere_current = mesh_drawable(mesh_primitive_sphere(0.3f));
	test_sphere = mesh_drawable(mesh_primitive_sphere(bodyWidth / 4));
	test_sphere.shading.color = { 0,0,1.0f };
	sphere_keyframe.shading.color = { 0,0,1 };
	sphere_current.shading.color = { 1,1,0 };

	polygon_keyframe = curve_drawable(key_positions);
	polygon_keyframe.color = { 0,0,0 };
	rp = new struct rope(10, 10.0f, 1.0f, 0.1f, { 0,0,0 }, { 5,5,0 });
	rope = curve_drawable(rp->n_positions);


	board = mesh_drawable(create_surfboard(7.0f * bodyHeight, 3.0f * bodyWidth, 0.5f * bodyWidth));
	thighLength = 0.6f * bodyHeight;
	averageLegLength = thighLength * 3.0f / 2.0f;
	//Main body


	


	mesh_drawable body = mesh_drawable(mesh_primitive_ellipsoid({ bodyWidth,bodyWidth,bodyHeight }));
	mesh_drawable head = mesh_drawable(mesh_primitive_sphere(bodyWidth));
	head.transform.rotate = rotation({ 0,0,1 }, pi / 2);
	mesh_drawable knee = mesh_drawable(mesh_primitive_sphere(0.5 * bodyWidth));
	mesh_drawable shoulder = mesh_drawable(mesh_primitive_sphere(0.7f * bodyWidth));
	mesh_drawable hip = mesh_drawable(mesh_primitive_sphere(0.6f * bodyWidth));
	mesh_drawable ankle = mesh_drawable(mesh_primitive_sphere(0.4f * bodyWidth));
	mesh_drawable elbow = mesh_drawable(mesh_primitive_sphere(0.4f * bodyWidth));
	mesh_drawable thigh = mesh_drawable(mesh_primitive_ellipsoid({ 0.7f*bodyWidth,0.7f*bodyWidth,thighLength/cos(pi/10) }));
	mesh_drawable leg = mesh_drawable(mesh_primitive_ellipsoid({ 0.6f*bodyWidth,0.6f*bodyWidth,0.6f*bodyHeight }));
	mesh_drawable arm = mesh_drawable(mesh_primitive_ellipsoid({ 0.5f*bodyWidth,0.5f*bodyWidth,0.5f*bodyHeight }));
	mesh_drawable forearm = mesh_drawable(mesh_primitive_ellipsoid({ 0.4f*bodyWidth,0.4f*bodyWidth,0.4f*bodyHeight }));
	mesh_drawable foot = mesh_drawable(mesh_primitive_ellipsoid({ 0.4f * bodyWidth,0.4f * bodyWidth,0.4f * bodyHeight }));

	arm.transform.rotate = rotation({ 0,1,0 }, pi / 2);
	forearm.transform.rotate = rotation({ 0,1,0 }, pi / 2);
	foot.transform.rotate = rotation({ 1,0,0 }, pi / 2);
	head.texture = opengl_texture_to_gpu(image_load_png("../Assets/face.png"),
		GL_REPEAT,
		GL_MIRRORED_REPEAT);

	hierarchy.add(body, "Body");
	hierarchy.add(head, "Head", "Body", { 0,0,1.4f*bodyHeight });

	hierarchy.add(shoulder, "RShoulder", "Body", { bodyWidth,0,0.6f*bodyHeight });
	hierarchy.add(shoulder, "LShoulder", "Body", { -bodyWidth,0,0.6f*bodyHeight });
	hierarchy.add(arm, "RArm", "RShoulder", { 0.6f * bodyHeight,0,0 });
	hierarchy.add(arm, "LArm", "LShoulder", { -0.6f * bodyHeight,0, 0});
	hierarchy.add(elbow, "RElbow", "RArm", { 0.5f * bodyHeight,0, 0});
	hierarchy.add(elbow, "LElbow", "LArm", { -0.5f * bodyHeight,0, 0});
	hierarchy.add(forearm, "RForearm", "RElbow", { 0.4f * bodyHeight,0,0 });
	hierarchy.add(forearm, "LForearm", "LElbow", { -0.4f * bodyHeight,0, 0});

	

	hierarchy.add(hip, "RHip", "Body", { 0.8f*bodyWidth,0,-0.7f * bodyHeight });
	hierarchy.add(hip, "LHip", "Body", { -0.8f*bodyWidth,0,-0.7f * bodyHeight });
	thigh.transform.rotate = rotation({ 0,1,0 }, -pi / 10);
	hierarchy.add(thigh, "RThigh", "RHip", { thighLength * sin(pi / 10),0,-thighLength*cos(pi/10)});
	thigh.transform.rotate = rotation({ 0,1,0 },pi / 10);

	hierarchy.add(thigh, "LThigh", "LHip", { -thighLength * sin(pi / 10) ,0,- thighLength * cos( pi / 10) });
	hierarchy.add(knee, "RKnee", "RThigh", { thighLength*sin(pi/10),0,-thighLength*cos(pi/10) });
	hierarchy.add(knee, "LKnee", "LThigh", { -thighLength * sin(pi / 10),0,-thighLength * cos(pi / 10) });
	hierarchy.add(leg, "LLeg", "LKnee", { 0,0,-0.6f * bodyHeight });
	hierarchy.add(leg, "RLeg", "RKnee", { 0,0,-0.6f * bodyHeight });
	hierarchy.add(ankle, "RAnkle", "RLeg", { 0, 0,-0.6 * bodyHeight });
	hierarchy.add(ankle, "LAnkle", "LLeg", { 0, 0,-0.6 * bodyHeight });
	hierarchy.add(foot, "RFoot", "RAnkle", {0, 0.3f*bodyHeight,0});
	hierarchy.add(foot, "LFoot", "LAnkle", {0, 0.3f*bodyHeight,0});
	board.transform.rotate = rotation{ {0,0,1}, -pi/2 };
	hierarchy.add(board, "Board", "RFoot", { 0.8f * bodyWidth,0,+0.1f*bodyWidth});
	hierarchy.update_local_to_global_coordinates();

	feetSpread = std::abs((hierarchy["RFoot"].global_transform.translate - hierarchy["LFoot"].global_transform.translate).x);
	effectifeThighLength = (hierarchy["LHip"].global_transform.translate - hierarchy["LKnee"].global_transform.translate).z/2;
	terrain = mesh_drawable(create_terrain_bruit(0, terrain_bruit));
	terrain.shading.phong.specular = 0.2f;
	terrain.shading.alpha = 0.2f;
	terrain.shading.color = { 0.5f,0.5f,0.95f } ;

	vagues = mesh_drawable(create_terrain_bruit(0, vagues_bruit));
	vagues.shading.phong.specular = 0.0f;
	/*terrain.shading.phong.diffuse = 0.8f;*/
	terrain.shading.phong.ambient = 0.8f;
	vagues.shading.alpha = 0.8f;
	vagues.shading.color = { 0,0,1.0f };
}



void display_frame()
{
		
	/***
	// Sanity check
	assert_vcl(key_times.size() == key_positions.size(), "key_time and key_positions should have the same size");

	// Update the current time
	

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
	***/
	
	/** *************************************************************  **/
	/** Compute the (animated) transformations applied to the elements **/
	/** *************************************************************  **/

	timer.update();
	float const t = timer.t;
	recalcterrain++;
	if (recalcterrain > 1) {
		terrain.update_position(update_terrain(waveH, t, terrain_bruit));
		vagues.update_position(update_terrain(waveH, t, vagues_bruit));
		recalcterrain = 0;
	}

	float x = 5.f * cos(2 * pi * t);
	float y = 5.f * sin(2 * pi * t);
	float x2 = 5.f * cos(2 * pi *( t+dt));
	float y2 = 5.f * sin(2 * pi * (t + dt));
	vec3 p = evaluate_terrain_bruit({ x,y,0 }, t, physics_bruit);
	vec3 dp = (evaluate_terrain_bruit({ x2,y2,0 }, t + dt, physics_bruit) - p);
	vec3 unit_direction = dp / norm(dp);

	vec3 normal_direction = { unit_direction.y, -unit_direction.x,0 };

	
	vec3 nose = evaluate_terrain_bruit(p + unit_direction * dl, t, physics_bruit);
	vec3 tail = evaluate_terrain_bruit(p - unit_direction * dl, t, physics_bruit);
	vec3 front = evaluate_terrain_bruit(p + normal_direction * dl, t, physics_bruit);
	vec3 back = evaluate_terrain_bruit(p - normal_direction * dl, t, physics_bruit);


	float yaw = std::atan2(dp.y, dp.x);
	hierarchy["Body"].transform.translate = p;
	hierarchy["Body"].transform.rotate = rotation({ 0,0,1 }, yaw);
	hierarchy.update_local_to_global_coordinates();

	vec3 frontFoot = evaluate_terrain_bruit(p + hierarchy["RFoot"].global_transform.translate - hierarchy["Body"].global_transform.translate, t, physics_bruit);
	vec3 backFoot = evaluate_terrain_bruit(p + hierarchy["LFoot"].global_transform.translate - hierarchy["Body"].global_transform.translate, t, physics_bruit);
	float deltaH = (frontFoot - backFoot).z;
	
	float pitch = atan2((frontFoot - backFoot).z, feetSpread/2);
	hierarchy["RFoot"].transform.rotate = rotation({ 0,1,0 }, -pitch);
	
	float roll = atan2(deltaH, feetSpread);
	float arg = averageLegLength / (2* thighLength) - deltaH / (4*thighLength);
	arg = (arg >= 0.0f) ? ((arg <= 1.0f) ? arg : 1.0f) : 0.0f;
	float alpha = acos(arg);
	float beta =-2*alpha;
	hierarchy["RHip"].transform.rotate = rotation({ 1,0,0 }, alpha);
	hierarchy["RKnee"].transform.rotate = rotation({ 1,0,0 }, beta);
	hierarchy["RAnkle"].transform.rotate = rotation({ 1,0,0 }, -roll -beta/2 );

	arg = averageLegLength / (2*thighLength) + deltaH / (4*thighLength);
	arg = (arg >= 0.0f) ? ((arg <= 1.0f) ? arg : 1.0f) : 0.0f;
	alpha = acos(arg);
	beta = -2 * alpha;
	hierarchy["LHip"].transform.rotate = rotation({ 1,0,0 }, alpha);
	hierarchy["LKnee"].transform.rotate = rotation({ 1,0,0 }, beta);
	hierarchy["LAnkle"].transform.rotate = rotation({ 1,0,0 }, -roll-beta/2);
	hierarchy.update_local_to_global_coordinates();

	
	
	/*hierarchy["Body"].transform.translate = { 0,0,0 };
	hierarchy["Body"].transform.rotate = rotation();*/
	
	//hierarchy["Board"].transform.translate = (-hierarchy["RFoot"].global_transform.translate + hierarchy["LFoot"].global_transform.translate) / 2;
	hierarchy.update_local_to_global_coordinates();
	hierarchy["Body"].transform.translate = (1.0f + 0.7f*waveH) * (hierarchy["Body"].global_transform.translate - hierarchy["Board"].global_transform.translate) + evaluate_terrain_bruit(hierarchy["Board"].global_transform.translate, t, 0.0f);
	hierarchy.update_local_to_global_coordinates();

	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at(vec3({0, 0, 2.0f}), p, { 0,0,1 });

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
	ImGui::SliderFloat("Waves Height", &waveH, 0.0f, 2.0f);
	ImGui::Checkbox("Frame", &user.gui.display_frame);
	ImGui::Checkbox("Display key positions", &user.gui.display_keyposition);
	ImGui::Checkbox("Display polygon", &user.gui.display_polygon);
	ImGui::Checkbox("Surface", &user.gui.display_surface);
	ImGui::Checkbox("Wireframe", &user.gui.display_wireframe);
}


void window_size_callback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	float const fov = 50.0f * pi / 180.0f;
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




