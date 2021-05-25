#include "vcl/vcl.hpp"
#include "scene_helper.hpp"

#pragma once

vcl::vec3 const g = { 0,0,-9.81f }; // gravity
float const dt = 0.01f;
float const dl = 0.01f;

struct particle_structure
{
	vcl::vec3 p; // Position
	vcl::vec3 v; // Speed
	float m;
	particle_structure(vcl::vec3 pos, vcl::vec3 speed, float masse) {
		p = pos;
		v = speed;
		m = masse;
	}
};

struct rope {
	int N;
	float K;
	float mu;
	float m;
	float L0;
	std::vector<particle_structure* > points;
	std::vector<vcl::vec3 > n_positions;
	std::vector<vcl::vec3 > n_speeds;
	rope(float N, float K, float mu, float m, vcl::vec3 p0, vcl::vec3 pf) {
		this->N = N;
		this->K = K;
		this->mu = mu;
		this->m = m;
		this->L0 = norm((pf-p0))/N;
		for (int i = 0; i < N; i++) {
			points.push_back(new particle_structure(p0+i*(pf-p0)/N, { 0,0,0 }, m));
			n_positions.push_back(p0 + i * (pf - p0) / N);
			n_speeds.push_back({ 0,0,0 });
		}
	}
};

vcl::vec3 spring_force(vcl::vec3 const& p_i, vcl::vec3 const& p_j, float L_0, float K);
void update_rope(struct rope* rp, float t, bool free);
void update_positions(struct rope* rp, float t, particle_structure* surfeur);