#include "GPUParticles.h"
#include <vector>
#include <iostream>
#include <cmath>
#include "glm/ext.hpp"

GPUParticles::GPUParticles(unsigned int emitNum) : m_start(std::chrono::high_resolution_clock::now()), m_particleTexture(nullptr), m_active(true), m_emitNum(emitNum)
{

}

void GPUParticles::init(GLuint program)
{
	std::vector<float> positions;

	positions.reserve(m_emitNum);

    int div3 = std::pow(m_emitNum, (1.0f/3.0f));

	for (float i = 0; i < div3*div3*div3; i++)
	{
		positions.push_back((rand() % div3) - 100);
		positions.push_back((rand() % div3) + 25);
		positions.push_back((rand() % div3) - 100);
		positions.push_back(1.0f);
	}
	std::vector<float> vel(m_emitNum * 4, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	GLuint posBuffer;
	GLuint velBuffer;
	glCreateBuffers(1, &posBuffer);
	glCreateBuffers(1, &velBuffer);

	glNamedBufferStorage(posBuffer, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(velBuffer, vel.size() * sizeof(float), vel.data(), GL_DYNAMIC_STORAGE_BIT);
	positions.clear();
	vel.clear();

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuffer);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glVertexAttribPointer(glGetAttribLocation(program, "position"), 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "position"));

	glBindBuffer(GL_ARRAY_BUFFER, velBuffer);
	glVertexAttribPointer(glGetAttribLocation(program, "velocity"), 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "velocity"));

    //Create texture
	m_particleTexture = std::shared_ptr<Texture>(new Texture());
	if(!m_particleTexture->create("textures/particle.png", 0, true, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGB, false))
	{
		std::cout << "Could not create particle texture" << std::endl;
	}
	m_particleTexture->bind();
	m_particleTexture->setParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_particleTexture->setParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_particleTexture->unbind();

	glBindVertexArray(0);
	glUseProgram(0);
}

void GPUParticles::render(glm::vec3 followPosition, std::shared_ptr<Camera> cam, GLuint program, GLuint computeProgram)
{
	glBindVertexArray(m_vao);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDisable(GL_DEPTH_TEST);
	m_particleTexture->bind();

    std::chrono::duration<float> elapsedTime = std::chrono::high_resolution_clock::now() - m_start;
	const float deltaTime = elapsedTime.count();
    m_start = std::chrono::high_resolution_clock::now();

	glUseProgram(computeProgram);
	glUniform1f(glGetUniformLocation(computeProgram, "deltaTime"), deltaTime);
	glUniform1i(glGetUniformLocation(computeProgram, "isActive"), m_active);
	glUniform3fv(glGetUniformLocation(computeProgram, "followPosition"), 1, glm::value_ptr(followPosition));

	glDispatchCompute(m_emitNum / 64, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glUseProgram(0);

	glUseProgram(program);

	glm::mat4 projection = glm::perspective(cam->getFov(), float(cam->getScreenSize()[0] / cam->getScreenSize()[1]), 0.0f, 1e3f); //we want a bigger farplane
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(cam->getView()));
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(glGetUniformLocation(program, "particleTexture"), 0);

	glDrawArrays(GL_POINTS, 0, m_emitNum);
	m_particleTexture->unbind();
	glUseProgram(0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void GPUParticles::setActive(bool flag)
{
	m_active = flag;
}

bool GPUParticles::getActive()
{
	return m_active;
}