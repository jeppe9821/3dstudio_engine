#version 430 core

in vec3 fs_color;
in vec2 fs_txCoords;

out vec4 color;

uniform sampler2D particleTexture;

void main()
{
	color = texture(particleTexture, fs_txCoords) * vec4(fs_color, 1.f);
}