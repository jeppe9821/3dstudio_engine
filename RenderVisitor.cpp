#include "RenderVisitor.h"
#include "Group.h"
#include "Transform.h"
#include "Geometry.h"
#include <iostream>
#include <vr/shaderUtils.h>

RenderVisitor::RenderVisitor()
{
}

int noMore = 0;

void RenderVisitor::resetState()
{
	m_stateStack = {};
	m_stateStack.push(std::shared_ptr<State>(new State()));
	noMore++;
}

void RenderVisitor::visit(Group& g)
{
	bool pop = false;

	if(g.hasState())
	{
		pop = true;
		std::shared_ptr<State> s(new State());
		s->merge(m_stateStack.top());
		s->merge(g.getState());
		m_stateStack.push(s);
	}

	g.accept(*this);

	if(pop)
	{
		m_stateStack.pop();
	}
}

void RenderVisitor::visit(Transform& g)
{
	if(m_transformationStack.empty())
	{
		m_transformationStack.push(g.getModelMatrix());
	}
	else
	{
		m_transformationStack.push(m_transformationStack.top() * g.getModelMatrix());
	}

	bool pop = false;

	if(g.hasState())
	{
		pop = true;
		std::shared_ptr<State> s(new State());
		s->merge(m_stateStack.top());
		s->merge(g.getState());
		m_stateStack.push(s);
	}

	g.acceptChildren(*this);

	if(pop)
	{
		m_stateStack.pop();
	}

	m_transformationStack.pop();
}

void RenderVisitor::visit(Geometry &g)
{
	bool pop = false;

	if(g.hasState())
	{
		std::shared_ptr<State> s(new State());
		s->merge(m_stateStack.top());
		s->merge(g.getState());
		m_stateStack.push(s);
		pop = true;
	}

	g.initShaders(m_stateStack.top()->getProgram());

	m_stateStack.top()->apply();

	if(!m_transformationStack.empty())
	{
		glm::mat4 transform = m_transformationStack.top();

		g.apply(transform);
	}

	g.render();

	if(pop)
	{
		m_stateStack.pop();
	}
}