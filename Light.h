#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Geometry.h"
#include <vector>

/// <summary>
/// The Light class
/// </summary>
class Light
{
	public:
		/// <summary>
		/// A simple constructor
		/// </summary>
		Light();

        /// <summary>
		/// Applies the uniforms of the light
		/// </summary>
        /// <param name="program">The shader program</param>
		/// <param name="idx">The id</param>
		void apply(GLuint program, size_t idx);

        /// <summary>
		/// Sets if the light should be enabled or not
		/// </summary>
        /// <param name="flag">The flag</param>
		void setEnabled(bool flag);

        /// <summary>
		/// Sets the position of the light
		/// </summary>
        /// <param name="position">The position</param>
		void setPosition(glm::vec4 position);

        /// <summary>
		/// Sets the diffuse of the light
		/// </summary>
        /// <param name="diffuse">The diffuse color</param>
		void setDiffuse(glm::vec4 diffuse);

        /// <summary>
		/// Sets the specular of the light
		/// </summary>
        /// <param name="specular">The specular color</param>
		void setSpecular(glm::vec4 specular);

        /// <summary>
		/// Checks if the light is enabled or not
		/// </summary>
        /// <returns>The isEnabled flag</returns>
		bool isEnabled();

        /// <summary>
		/// Gets the position
		/// </summary>
        /// <returns>The position</returns>
		glm::vec4 getPosition();

        /// <summary>
		/// Gets the diffuse
		/// </summary>
        /// <returns>The diffuse</returns>
		glm::vec4 getDiffuse();

        /// <summary>
		/// Gets the specular
		/// </summary>
        /// <returns>The specular</returns>
		glm::vec4 getSpecular();

	private:
		bool m_enabled;
		glm::vec4 m_position;
		glm::vec4 m_diffuse;
		glm::vec4 m_specular;
};

