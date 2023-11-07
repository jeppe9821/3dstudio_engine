#include "UpdateCallback.h"

#include <memory>

class Node;
class Light;
class OrthographicCamera;

class DepthCameraSetPositionCallback : public UpdateCallback
{
    public:
        DepthCameraSetPositionCallback(std::shared_ptr<Light> light, std::shared_ptr<OrthographicCamera> orthoCamera);
        void update(Node &n) override;

    private:
        std::shared_ptr<Light> m_light;
        std::shared_ptr<OrthographicCamera> m_orthoCamera;
};