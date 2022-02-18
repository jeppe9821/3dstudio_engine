#include "SortedGroup.h"
#include "Camera.h"
#include "Transform.h"
#include <iostream>
#include <algorithm>

SortedGroup::SortedGroup(std::shared_ptr<State> state) : Transform(state)
{
}

SortedGroup::SortedGroup() : Transform()
{
}

SortedGroup::SortedGroup(std::shared_ptr<Camera> camera) : m_camera(camera)
{
}

SortedGroup::~SortedGroup()
{
}

void SortedGroup::addChild(std::shared_ptr<Node> child)
{
    Group::addChild(child);

    BoundingBox b = calculateBoundingBox();
    m_center = b.getCenter();
}

void SortedGroup::update(Node &n)
{
    //FIXME: This sorting algorithm isn't.... Optimal. It only checks the direction of the camera and will only work in cases where the objects are nicely
    //sorted in a specific way. Ideally you'd want to check the distance between the object and the camera and sort from the object furthest away to the object closest to the camera.
    //But for some reason it didn't want to work and had really weird artifacts. There is however a better way to calculate the distance which I have yet to understand
    //which is taking the dot product of the camera's Z-axis. This might be need to be revisisted in the future

    std::vector<std::shared_ptr<Node>> children = getChildren();

    if(m_camera->getDirection().x < 0 && m_rightOrder)
    {
        int start = children.size()-1;
        int end = 0;

        while (end < start)
        {
            auto temp = getChild(end);
            setChildAt(end, getChild(start));
            setChildAt(start, temp);
            end++;
            start--;
        }

        m_rightOrder = false;
    }
    else if(m_camera->getDirection().x > 0 && !m_rightOrder)
    {
        int start = 0;
        int end = children.size()-1;

        while (start < end)
        {
            auto temp = getChild(start);
            setChildAt(start, getChild(end));
            setChildAt(end, temp);
            start++;
            end--;
        }

        m_rightOrder = true;
    }
}