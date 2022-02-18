#include "LOD.h"
#include "Camera.h"
#include <iostream>
#include <glm/glm.hpp>

LOD::LOD() : Transform()
{
}

LOD::LOD(std::shared_ptr<State> state) : Transform(state)
{
}

LOD::LOD(std::shared_ptr<Camera> camera) : m_camera(camera), m_radius(0), m_center(glm::vec3(0)), m_distances(), m_levels(0)
{
}

LOD::~LOD()
{
}

void LOD::addChild(std::shared_ptr<Node> child)
{
    Group::addChild(child);

    BoundingBox b = calculateBoundingBox();
    m_radius = b.getRadius();
    m_center = b.getCenter();
    m_levels = m_levels+1;
    m_distances.push_back(m_radius + (m_levels * 300));
}

void LOD::update(Node &n)
{
    if(getChildren().size() < m_levels)
    {
        return;
    }

    glm::vec3 position = m_center;
    float distance = glm::distance(m_camera->getPosition(), position);

    for(int i = m_levels-1; i >= 0; i--)
    {
        if(distance > m_distances[i])
        {
            for(int j = 0; j < m_levels; j++)
            {
                getChild(j)->setEnabled(false);
            }

            getChild(i)->setEnabled(true);
            break;
        }
    }
}