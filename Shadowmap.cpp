#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shadowmap.h"
#include "RenderToTexture.h"
#include "OrthographicCamera.h"
#include "Group.h"
#include "Texture.h"

#include <iostream>

Shadowmap::Shadowmap(GLuint depthProgram) :
    m_renderToTextureId(30),
    m_depthProgram(depthProgram),
    m_enabled(false)
{
}

void Shadowmap::init(glm::uvec2 screenSize)
{
    m_renderToTexture = std::shared_ptr<RenderToTexture>(new RenderToTexture(m_renderToTextureId));
    m_depthCamera = std::shared_ptr<OrthographicCamera>(new OrthographicCamera());

    glUseProgram(m_depthProgram);
    m_depthCamera->init(m_depthProgram);
    m_depthCamera->setScreenSize(screenSize);
    glUseProgram(0);
}

std::shared_ptr<Texture> Shadowmap::render(GLuint program, std::shared_ptr<Camera> camera, std::shared_ptr<Group> subtree)
{
    if(!subtree->hasCachedBoundingBox())
    {
        subtree->calculateBoundingBox();
    }

    BoundingBox b = subtree->getCachedBoundingBox();

    m_depthCamera->setTop(b.getRadius());
    m_depthCamera->setNearFar(glm::uvec2(1.0f, b.getRadius() * 3.0f));

    //Calculate the light space matrix
    glUseProgram(m_depthProgram);
    m_depthCamera->init(m_depthProgram);
    m_depthCamera->apply(m_depthProgram);
    glUseProgram(0);

	m_renderToTexture->prepare();
	m_renderToTexture->render(m_depthProgram, m_depthCamera, subtree);

    glUseProgram(m_depthProgram);
    glUniformMatrix4fv(glGetUniformLocation(m_depthProgram, "u_lightSpaceMatrix"), 1, false, glm::value_ptr(m_depthCamera->getLightSpaceMatrix()));
    glUseProgram(0);

	m_renderToTexture->unprepare(camera->getScreenSize());

	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "u_depthTexture"), m_renderToTextureId);
	glUniformMatrix4fv(glGetUniformLocation(program, "u_lightSpaceMatrix"), 1, false, glm::value_ptr(m_depthCamera->getLightSpaceMatrix()));

    camera->init(program);
    camera->apply(program);
    glUseProgram(0);

    return m_renderToTexture->getTexture();
}

void Shadowmap::setEnabled(bool flag)
{
    m_enabled = flag;
    if(flag == false)
    {
        m_renderToTexture->clear();
    }
}

std::shared_ptr<OrthographicCamera> Shadowmap::getDepthCamera()
{
    return m_depthCamera;
}