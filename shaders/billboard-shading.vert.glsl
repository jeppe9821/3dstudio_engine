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

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

uniform mat3 m_3x3_inv_transp;

void main()
{
    position = v * m * vertex.position;
    normal = normalize(m_3x3_inv_transp * vertex.normal);
    texCoord = vertex.texCoord;

    int spherical = 0; //this will define if the billboard should follow on the Y axis or not

    mat4 modelView = v * m;

    modelView[0][0] = 1.0 * m[0][0];
    modelView[0][1] = 0.0;
    modelView[0][2] = 0.0;

    if(spherical == 1)
    {
        modelView[1][0] = 0.0;
        modelView[1][1] = 1.0 * m[1][1];
        modelView[1][2] = 0.0;
    }

    modelView[2][0] = 0.0;
    modelView[2][1] = 0.0;
    modelView[2][2] = 1.0 * m[2][2];

    vec4 position = modelView * vertex.position;
    gl_Position = p * position;
}