#pragma once
#include <GLFW/glfw3.h>

/// <summary>
/// A simple FPScounter class, used for rendering the FPS on screen
/// </summary>
class FPSCounter
{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		FPSCounter();

		/// <summary>
        /// Renders the fps
        /// </summary>
        /// <param name="window">The window</param>
		void render(GLFWwindow* window);

	private:
		/// <summary>
		/// Updates the FPS
		/// </summary>
		void update();

		double m_lastTime;
		unsigned int m_numFrames;
		double m_fps;
};