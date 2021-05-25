#include "vcl/vcl.hpp"
#include "scene_helper.hpp"
#include <iostream>
#include <list>
#include <time.h>

#include "physics.hpp"
#include <terrain.hpp>

using namespace vcl;
float offset = 0.9f;

/** Compute spring force applied on particle pi from particle pj */
vec3 spring_force(vec3 const& p_i, vec3 const& p_j, float L_0, float K)
{
	return K * ((p_j - p_i) - L_0 * (p_j - p_i) / norm((p_j - p_i)));
}

void update_rope(struct rope* rp, float t, bool free) {
	std::vector<particle_structure* > particules = rp->points;
	//std::vector<vec3> n_positions = rp.n_positions;
	/*std::vector<vec3> n_speeds = rp.n_speeds;*/
	float mu = rp->mu;
	float K = rp->K;
	float L0 = rp->L0;
	int const d = free ? 0 : 1;
	for (unsigned long i = d; i < particules.size(); ++i)
	{
		particle_structure* it = particules[i];
		float m = it->m * ((i == particules.size()-1) ? 50 :1);
		vec3 fh_spring = { 0,0,0 };
		vec3 fh_damping = { 0,0,0 };
		vec3 fd_spring = { 0,0,0 };
		vec3 fd_damping = { 0,0,0 };
		if (i > 0) {
			particle_structure* h = particules[i-1];
			fh_spring = spring_force(it->p, h->p, L0, K);
			fh_damping = -mu * (it->v - h->v);
		}
		if (i < particules.size()-1) {
			particle_structure* d = particules[i+1];
			fd_spring = spring_force(it->p, d->p, L0, K);
			fd_damping = -mu * (it->v - d->v);
		}
		vec3 const f_weight = m * g;
		vec3 const f = f_weight + fh_spring + fh_damping + fd_spring + fd_damping;

		// Numerical Integration (Verlet)
		{
			rp->n_speeds[i] = (it->v) + dt * f / m;
			rp->n_positions[i] = it->p + dt * it->v;
		}
	}
	for (unsigned long i = 0; i < particules.size(); i++) {
		particle_structure* it = particules[i];
		it->p = rp->n_positions[i];
		it->v = rp->n_speeds[i];
	}
}




void update_positions(struct rope* rp, float t, particle_structure* surfeur) {
	update_rope(rp, t, false);
	float hauteur = evaluate_terrain_bruit((rp->points[rp->N-1]->p.x) / 20 + 0.5f, (rp->points[rp->N - 1]->p.y) / 20 + 0.5f, t, 0.0f).z;
	if (hauteur + offset > rp->points[rp->N - 1]->p.z) {
		rp->points[rp->N - 1]->v += vec3(0, 0, (hauteur + offset - rp->points[rp->N - 1]->p.z) / dt);
		rp->points[rp->N - 1]->p.z = hauteur + offset;
	}
}