#version 430 core

in VS_OUT {
    vec3 texCoords;
} vs_in;

out vec4 fcolor;

uniform samplerCube u_Skybox;

const float levels = 5.0;

void main()
{
    vec4 texColor = texture(u_Skybox, vs_in.texCoords);
    fcolor = texColor;
}