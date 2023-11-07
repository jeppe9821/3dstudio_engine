#include "DepthCameraSetPositionCallback.h"
#include "Light.h"
#include "OrthographicCamera.h"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

DepthCameraSetPositionCallback::DepthCameraSetPositionCallback(std::shared_ptr<Light> light, std::shared_ptr<OrthographicCamera> orthoCamera) : m_light(light), m_orthoCamera(orthoCamera)
{
}

void DepthCameraSetPositionCallback::update(Node &n)
{
	m_orthoCamera->setTransform(glm::lookAt(glm::vec3(m_light->getPosition()), glm::vec3(0.0f), glm::vec3(0.00001f, 1.0f, 0.00001f)));
	m_orthoCamera->setPosition(m_light->getPosition());
}