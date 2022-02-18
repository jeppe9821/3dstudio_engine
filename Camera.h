#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

/// <summary>
/// The Camera class
/// </summary>
class Camera
{
	public:
		/// <summary>
		/// The Camera Constructor
		/// </summary>
		Camera();

		/// <summary>
        /// Processes the input for the camera
        /// </summary>
        /// <param name="window">The window used for the input</param>
		void processInput(GLFWwindow* window);

		/// <summary>
        /// Returns the screen size
        /// </summary>
		/// <returns>The screen size</returns>
		glm::uvec2 getScreenSize();

		/// <summary>
        /// Sets the screen size for the camera
        /// </summary>
        /// <param name="size">The size</param>
		void setScreenSize(const glm::uvec2& size);

		/// <summary>
        /// Initilizes the camera
        /// </summary>
		/// <param name="program">The program</param>
		/// <returns>A flag showing if the camera initilized or not</returns>
		bool init(GLuint program);

		/// <summary>
        /// Initilizes the camera
        /// </summary>
		/// <param name="program">The program</param>
		void setNearFar(const glm::vec2& nearFar);

		/// <summary>
        /// Initilizes the camera
        /// </summary>
		/// <param name="program">The program</param>
		void setFov(float fov);

		/// <summary>
        /// Applies the camera uniforms
        /// </summary>
		/// <param name="program">The program</param>
		void apply(GLuint program);

		/// <summary>
        /// Applies the camera uniforms
        /// </summary>
		/// <param name="transform">The transform</param>
		void setTransform(glm::mat4 transform);

		/// <summary>
        /// Sets the variables for the camera
        /// </summary>
		/// <param name="eye">the camera position</param>
		/// <param name="direction">the camera direction</param>
		/// <param name="up">the up vector</param>
		void set(glm::vec3 eye, glm::vec3 direction, glm::vec3 up);

		/// <summary>
        /// Sets the position
        /// </summary>
		/// <param name="pos">the position</param>
		void setPosition(glm::vec3 pos);

		/// <summary>
        /// Returns the position
        /// </summary>
		/// <returns> The position </returns>
		glm::vec3 getPosition() const;

		/// <summary>
        /// Sets the scene scale
        /// </summary>
		/// <param name="scale">the scale of the scene</param>
		void setSceneScale(float scale);

		/// <summary>
        /// Returns the direction
        /// </summary>
		/// <returns> The direction </returns>
		glm::vec3 getDirection();

	private:
		GLint m_uniform_v;
		GLint m_uniform_p;
		GLint m_uniform_v_inv;
		glm::uvec2 m_screenSize;
		glm::vec2 m_nearFar;

		glm::vec3 m_initialPosition;
		glm::vec3 m_position;
		glm::vec3 m_initialDirection;
		glm::vec3 m_direction;
		glm::vec3 m_up;

		// Prevents the camera from jumping around when first clicking left click
		bool m_firstClick;

		// Adjust the speed of the camera and it's sensitivity when looking around
		float m_speed;
		float m_sceneScale;
		float m_sensitivity;
		float m_fov;
};

