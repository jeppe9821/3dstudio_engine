#version 430 core

layout(location = 0) in vec4 position;

uniform mat4 view;
out vec3 vs_color;

void main()
{
	vs_color = mix(vec3(0.7, 0.0, 0.5), vec3(0.0, 0.4, 0.7), smoothstep(0,1000,distance(position, vec4(position*0.1))));
	gl_Position = view * vec4(position.xyz, 1.0);
}