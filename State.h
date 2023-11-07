#pragma once

#include "Material.h"
#include <vector>
#include <memory>

class Light;

class State
{
	public:
		State(GLuint program);
		State();
		~State();

		void apply();
		void applyTextures();
		void merge(std::shared_ptr<State> state);
		void add(std::shared_ptr<Light>& light);
		void enableAlphaBlending(GLenum sfactor, GLenum dfactor);

		void setMaterial(std::shared_ptr<Material>& material);
		void setLightEnabled(int i, bool flag);
		void setPolygonMode(GLenum mode);
		void setCullFace(GLenum cullFace);
		void setTexture(std::shared_ptr<Texture> texture, unsigned int unit);

		void setProgram(GLuint program);
		GLuint getProgram();
		GLenum getCullFace();
		GLenum getPolygonMode();
		bool getLightEnabled();
		std::shared_ptr<Material> getMaterial();
		const std::vector<std::shared_ptr<Light>>& getLights();
		GLenum getAlphaBlendingSrc();
		GLenum getAlphaBlendingDst();
		std::shared_ptr<Texture> getTexture(unsigned int slot);
		std::vector<std::shared_ptr<Texture>> getTextures();

	private:
		std::shared_ptr<Material> m_material;
		GLuint m_program = 0;
		std::vector<std::shared_ptr<Light>> m_lights;
		GLint m_uniform_numberOfLights;

		GLenum m_polygonMode = -1;
		GLenum m_cullFace = -1;
		GLenum m_alphaBlendingSrc = -1;
		GLenum m_alphaBlendingDst = -1;

		std::vector<std::shared_ptr<Texture>> m_textures;
};