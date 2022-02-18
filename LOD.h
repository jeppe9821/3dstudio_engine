#pragma once
#include "Transform.h"
#include "UpdateCallback.h"

class Camera;

/// <summary>
/// The LevelOfDetail class, handles the detail of objects within the subtree
/// </summary>
class LOD : public Transform, public UpdateCallback
{
    public:
        /// <summary>
        /// The constructor, given by Node
        /// </summary>
        LOD();

        /// <summary>
        /// The state constructor, given by Node
        /// </summary>
        /// <param name="state">The state</param>
        LOD(std::shared_ptr<State> state);

        /// <summary>
        /// The camera constructor
        /// </summary>
        /// <param name="camera">The camera</param>
        LOD(std::shared_ptr<Camera> camera);

        /// <summary>
        /// The destructor, given by Node
        /// </summary>
        virtual ~LOD() override;

        /// <summary>
        /// The update function for the LOD
        /// </summary>
        /// <param name="n">The node</param>
        void update(Node &n) override;

        /// <summary>
        /// Adds a child to the LOD group, given by Group
        /// </summary>
        /// <param name="n">The child</param>
        virtual void addChild(std::shared_ptr<Node> n) override;

    private:
        std::shared_ptr<Camera> m_camera;
        float m_radius;
        glm::vec3 m_center;
        std::vector<float> m_distances;
        int m_levels;
};