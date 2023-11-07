#include "RenderToTexture.h"
#include "Group.h"
#include "RenderVisitor.h"
#include "Texture.h"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderToTexture::RenderToTexture(unsigned int textureSlot) : m_renderVisitor(std::shared_ptr<RenderVisitor>(new RenderVisitor()))
{
    m_exportTexture = std::shared_ptr<Texture>(new Texture());
    m_exportTexture->create(textureSlot);

    //Framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_exportTexture->getId(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderToTexture::prepare()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderToTexture::unprepare(glm::uvec2 screensize)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, screensize.x, screensize.y);
    //glClear(GL_COLOR_BUFFER_BIT);
}

void RenderToTexture::render(GLuint program, std::shared_ptr<Camera> camera, std::shared_ptr<Group> node)
{
    GLuint prevProgram = 0;
    if(node->hasState())
    {
        prevProgram = node->getState()->getProgram();
        node->getState()->setCullFace(GL_FRONT);
	    node->getState()->setProgram(program);
    }

	glUseProgram(program);
	camera->init(program);
	camera->apply(program);
	glUseProgram(0);

    m_renderVisitor->resetState();
    m_renderVisitor->visit(*node);

    if(node->hasState())
    {
        node->getState()->setCullFace(GL_BACK);
        node->getState()->setProgram(prevProgram);
    }
}

void RenderToTexture::clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<Texture> RenderToTexture::getTexture()
{
    return m_exportTexture;
}