#pragma once

class Group;
class Transform;
class Geometry;

/// <summary>
/// The nodevisitor base class, will be visiting all the nodes in the scenegraph
/// </summary>
class NodeVisitor
{
    public:
		/// <summary>
		/// Visits the group
		/// </summary>
		/// <param name="g">The group</param>
        virtual void visit(Group &g) = 0;

		/// <summary>
		/// Visits the transform
		/// </summary>
		/// <param name="t">The transform</param>
        virtual void visit(Transform &t) = 0;

		/// <summary>
		/// Visits the geometry
		/// </summary>
		/// <param name="g">The geometry</param>
        virtual void visit(Geometry &g) = 0;
};