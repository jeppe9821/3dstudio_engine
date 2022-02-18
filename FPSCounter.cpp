#include "FPSCounter.h"
#include <vr/DrawText.h>
#include <sstream>

FPSCounter::FPSCounter() : m_lastTime(0), m_numFrames(0), m_fps(0)
{
}

void FPSCounter::render(GLFWwindow* window)
{
	update();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	std::ostringstream str;
	str << "FPS: " << m_fps << std::ends;

	vr::Text::setColor(glm::vec4(1, 1, 0, 0.8));
	vr::Text::setFontSize(20);
	vr::Text::drawText(width, height, 10, 20, str.str().c_str());
}

void FPSCounter::update()
{
	m_numFrames++;
	double now = glfwGetTime();
	double delta_t = now - m_lastTime;
	if (delta_t > 50e-3)
	{
		m_fps = m_numFrames / delta_t;

		m_numFrames = 0;
		m_lastTime = now;
	}
}
