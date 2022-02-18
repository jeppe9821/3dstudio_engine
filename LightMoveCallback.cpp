#include "LightMoveCallback.h"
#include "Transform.h"
#include "Light.h"

#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

LightMoveCallback::LightMoveCallback(std::shared_ptr<Light> light, std::shared_ptr<Transform> transform) : m_light(light), m_transform(transform), m_move(true), m_tick(0), m_pressed(false)
{
}

void LightMoveCallback::update(Node &n)
{
    if(!m_move)
    {
        return;
    }

    m_tick += 0.01f; //FIXME: This will now vary depending on FPS. Very bad, but good enough for now. We could not use glfwGetTime() because the time keeps ticking even if the move is stopped

    float y = m_light->getPosition().y;
    float x = 1.0f * glm::sin(m_tick) * y;
    float z = 1.0f * glm::cos(m_tick) * y;
    float w = m_light->getPosition().w;

    m_light->setPosition(glm::vec4(x, y, z, w));

    m_transform->resetTransform();
    m_transform->translate(glm::vec3(m_light->getPosition()));
    m_transform->scale(glm::vec3(0.1f, 0.1f, 0.1f));
}

void LightMoveCallback::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !m_pressed)
	{
        m_move = !m_move;
        m_pressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE && m_pressed)
	{
        m_pressed = false;
	}
}
