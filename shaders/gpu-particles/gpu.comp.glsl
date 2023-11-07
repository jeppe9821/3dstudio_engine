#version 430 core

layout(local_size_x = 64) in;

layout(std430, binding = 0) buffer Pos
{
	vec4 position[];
};

layout(std430, binding = 1) buffer Vel
{
	vec4 velocity[];
};

uniform vec3 followPosition;
uniform float deltaTime;
uniform bool isActive;

void main()
{
	if(isActive)
	{
		velocity[gl_GlobalInvocationID.x] += 1000 * vec4(normalize(followPosition - position[gl_GlobalInvocationID.x].xyz), 0.0) * min(0.05, deltaTime);
	}
	else
	{
		velocity[gl_GlobalInvocationID.x] = vec4(0);
	}

	position[gl_GlobalInvocationID.x] += velocity[gl_GlobalInvocationID.x] * deltaTime;
}