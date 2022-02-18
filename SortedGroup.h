#pragma once

#include "Transform.h"
#include "UpdateCallback.h"

class Camera;
class Node;

/// <summary>
/// The SortedGroup class, this sorts a group of nodes depending on the angle of the camera
/// </summary>
class SortedGroup : public Transform, public UpdateCallback
{
    public:
        /// <summary>
        /// The constructor for state, given by Node
        /// </summary>
        /// <param name="state">state</param>
        SortedGroup(std::shared_ptr<State> state);

        /// <summary>
        /// Empty Constructor, givne by Node
        /// </summary>
        SortedGroup();

        /// <summary>
        /// The constructor for Camera, given by Node
        /// </summary>
        /// <param name="camera">Camera</param>
        SortedGroup(std::shared_ptr<Camera> camera);

        /// <summary>
        /// The destructor given by node
        /// </summary>        
        virtual ~SortedGroup() override;

        /// <summary>
        /// Adds a child given by Group
        /// </summary>
        virtual void addChild(std::shared_ptr<Node> child) override;

        /// <summary>
        /// Updates the node given by UpdateCallback
        /// </summary>
        void update(Node &n) override;
    private:
        std::shared_ptr<Camera> m_camera;
        glm::vec3 m_center;
        bool m_rightOrder;
};