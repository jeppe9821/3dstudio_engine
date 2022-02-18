#include "UpdateVisitor.h"
#include "Group.h"
#include "Transform.h"
#include "Geometry.h"
#include <iostream>

void UpdateVisitor::visit(Group& g)
{
	g.invokeUpdateCallbacks();
	g.accept(*this);
}

void UpdateVisitor::visit(Transform& t)
{
	t.invokeUpdateCallbacks();
	t.acceptChildren(*this);
}

void UpdateVisitor::visit(Geometry& g)
{
	g.invokeUpdateCallbacks();
}