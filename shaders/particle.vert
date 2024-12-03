#version 430 core

layout (location = 0) in vec3 vert_pos;

out vec2 frag_uv;

mat4 scale = {
	{ 0.05f, 0.00f, 0.00f, 0.0f },
	{ 0.00f, 0.05f, 0.00f, 0.0f },
	{ 0.00f, 0.00f, 0.05f, 0.0f },
	{ 0.00f, 0.00f, 0.00f, 1.0f },
};

mat4 translate = {
	{ 1.0f, 0.0f, 0.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f },
};

void main()
{
	frag_uv = vert_pos.xy;

	int x = gl_InstanceID % 10;
	int y = gl_InstanceID / 10;

	translate[3][0] = x / 10.0f * 2.0f - 1.0f + 0.1f;
	translate[3][1] = y / 10.0f * 2.0f - 1.0f + 0.1f;

	gl_Position = translate * scale * vec4(vert_pos, 1.0f);
}
