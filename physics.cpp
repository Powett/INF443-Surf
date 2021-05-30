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
	return (norm(p_j - p_i) > L_0) ? K * ((p_j - p_i) - L_0 * (p_j - p_i) / norm(p_j - p_i)) : vec3(0, 0, 0);
}

void update_rope(struct rope* rp, float t, bool free) {
	std::vector<particle_structure* > particules = rp->points;
	float mu = rp->mu;
	float K = rp->K;
	float L0 = rp->L0;
	//if the rope isn't free we don't update the first particle of the rope and start from i=1
	int const d = free ? 0 : 1;
	for (unsigned long i = d; i < particules.size(); ++i)
	{
		particle_structure* it = particules[i];
		float m = it->m * ((i == particules.size() - 1) ? 100.0f : 1.0f);
		vec3 fh_spring = { 0,0,0 };
		vec3 fh_damping = { 0,0,0 };
		vec3 fd_spring = { 0,0,0 };
		vec3 fd_damping = { 0,0,0 };
		vec3 f = { 0,0,0 };
		//calculate the force by the particle in position i-1
		if (i > 0) {
			particle_structure* h = particules[i - 1];
			fh_spring = spring_force(it->p, h->p, L0, K);
			fh_damping = -mu * (it->v - h->v);
		}
		//calculate the force by the particle in position i+1
		if (i < particules.size() - 1) {
			particle_structure* d = particules[i + 1];
			fd_spring = spring_force(it->p, d->p, L0, K);
			fd_damping = -mu * (it->v - d->v);
		}

		vec3 const f_weight = m * g;
		f += f_weight + fh_spring + fh_damping + fd_spring + fd_damping;

		// Numerical Integration (Verlet)
		{
			rp->n_speeds[i] = ((it->v) + dt * f / m);
			rp->n_positions[i] = it->p + dt * it->v;
			float hauteur = evaluate_terrain_bruit(it->p, t, 0.0f).z;
			//The las particle of the rope is the surfer, he doesn't sink into the water. We update his position and speed accordingly
			if ( (i == particules.size() - 1) && (rp->n_positions[i].z - offset - hauteur < 0) ) {
				rp->n_speeds[i].z += (offset + hauteur - rp->n_positions[i].z)/dt;
				rp->n_positions[i].z = offset + hauteur;
			}
		}
	}
	for (unsigned long i = d; i < particules.size(); i++) {
		particle_structure* it = particules[i];
		it->p = rp->n_positions[i];
		it->v = rp->n_speeds[i];
	}
}

//The rope used for the physics calculations is only one segment, we render a more natural rope by using the calculated points as reference.
void update_display_rope(struct rope* rp, vec3 A, vec3 B) {
	float size = rp->points.size();
	for (float i = 0; i < size; ++i) {
		 float const u = i / (size - 1);
		 rp->n_positions[i] = vec3(u * A.x + (1 - u) * B.x, u * A.y + (1 - u) * B.y, u * A.z + (1 - u) * B.z + 5 * u * (u-1));
		 rp->points[i]->p = rp->n_positions[i];
	}
}