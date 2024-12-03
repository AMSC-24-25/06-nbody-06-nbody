#version 430 core

#define RADIUS 0.02f

struct Particle {
	vec3 position;
	float mass;
};

layout (location = 0) in vec3 vert_pos;
// SSBO used to gather particles data
layout (std430, binding = 0) buffer Particles {
	Particle particles[];
};

out vec2 frag_uv;

void main()
{
	// Forwards xy position to allow interpolation
	frag_uv = vert_pos.xy;

	float scale = min(RADIUS * particles[gl_InstanceID].mass, 0.1f);
	vec3 translate = particles[gl_InstanceID].position;

	gl_Position = vec4(scale * vert_pos + translate, 1.0f);
}
