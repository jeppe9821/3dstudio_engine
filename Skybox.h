#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <memory>
#include "Geometry.h"

class Camera;

class Skybox
{
    public:
        Skybox();
        ~Skybox();
        void render(GLuint program, std::shared_ptr<Camera> camera);
        unsigned int getCubemapTexture();
        void bind(int i);
        void unbind();
    private:
        unsigned int m_CubemapTexture;
        unsigned int m_SkyboxVAO;
        unsigned int m_SkyboxVBO;

        void prepare();
        unsigned int loadTexture(std::vector<std::string> faces);
};