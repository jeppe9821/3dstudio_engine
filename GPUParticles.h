#pragma once

#include <GL/glew.h>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"
#include <chrono>
#include <ratio>
#include "Camera.h"

class GPUParticles
{
    public:
        /// <summary>
        /// Constructor
        /// </summary>
        GPUParticles(unsigned int emitNum);

        /// <summary>
        /// Initilizes the gpu particles
        /// </summary>
        void init(GLuint program);

        /// <summary>
        /// Renders the GPU particles
        /// <param name="followPosition">The position the particles should follow</param>
        /// <param name="viewMatrix">The viewmatrix of the camera</param>
        /// <param name="screenSize">The screensize</param>
        /// <param name="program">The program containing vert,frag,geo shaders</param>
        /// <param name="computeProgram">The compute program for the compute shaders</param>
        /// </summary>
        void render(glm::vec3 followPosition, std::shared_ptr<Camera> cam, GLuint program, GLuint computeProgram);

        /// <summary>
        /// Sets if the particles should be actively moving or not
        /// <param name="flag">Flag</param>
        /// </summary>
        void setActive(bool flag);

        /// <summary>
        ///  Returns active flag
        /// </summary>
        bool getActive();
    private:
        GLuint m_vao;
        std::shared_ptr<Texture> m_particleTexture;
        bool m_active;
        unsigned int m_emitNum;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};