#pragma once

#include "NodeVisitor.h"

#include <memory>
#include <vector>
#include <sstream>
#include "Light.h"
#include "Camera.h"
#include <stack>

class Group;
class Transform;
class Geometry;


/// <summary>
/// The RenderVisitor, this will visit all the nodes and render them
/// </summary>
class RenderVisitor : public NodeVisitor
{
    public:

        /// <summary>
        /// The constructor
        /// </summary>
        RenderVisitor();

        /// <summary>
        /// Resets the state
        /// </summary>
        /// <param name="program">program</param>
        void resetState(GLuint program);

        /// <summary>
        /// Visits the group node
        /// </summary>
        /// <param name="g">group node</param>
        virtual void visit(Group& g) override;

        /// <summary>
        /// Visits the Transform node
        /// </summary>
        /// <param name="g">Transform node</param>
        virtual void visit(Transform& g) override;

        /// <summary>
        /// Visits the Geometry node
        /// </summary>
        /// <param name="g">Geometry node</param>
        virtual void visit(Geometry &g) override;

	private:
		std::stack<glm::mat4> m_transformationStack;
        std::stack<std::shared_ptr<State>> m_stateStack;
};