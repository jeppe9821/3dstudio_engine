#include "Group.h"
#include "NodeVisitor.h"
#include <iostream>

Group::Group(std::shared_ptr<State> state) : Node(state)
{
}

Group::Group() : Node()
{
}

Group::~Group()
{
}

void Group::addChild(std::shared_ptr<Node> n)
{
	m_children.push_back(n);
}

void Group::accept(NodeVisitor &v)
{
	if(isEnabled())
		acceptChildren(v);
}

void Group::acceptChildren(NodeVisitor &v)
{
	for (auto child : m_children)
	{
		child->accept(v);
	}
}

BoundingBox Group::calculateBoundingBox()
{
	BoundingBox box;

	for (auto child : m_children)
	{
		box.expand(child->calculateBoundingBox());
	}

	m_cachedBoundingBox = box;
	m_hasCachedBoundingBox = true;

	return box;
}

BoundingBox Group::getCachedBoundingBox()
{
	return m_cachedBoundingBox;
}

bool Group::hasCachedBoundingBox()
{
	return m_hasCachedBoundingBox;
}

std::shared_ptr<Node> Group::removeChildAt(int index)
{
	std::shared_ptr<Node> child = m_children[index];
	m_children.erase(m_children.begin() + index);
	return child;
}

void Group::setChildAt(int index, std::shared_ptr<Node> child)
{
	m_children[index] = child;
}

std::shared_ptr<Node> Group::getChild(int i)
{
	return m_children[i];
}

std::vector<std::shared_ptr<Node>> Group::getChildren()
{
	return m_children;
}