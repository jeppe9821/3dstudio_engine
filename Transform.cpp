#include "Transform.h"
#include "NodeVisitor.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Transform::Transform(std::shared_ptr<State> state) : Group(state)
{
}

Transform::Transform() : Group()
{
}

Transform::~Transform()
{
}

void Transform::translate(glm::vec3 translation)
{
    m_object2world = glm::translate(m_object2world, translation);
    m_translation += translation;
}

void Transform::rotate(float rad, glm::vec3 axis)
{
    m_object2world = glm::rotate(m_object2world, rad, axis);
}

void Transform::scale(glm::vec3 scaling)
{
    m_object2world = glm::scale(m_object2world, scaling);
}

void Transform::accept(NodeVisitor &v)
{
    if(isEnabled())
    {
        v.visit(*this);
    }
}

BoundingBox Transform::calculateBoundingBox()
{
	BoundingBox box;

	for (auto child : getChildren())
	{
        BoundingBox childBox = child->calculateBoundingBox();
        childBox = childBox * m_object2world;
        box.expand(childBox);
	}

	return box;
}

void Transform::setInitialTransform(const glm::mat4& m)
{
    m_object2world = m_initialTransform = m;
}

void Transform::resetTransform()
{
    m_object2world = m_initialTransform;
}

glm::mat4 Transform::getModelMatrix()
{
    return this->m_object2world;
}

glm::vec3 Transform::getTranslation()
{
    return this->m_translation;
}
