#include <GL/glew.h>
#include "Camera.h"

#include <memory>

class Group;
class RenderVisitor;
class Texture;

class RenderToTexture
{
    public:
        RenderToTexture(unsigned int textureSlot);
        void render(GLuint program, std::shared_ptr<Camera> camera, std::shared_ptr<Group> node);
        void prepare();
        void unprepare(glm::uvec2 screensize);
        void clear();
        std::shared_ptr<Texture> getTexture();

    private:
        unsigned int m_fbo;
        std::shared_ptr<RenderVisitor> m_renderVisitor;
        std::shared_ptr<Texture> m_exportTexture;
};