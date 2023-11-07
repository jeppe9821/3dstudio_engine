#version 430 core

struct Vertex
{
  vec4 position;
  vec3 normal;
  vec2 texCoord;
};

in Vertex vertex;

layout(location = 0) out vec4 position;  // position of the vertex (and fragment) in world space
layout(location = 1) out vec3 normal;  // surface normal vector in world space
layout(location = 2) out vec2 texCoord;

// model, view and projection transform
uniform mat4 m, v, p;

// Inverse transpose of model matrix for transforming normals
uniform mat3 m_3x3_inv_transp;

uniform mat4 u_lightSpaceMatrix;
out vec4 _fragPosLightSpace;

void main()
{
  mat4 mv = v * m;
  texCoord = vertex.texCoord;

  _fragPosLightSpace = u_lightSpaceMatrix * m * vertex.position;

  position = mv * vertex.position;
  normal = normalize(m_3x3_inv_transp * vertex.normal);

  gl_Position = p * position;
}
