#include "DrawCameraStatus.h"
#include <vr/DrawText.h>
#include <sstream>
#include "LightMoveCallback.h"

DrawCameraStatus::DrawCameraStatus()
{
}

void DrawCameraStatus::render(GLFWwindow* window, std::shared_ptr<LightMoveCallback> callback)
{
    std::string cameraStatus;

    if(callback->isAutomaticallyMoving() && !callback->isManuallyMoving())
    {
        cameraStatus = "auto";
    }

    if(!callback->isAutomaticallyMoving() && callback->isManuallyMoving())
    {
        cameraStatus = "manual";
    }

    if(!callback->isAutomaticallyMoving() && !callback->isManuallyMoving())
    {
        cameraStatus = "none";
    }

	std::ostringstream str;
	str << "Light Movement Status: " << cameraStatus << std::ends;

	vr::Text::setColor(glm::vec4(1, 1, 0, 0.8));
	vr::Text::setFontSize(20);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	vr::Text::drawText(width, height, 10, 64, str.str().c_str());
    vr::Text::drawText(width, height, 10, 200, "TUTORIAL: Press 9 to toggle shadowmap with a delay on half a second");
    vr::Text::drawText(width, height, 10, 220, "TUTORIAL: Press 8 for manual light, 7 for automatic.");
    vr::Text::drawText(width, height, 10, 240, "You move around the manual light by moving around the camera");
    vr::Text::drawText(width, height, 10, 290, "Press 6 to activate particle animation");
}