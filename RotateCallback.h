#pragma once

#include "UpdateCallback.h"
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

/// <summary>
/// The RotateCallback class, this will rotate an node every frame
/// </summary>
class RotateCallback : public UpdateCallback
{
	public:
		/// <summary>
		/// The RotateCallback constructor
		/// </summary>
		RotateCallback();

        /// <summary>
        /// Updates
        /// </summary>
        /// <param name="n">the node</param>
		virtual void update(Node &n) override;
};