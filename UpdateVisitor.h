#include "NodeVisitor.h"

class Node;

class UpdateVisitor : public NodeVisitor
{
	public:
		virtual void visit(Group &) override;
        virtual void visit(Transform &) override;
        virtual void visit(Geometry &) override;
		void invokeUpdateCallbacks(Node &n);
};