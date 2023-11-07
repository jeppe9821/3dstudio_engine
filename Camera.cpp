#include <GL/glew.h>
#include "Camera.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtx/io.hpp>

Camera::Camera() :
	m_uniform_v(0),
	m_uniform_p(0),
	m_uniform_v_inv(0),
	m_firstClick(true),
	m_speed(0.1f),
	m_sceneScale(1),
	m_sensitivity(100.0f),
	m_fov(50)
{
	m_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_position = glm::vec3(0.0f, -1.0f, 0.0f);

	m_initialPosition = m_position;
	m_initialDirection = m_direction;

  	m_screenSize[0] = 1280;
  	m_screenSize[1] = 720;

	m_nearFar = glm::vec2(0.1, 100);
}

bool Camera::init(GLuint program)
{
	const char* uniform_name;
	uniform_name = "v";
	m_uniform_v = glGetUniformLocation(program, uniform_name);

	if (m_uniform_v == -1)
	{
		//std::cout << "Could not bind uniform " << uniform_name << std::endl;
		//return false;
	}

	uniform_name = "p";
	m_uniform_p = glGetUniformLocation(program, uniform_name);

	if (m_uniform_p == -1)
	{
		//std::cout << "Could not bind uniform " << uniform_name << std::endl;
		//return false;
	}

	uniform_name = "v_inv";
	m_uniform_v_inv = glGetUniformLocation(program, uniform_name);

	if (m_uniform_v_inv == -1)
	{
		//std::cout << "Could not bind uniform " << uniform_name << std::endl;
		//return false;
	}

	return true;
}

void Camera::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_position += m_speed * m_sceneScale * m_direction;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_position += m_speed * m_sceneScale * -glm::normalize(glm::cross(m_direction, m_up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_position += m_speed * m_sceneScale * -m_direction;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_position += m_speed * m_sceneScale * glm::normalize(glm::cross(m_direction, m_up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		m_position = m_initialPosition;
		m_direction = m_initialDirection;
		//m_position += m_speed * m_sceneScale * m_up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_position += m_speed * m_sceneScale * -m_up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		m_speed = 0.1f;
	}


	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (m_firstClick)
		{
			glfwSetCursorPos(window, (m_screenSize[0] / 2), (m_screenSize[1] / 2));
			m_firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees
		float rotX = m_sensitivity * (float)(mouseY - (m_screenSize[1] / 2)) / m_screenSize[1];
		float rotY = m_sensitivity * (float)(mouseX - (m_screenSize[0] / 2)) / m_screenSize[0];

		// Calculates upcoming vertical change in the direction
		glm::vec3 newdirection = glm::rotate(m_direction, glm::radians(-rotX), glm::normalize(glm::cross(m_direction, m_up)));

		// Decides whether or not the next vertical direction is legal or not
		if (abs(glm::angle(newdirection, m_up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			m_direction = newdirection;
		}

		// Rotates the direction left and right
		m_direction = glm::rotate(m_direction, glm::radians(-rotY), m_up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (m_screenSize[0] / 2), (m_screenSize[1] / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		m_firstClick = true;
	}
}

void Camera::set(glm::vec3 eye, glm::vec3 direction, glm::vec3 up)
{
	m_position = eye;
	m_direction = direction;
	m_up = up;

	m_initialPosition = m_position;
	m_initialDirection = m_direction;
}

void Camera::setSceneScale(float scale)
{
	m_sceneScale = scale;
}

void Camera::setTransform(glm::mat4 transform)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat direction;
	glm::decompose(transform, scale, direction, translation, skew, perspective);

	m_position = translation;
	m_up = glm::vec3(0, 1, 0) * direction;
	m_direction = glm::vec3(0.0f, 0.0f, -1.0f) * direction;

	m_transform = transform;
}

void Camera::setNearFar(const glm::vec2& nearFar)
{
	m_nearFar = nearFar;
}

void Camera::setFov(float fov)
{
	m_fov = fov;
}

void Camera::setPosition(glm::vec3 pos)
{
	m_position = pos;
}

void Camera::setScreenSize(const glm::uvec2& size)
{
	m_screenSize = size;
}

glm::uvec2 Camera::getScreenSize()
{
	return m_screenSize;
}

glm::vec3 Camera::getPosition() const
{
	return m_position;
}

glm::vec3 Camera::getDirection()
{
	return m_direction;
}

glm::vec3 Camera::getUp()
{
	return m_up;
}

float Camera::getFov()
{
	return m_fov;
}

glm::vec2 Camera::getNearFar()
{
	return m_nearFar;
}

GLint Camera::getViewUniform()
{
	return m_uniform_v;
}

GLint Camera::getProjectionUniform()
{
	return m_uniform_p;
}

GLint Camera::getViewInverseUniform()
{
	return m_uniform_v_inv;
}

glm::mat4 Camera::getTransform()
{
	return m_transform;
}

void Camera::setLightSpaceMatrix(glm::mat4 matrix)
{
    m_lightSpaceMatrix = matrix;
}

glm::mat4 Camera::getLightSpaceMatrix()
{
    return m_lightSpaceMatrix;
}

glm::mat4 Camera::getView()
{
	return m_view;
}

glm::mat4 Camera::getProjection()
{
	return m_projection;
}

void Camera::setView(glm::mat4 view)
{
	m_view = view;
}

void Camera::setProjection(glm::mat4 proj)
{
	m_projection = proj;
}