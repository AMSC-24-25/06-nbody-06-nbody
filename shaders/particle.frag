#version 430 core

in vec2 frag_uv;
out vec4 frag_color;

void main()
{
	float r = dot(frag_uv, frag_uv);
	if (r > 1.0f) {
		discard;
	}

	// Simple sphere shading
	frag_color = vec4(1 - r, 1 - r, 1 - r, 1.0f);
}
