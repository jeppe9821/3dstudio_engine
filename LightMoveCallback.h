#include <memory>

#include "UpdateCallback.h"
#include <glm/glm.hpp>

class Node;
class Transform;
class Light;
class GLFWwindow;
class Camera;

/// <summary>
/// A callback class for moving around the light
/// </summary>
class LightMoveCallback : public UpdateCallback
{
    public:
        /// <summary>
        /// The constructor
        /// </summary>
        /// <param name="light">The light</param>
        /// <param name="transform">The transform</param>
        LightMoveCallback(std::shared_ptr<Light> light, std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera);

        /// <summary>
        /// The update function, given by UpdateCallback
        /// </summary>
        /// <param name="n">The node</param>
        void update(Node &n) override;

        /// <summary>
        /// Processes the input
        /// </summary>
        /// <param name="window">The window</param>
        void processInput(GLFWwindow *window);

        bool isAutomaticallyMoving();
        bool isManuallyMoving();

    private:
        std::shared_ptr<Light> m_light;
        std::shared_ptr<Transform> m_transform;
        std::shared_ptr<Camera> m_camera;
        bool m_move;
        float m_tick;
        bool m_pressed;
        bool m_manualMove;
        bool m_skipAFrame;
        glm::vec4 m_oldPosition;
        float m_frameAt;
};