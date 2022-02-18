#include "State.h"
#include "Light.h"
#include <iostream>
#include <vr/glErrorUtil.h>

State::State(GLuint program) : m_uniform_numberOfLights(-1), m_isProgramsMerged(false)
{
	m_program = program;
	m_material = std::shared_ptr<Material>(new Material());

	m_textures.resize(2);
}

State::State() : m_uniform_numberOfLights(-1), m_isProgramsMerged(false)
{
	m_material = std::shared_ptr<Material>(new Material());
	m_textures.resize(2);
}

State::~State()
{
}

void State::apply()
{
	if(m_program == 0)
	{
		std::cout << "Program is undefined. We cannot apply state" << std::endl;
		return;
	}

	glUseProgram(m_program);

	if(m_isProgramsMerged)
	{
		return;
	}

	if(m_material)
	{
		m_material->apply(m_program);
	}

	if(m_textures.size() > 0)
	{
		applyTextures();
	}

	if(m_lights.size() > 0)
	{
		// Update number of lights
		if (m_uniform_numberOfLights == -1)
		{
			const char *uniform_name = "numberOfLights";
			m_uniform_numberOfLights = glGetUniformLocation(m_program, uniform_name);

			if (m_uniform_numberOfLights == -1)
			{
				std::cout << "Could not bind uniform " << uniform_name << std::endl;
			}
		}

		glUniform1i(m_uniform_numberOfLights, (GLint)m_lights.size());

		// Apply lightsources
		size_t i = 0;
		for (auto l : m_lights)
		{
			if(m_lights[i]->isEnabled())
			{
				l->apply(m_program, i++);
			}
		}
	}

	if(m_polygonMode != -1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(m_cullFace != -1)
	{
		glCullFace(m_cullFace);
	}
	else
	{
		glCullFace(GL_BACK);
	}

	if(m_alphaBlendingSrc != -1 && m_alphaBlendingDst != -1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_alphaBlendingSrc, m_alphaBlendingDst);
	}

	m_isProgramsMerged = false;
}

void State::applyTextures()
{
	GLint loc = 0;
	std::vector<int> slotActive;
	std::vector<int> slots;
	slotActive.resize(m_textures.size());
	slots.resize(m_textures.size());

	for (int i = 0; i < m_textures.size(); i++)
	{
		slots[i] = i;
		slotActive[i] = m_textures[i] != nullptr;

		if (slotActive[i])
		{
			m_textures[i]->bind();
		}
	}

	loc = glGetUniformLocation(m_program, "material.textures");
	glUniform1iv(loc, (GLsizei)slots.size(), slots.data());

	CHECK_GL_ERROR_LINE_FILE();

	loc = glGetUniformLocation(m_program, "material.activeTextures");
	glUniform1iv(loc, (GLsizei)slotActive.size(), slotActive.data());
}

void State::merge(std::shared_ptr<State> state)
{
	if(state->getProgram() != 0)
	{
		if(m_program != 0 && state->getProgram() != m_program)
		{
			m_isProgramsMerged = true;
		}

		m_program = state->getProgram();
	}

	if(state->getAlphaBlendingSrc() != -1 && state->getAlphaBlendingDst() != -1)
	{
		m_alphaBlendingSrc = state->getAlphaBlendingSrc();
		m_alphaBlendingDst = state->getAlphaBlendingDst();
	}

	if(state->getPolygonMode() != -1)
	{
		setPolygonMode(state->getPolygonMode());
	}

	if(getCullFace() != -1)
	{
		setCullFace(state->getCullFace());
	}

	if(getMaterial() != state->getMaterial() && getMaterial() != nullptr && state->getMaterial() != nullptr)
	{
		m_material->merge(state->m_material);
	}

	if(state->getLights().size() > 0)
	{
		for(auto light : state->getLights())
		{
			add(light);
		}
	}

	if(!state->getTextures().empty())
	{
		std::vector<int> texturesIndexToMerge;
		for(int i = 0; i < state->getTextures().size(); i++)
		{
			if(state->getTexture(i) != nullptr)
			{
				texturesIndexToMerge.push_back(i);
			}
		}

		if(texturesIndexToMerge.size() > 0)
		{
			for(auto index : texturesIndexToMerge)
			{
				m_textures[index] = state->getTexture(index);
			}
		}
	}
}

bool State::isProgramsMerged()
{
	return m_isProgramsMerged;
}

void State::enableAlphaBlending(GLenum src, GLenum dst)
{
	m_alphaBlendingSrc = src;
	m_alphaBlendingDst = dst;
}

void State::add(std::shared_ptr<Light>& light)
{
	m_lights.push_back(light);
}

void State::setMaterial(std::shared_ptr<Material>& material)
{
  	m_material = material;
}

void State::setLightEnabled(int i, bool flag)
{
	m_lights[i]->setEnabled(flag);
}

void State::setPolygonMode(GLenum mode)
{
	m_polygonMode = mode;
}

void State::setCullFace(GLenum cullFace)
{
	m_cullFace = cullFace;
}

void State::setTexture(std::shared_ptr<Texture> texture, unsigned int unit)
{
	m_textures[unit] = texture;
}

GLenum State::getAlphaBlendingSrc()
{
	return m_alphaBlendingSrc;
}

GLenum State::getAlphaBlendingDst()
{
	return m_alphaBlendingDst;
}

const std::vector<std::shared_ptr<Light>>& State::getLights()
{
	return m_lights;
}

std::shared_ptr<Material> State::getMaterial()
{
	return m_material;
}

void State::setProgram(GLuint program)
{
	m_program = program;
}

GLuint State::getProgram()
{
	return m_program;
}

bool State::getLightEnabled()
{
	return true;
}

GLenum State::getPolygonMode()
{
	return m_polygonMode;
}

GLenum State::getCullFace()
{
	return m_cullFace;
}

std::shared_ptr<Texture> State::getTexture(unsigned int slot)
{
	return m_textures[slot];
}

std::vector<std::shared_ptr<Texture>> State::getTextures()
{
	return m_textures;
}