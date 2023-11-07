#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RenderToTexture;
class Camera;
class OrthographicCamera;
class Group;
class Texture;

class Shadowmap
{
    public:
        Shadowmap(GLuint depthProgram);
        void init(glm::uvec2 screenSize);
        std::shared_ptr<Texture> render(GLuint program, std::shared_ptr<Camera> camera, std::shared_ptr<Group> subtree);
        void setEnabled(bool flag);
        std::shared_ptr<OrthographicCamera> getDepthCamera();

    private:
        GLuint m_depthProgram;
        unsigned int m_renderToTextureId;
        std::shared_ptr<RenderToTexture> m_renderToTexture;
        std::shared_ptr<OrthographicCamera> m_depthCamera;
        bool m_enabled;
};