/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>

#include "Material.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

#include <vr/glErrorUtil.h>
#include <sstream>


/// Simple class for storing material properties
Material::Material() : m_shininess(5)
{
	m_ambient = glm::vec4(0.1, 1.0, 0.2, 1.0);
	m_diffuse = glm::vec4(0.7, 0.8, 0.8, 1.0);
	m_specular = glm::vec4(1.0, 1.0, 1.0, 1.0);
}

Material::~Material()
{
}

void Material::merge(std::shared_ptr<Material> other)
{
	glm::vec4 otherAmbient = other->getAmbient();
	glm::vec4 otherDiffuse = other->getDiffuse();
	glm::vec4 otherSpecular = other->getSpecular();
	GLfloat otherShininess = other->getShininess();

	glm::vec4 myAmbient = getAmbient();
	glm::vec4 myDiffuse = getDiffuse();
	glm::vec4 mySpecular = getSpecular();
	GLfloat myShininess = getShininess();

	glm::vec4 ambientDiff = glm::abs(myAmbient - otherAmbient);
	glm::vec4 diffuseDiff = glm::abs(myDiffuse - otherDiffuse);
	glm::vec4 specularDiff = glm::abs(mySpecular - otherSpecular);
	GLfloat shininessDiff = std::fabs(myShininess - otherShininess);

	float epsilon = std::numeric_limits<float>::epsilon();

	if(shininessDiff > epsilon)
	{
		setShininess(otherShininess);
	}

	for(int i = 0; i < 4; i++)
	{
		if(ambientDiff[i] < epsilon)
		{
			setAmbient(otherAmbient);
		}
		if(diffuseDiff[i] < epsilon)
		{
			setDiffuse(otherDiffuse);
		}
		if(specularDiff[i] < epsilon)
		{
			setSpecular(otherSpecular);
		}
	}
}

void Material::apply(GLuint program)
{
	GLint loc = 0;
	int i = 0;

	loc = glGetUniformLocation(program, "material.ambient");
	glUniform4fv(loc, 1, glm::value_ptr(m_ambient));

	loc = glGetUniformLocation(program, "material.specular");
	glUniform4fv(loc, 1, glm::value_ptr(m_specular));

	loc = glGetUniformLocation(program, "material.diffuse");
	glUniform4fv(loc, 1, glm::value_ptr(m_diffuse));

	loc = glGetUniformLocation(program, "material.shininess");
	glUniform1f(loc, m_shininess);

	CHECK_GL_ERROR_LINE_FILE();
}

glm::vec4 Material::getAmbient() const
{
	return m_ambient;
}

glm::vec4 Material::getSpecular() const
{
	return m_specular;
}

glm::vec4 Material::getDiffuse() const
{
	 return m_diffuse;
}

GLfloat Material::getShininess() const
{
	return m_shininess;
}

void Material::setAmbient(const glm::vec4& color)
{
	m_ambient = color;
}

void Material::setSpecular(const glm::vec4& color)
{
	m_specular = color;
}

void Material::setDiffuse(const glm::vec4& color)
{
	m_diffuse = color;
}

void Material::setShininess(float s)
{
	m_shininess = s;
}