#include "Light.h"
#include <iostream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Light::Light() : m_enabled(true)
{
}

void Light::apply(GLuint program, size_t idx)
{
	int i = 0;

	// Update light position
	//m_mesh->object2world = glm::translate(glm::mat4(1), glm::vec3(this->position));

	std::stringstream str;
	str << "lights[" << idx << "].";
	std::string prefix = str.str();

	GLint loc = -1;
	std::string uniform_name;

	uniform_name = prefix + "enabled";
	loc = glGetUniformLocation(program, uniform_name.c_str());

	if (loc == -1)
	{
		//std::cerr << "Could not bind uniform << " << uniform_name << std::endl;
		//return;
	}

	glUniform1i(loc, m_enabled);

	#define APPLY_UNIFORM4V(PROGRAM, NAME, VALUE) \
	uniform_name = prefix + std::string(NAME);  \
	loc = glGetUniformLocation(program, uniform_name.c_str()); \
	if (loc == -1) { \
		/*std::cerr << "Could not bind uniform << " << uniform_name << std::endl;*/ \
		return; \
			} \
	glUniform4fv(loc, 1, glm::value_ptr(VALUE));

	APPLY_UNIFORM4V(program, "diffuse", this->m_diffuse);
	APPLY_UNIFORM4V(program, "specular", this->m_specular);
	APPLY_UNIFORM4V(program, "position", this->m_position);
}

void Light::setEnabled(bool flag)
{
	this->m_enabled = flag;
}

void Light::setPosition(glm::vec4 position)
{
	this->m_position = position;
}

void Light::setDiffuse(glm::vec4 diffuse)
{
	this->m_diffuse = diffuse;
}

void Light::setSpecular(glm::vec4 specular)
{
	this->m_specular = specular;
}

bool Light::isEnabled()
{
	return m_enabled;
}

glm::vec4 Light::getPosition()
{
	return m_position;
}

glm::vec4 Light::getDiffuse()
{
	return m_diffuse;
}

glm::vec4 Light::getSpecular()
{
	return m_specular;
}