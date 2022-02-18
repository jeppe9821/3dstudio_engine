#pragma once

#include <memory>

#include "Material.h"
#include "BoundingBox.h"
#include <glm/glm.hpp>
#include "State.h"
#include "UpdateCallback.h"

class NodeVisitor;

/// <summary>
/// The Node base class
/// </summary>
class Node
{
	public:
		/// <summary>
		/// Calculates the boundingbox
		/// </summary>
		/// <returns>The bounding box</returns>
		virtual BoundingBox calculateBoundingBox() = 0;

		/// <summary>
		/// Accepts the node
		/// </summary>
		/// <param name="v">The visitor</param>
		virtual void accept(NodeVisitor &v) = 0;

		/// <summary>
		/// The state constructor
		/// </summary>
		/// <param name="state">The state</param>
		Node(std::shared_ptr<State> state);

		/// <summary>
		/// The constructor
		/// </summary>
		Node();

		/// <summary>
		/// The destructor
		/// </summary>
		virtual ~Node();

		/// <summary>
		/// Adds an update callback
		/// </summary>
		/// <param name="callback">The callback</param>
		void addUpdateCallback(std::shared_ptr<UpdateCallback> callback);

		/// <summary>
		/// Invokes the update callbacks
		/// </summary>
		void invokeUpdateCallbacks();

		/// <summary>
		/// Sets the name
		/// </summary>
		/// <param name="name">The name</param>
		void setName(std::string name);

		/// <summary>
		/// Sets the state
		/// </summary>
		/// <param name="state">The state</param>
		void setState(std::shared_ptr<State> state);

		/// <summary>
		/// Sets  if the node should be enabled or not in the scenegraph
		/// </summary>
		/// <param name="flag">The flag</param>
		void setEnabled(bool flag);

		/// <summary>
		/// Gets the name
		/// </summary>
		/// <returns>The name</returns>
		std::string getName();

		/// <summary>
		/// Gets the state
		/// </summary>
		/// <returns>The name</returns>
		std::shared_ptr<State> getState();

		/// <summary>
		/// Checks if the node has a state
		/// </summary>
		/// <returns>The flag</returns>
		bool hasState();

		/// <summary>
		/// Checks if the node is enabled
		/// </summary>
		/// <returns>The flag</returns>
		bool isEnabled();

	private:
		std::string m_name;
		std::shared_ptr<State> m_state;
		std::vector<std::shared_ptr<UpdateCallback>> m_updateCallbacks;
		bool m_enabled;
};
