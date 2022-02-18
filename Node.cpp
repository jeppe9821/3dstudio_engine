#include "Node.h"

Node::Node(std::shared_ptr<State> state) : m_state(state), m_enabled(true)
{
	m_name = "DefaultNodeWithState_Name";
	m_state = state;
}

Node::Node() : m_enabled(true)
{
	m_name = "DefaultNode_Name";
	m_state = nullptr;
}

Node::~Node()
{
}

void Node::addUpdateCallback(std::shared_ptr<UpdateCallback> callback)
{
	m_updateCallbacks.push_back(callback);
}

void Node::invokeUpdateCallbacks()
{
	for(auto callback : m_updateCallbacks)
	{
		callback->update(*this);
	}
}

void Node::setName(std::string name)
{
	m_name = name;
}

void Node::setState(std::shared_ptr<State> state)
{
	m_state = state;
}

void Node::setEnabled(bool flag)
{
	m_enabled = flag;
}

std::string Node::getName()
{
	return m_name;
}

std::shared_ptr<State> Node::getState()
{
	return m_state;
}

bool Node::hasState()
{
	return m_state != nullptr;
}

bool Node::isEnabled()
{
	return m_enabled;
}