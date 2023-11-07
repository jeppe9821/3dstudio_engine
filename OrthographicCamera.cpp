#include <GL/glew.h>
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtx/io.hpp>

#include <iostream>

OrthographicCamera::OrthographicCamera() :
    Camera(),
    m_top(25.0f),
    m_oblscale(0.0f),
    m_oblrad(3.14567f/4.0f)
{
}

void OrthographicCamera::apply(GLuint program)
{
    glm::vec3 position = getPosition();
    glm::uvec2 screenSize = getScreenSize();
    glm::vec3 direction = getDirection();
    glm::vec3 up = getUp();
    glm::vec2 nearFar = getNearFar();

	GLint uniform_v = getViewUniform();
	GLint uniform_p = getProjectionUniform();
    GLint uniform_v_inv = getViewInverseUniform();

    float aspect = float(screenSize[0])/float(screenSize[1]);
    float bottom = -m_top;
    float right = m_top * aspect;
    float left = -right;

	glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);

    projection = glm::ortho(left, right, bottom, m_top, nearFar[0], nearFar[1]);
	view = glm::lookAt(position, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.00001f, 1.0f, 0.00001f));

    glm::mat4 H = glm::mat4(1.0f);

    H[2][0] = m_oblscale * cos(m_oblrad);
    H[2][1] = m_oblscale * sin(m_oblrad);

    projection *= H;

    glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(projection));

	glm::mat4 v_inv = glm::inverse(view);
	glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

    setLightSpaceMatrix(projection * view);
}

void OrthographicCamera::setTop(float top)
{
    m_top = top;
}