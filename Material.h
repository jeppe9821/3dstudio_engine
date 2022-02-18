#pragma once

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
/* GLM */
// #define GLM_MESSAGES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Texture.h"

/// <summary>
/// The Material class
/// </summary>
class Material
{
	public:
		/// <summary>
		/// The cosntructor
		/// </summary>
		Material();

		/// <summary>
		/// The destructor
		/// </summary>
		~Material();

        /// <summary>
        /// Merges the material with another material
        /// </summary>
        /// <param name="other">other material</param>
		void merge(std::shared_ptr<Material> other);

        /// <summary>
        /// Applies a program for the material
        /// </summary>
        /// <param name="program">the program</param>
		void apply(GLuint program);

        /// <summary>
        /// Retruns the ambient
        /// </summary>
        /// <returns>The ambient</returns>
		glm::vec4 getAmbient() const;

        /// <summary>
        /// Retruns the specular
        /// </summary>
        /// <returns>The specular</returns>
		glm::vec4 getSpecular() const;

        /// <summary>
        /// Retruns the diffuse
        /// </summary>
        /// <returns>The diffuse</returns>
		glm::vec4 getDiffuse() const;

        /// <summary>
        /// Retruns the shininess
        /// </summary>
        /// <returns>The shininess</returns>
		GLfloat getShininess() const;

        /// <summary>
        /// Sets the ambient
        /// </summary>
        /// <param name="color">the color</param>
		void setAmbient(const glm::vec4& color);

        /// <summary>
        /// Sets the specular
        /// </summary>
        /// <param name="color">the color</param>
		void setSpecular(const glm::vec4& color);

        /// <summary>
        /// Sets the diffuse
        /// </summary>
        /// <param name="color">the color</param>
		void setDiffuse(const glm::vec4& color);

        /// <summary>
        /// Sets the shininess
        /// </summary>
        /// <param name="s">the shininess</param>
		void setShininess(float s);

	private:
		glm::vec4 m_ambient;
		glm::vec4 m_diffuse;
		glm::vec4 m_specular;

		GLfloat m_shininess;
};

