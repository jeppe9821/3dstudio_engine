#pragma once

#include "Node.h"

/// <summary>
/// The group class, contains children for a given group in the scenegraph
/// </summary>
class Group : public Node
{
    public:
		/// <summary>
		/// The constructor for states, given by Node
		/// </summary>
		Group(std::shared_ptr<State> state);

		/// <summary>
		/// A basic constructor given by Node
		/// </summary>
		Group();

		/// <summary>
		/// The destructor given by Node
		/// </summary>
        virtual ~Group() override;

        /// <summary>
		/// Adds a child to the group
		/// </summary>
        /// <param name="n">The child</param>
        virtual void addChild(std::shared_ptr<Node> n);

        /// <summary>
		/// Accepts the node for the visitor, given by Node
		/// </summary>
        /// <param name="v">The visitor</param>
        virtual void accept(NodeVisitor& v) override;

        /// <summary>
		/// Accepts the children for the node visitor
		/// </summary>
        /// <param name="v">The visitor</param>
		virtual void acceptChildren(NodeVisitor &v);

        /// <summary>
		/// Calculates the bounding box, given by Node
		/// </summary>
        /// <returns>A bounding box</returns>
        virtual BoundingBox calculateBoundingBox() override;

        /// <summary>
		/// Removes child at index
		/// </summary>
        /// <param name="index">the index</param>
        /// <returns>The removed child</returns>
        std::shared_ptr<Node> removeChildAt(int index);

        /// <summary>
		/// Sets a child at index
		/// </summary>
        /// <param name="index">Sets a child at index</param>
        /// <param name="child">The child</param>
        void setChildAt(int index, std::shared_ptr<Node> child);

        /// <summary>
		/// Returns the child at index
		/// </summary>
        /// <param name="index">The index to get the child</param>
        /// <returns>The child</returns>
        std::shared_ptr<Node> getChild(int i);

        /// <summary>
		/// Returns all the children
		/// </summary>
        /// <returns>A children vector</returns>
		std::vector<std::shared_ptr<Node>> getChildren();

    private:
        std::vector<std::shared_ptr<Node>> m_children;
};