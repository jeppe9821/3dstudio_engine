#version 430 core

struct Vertex
{
  vec4 position;
  vec3 normal;
  vec2 texCoord;
};

in Vertex vertex;

// model, view and projection transform
uniform mat4 m;
uniform mat4 u_lightSpaceMatrix;

void main()
{
  gl_Position = u_lightSpaceMatrix * m * vertex.position;
}
