#include <memory>

#include "UpdateCallback.h"

class Node;
class Transform;
class Light;
class GLFWwindow;

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
        LightMoveCallback(std::shared_ptr<Light> light, std::shared_ptr<Transform> transform);

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

    private:
        std::shared_ptr<Light> m_light;
        std::shared_ptr<Transform> m_transform;
        bool m_move;
        float m_tick;
        bool m_pressed;
};