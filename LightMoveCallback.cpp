#include "LightMoveCallback.h"
#include "Transform.h"
#include "Light.h"
#include "Camera.h"

#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

LightMoveCallback::LightMoveCallback(std::shared_ptr<Light> light, std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera) : m_light(light), m_skipAFrame(false), m_oldPosition(glm::vec4(0)), m_transform(transform), m_move(true), m_manualMove(false), m_tick(0), m_pressed(false), m_camera(camera)
{
}

void LightMoveCallback::update(Node &n)
{
    if(m_manualMove)
    {
        m_light->setPosition(glm::vec4(m_camera->getPosition(), m_light->getPosition().w));
    }

    if(!m_move)
    {
        return;
    }

    m_oldPosition = m_light->getPosition();

    m_tick += 0.01f; //FIXME: This will now vary depending on FPS. Very bad, but good enough for now. We could not use glfwGetTime() because the time keeps ticking even if the move is stopped

    float y = m_light->getPosition().y;
    float x = 1.0f * glm::sin(m_tick) * y;
    float z = 1.0f * glm::cos(m_tick) * y;
    float w = m_light->getPosition().w;

    m_light->setPosition(glm::vec4(x, y, z, w));

    m_transform->resetTransform();
    m_transform->translate(glm::vec3(m_light->getPosition()));
    m_transform->scale(glm::vec3(0.01f, 0.01f, 0.01f));
}

void LightMoveCallback::processInput(GLFWwindow *window)
{
    if(m_skipAFrame)
    {
        if(glm::abs(glfwGetTime() - m_frameAt) > 0.5f)
        {
            std::cout << "do not skip frame" << std::endl;
            m_skipAFrame = false;
        }
        return;
    }

    if(glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && !m_pressed)
    {
        m_move = false;
        m_manualMove = !m_manualMove;
        m_pressed = true;
        m_skipAFrame = true;
        m_frameAt = glfwGetTime();
    }

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !m_pressed)
	{
        m_move = !m_move;
        m_pressed = true;
        m_manualMove = false;
        m_light->setPosition(m_oldPosition);
        m_skipAFrame = true;
        m_frameAt = glfwGetTime();
	}

    if(glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE && m_pressed)
    {
        m_pressed = false;
    }

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE && m_pressed)
	{
        m_pressed = false;
	}
}

bool LightMoveCallback::isAutomaticallyMoving()
{
    return m_move;
}

bool LightMoveCallback::isManuallyMoving()
{
    return m_manualMove;
}
