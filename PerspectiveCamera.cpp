#include <GL/glew.h>
#include "PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtx/io.hpp>

PerspectiveCamera::PerspectiveCamera() : Camera()
{

}

void PerspectiveCamera::apply(GLuint program)
{
    glm::vec3 position = getPosition();
    glm::vec3 direction = getDirection();
    glm::vec3 up = getUp();
    glm::uvec2 screenSize = getScreenSize();
    float fov = getFov();
    glm::vec2 nearFar = getNearFar();
	GLint uniform_v = getViewUniform();
	GLint uniform_p = getProjectionUniform();
	GLint uniform_v_inv = getViewInverseUniform();

	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(position, position + direction, up);

	float aspect = (float)screenSize[0] / (float)screenSize[1];

	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(fov), aspect, nearFar[0], nearFar[1]);

	glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(projection));
	glm::mat4 v_inv = glm::inverse(view);
	glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

	setLightSpaceMatrix(projection * view);
	setProjection(projection);
	setView(view);
}