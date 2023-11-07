#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 vs_color[];
out vec3 fs_color;
out vec2 fs_txCoords;

uniform mat4 projection;

const vec2 coords[4] =
{
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 1.0)
};

void main()
{
	fs_color = vs_color[0];

	for (int i = 0; i < 4; i++)
	{
		fs_txCoords = coords[i];
		vec4 pos = gl_in[0].gl_Position;
		pos.xy += 0.5 * (coords[i] - vec2(0.5));
		gl_Position = projection * pos;
		EmitVertex();
	}
}