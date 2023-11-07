#version 430 core

struct Vertex
{
  vec4 position;
  vec3 normal;
  vec2 texCoord;
};

in Vertex vertex;

out VS_OUT {
    vec3 texCoords;
} vs_out;

uniform mat4 p, v;
void main()
{
    vs_out.texCoords = vec3(vertex.position);
    vec4 pos = p * v * vertex.position;
    gl_Position = pos.xyww;
}