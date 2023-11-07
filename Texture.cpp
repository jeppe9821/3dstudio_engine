#include "Texture.h"
#include <stb_image.h>
#include <iostream>
#include <vr/FileSystem.h>
#include <vr/glErrorUtil.h>

Texture::Texture() : m_id(0), m_type(0), m_valid(false), m_textureSlot(0)
{
}

Texture::~Texture()
{
	cleanup();
}

bool Texture::create(const char* image, unsigned int slot, bool flipVertical, GLenum texType, GLenum pixelType, GLenum texFormat, GLint internalFormat, bool doDefault)
{
	if (m_valid)
	{
		cleanup();
	}

	std::string filepath = image;

	bool exist = vr::FileSystem::exists(filepath);

	std::string vrPath = vr::FileSystem::getEnv("VR_PATH");

	if (vrPath.empty())
	{
		std::cerr << "The environment variable VR_PATH is not set. It should point to the directory where the vr library is (just above models)" << std::endl;
	}

	if (!exist && !vrPath.empty())
	{
		filepath = std::string(vrPath) + "/" + filepath;
		exist = vr::FileSystem::exists(filepath);
	}

	if (!exist)
	{
		std::cerr << "Unable to locate image: " << image << std::endl;
		return false;
	}

	m_textureSlot = slot;

	m_type = texType;

	int widthImg, heightImg, numColCh;

	stbi_set_flip_vertically_on_load(flipVertical);

	unsigned char* bytes = stbi_load(filepath.c_str(), &widthImg, &heightImg, &numColCh, 0);
	if (!bytes) {
		std::cerr << "Error reading image: " << image << std::endl;
		return false;
	}

	if (numColCh == 3)
		texFormat = GL_RGB;

	glGenTextures(1, &m_id);

	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(texType, m_id);

	//CHECK_GL_ERROR_LINE_FILE();

	if(doDefault)
	{
		setParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		setParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		setParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
		setParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glTexImage2D(texType, 0, internalFormat, widthImg, heightImg, 0, texFormat, pixelType, bytes);

	if(doDefault)
	{
		glGenerateMipmap(texType);
	}

	stbi_image_free(bytes);
	glBindTexture(texType, 0);

	m_valid = true;
	return true;
}

bool Texture::create(unsigned int slot)
{
	m_valid = true;
	m_textureSlot = slot;
	m_type = GL_TEXTURE_2D;

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setParameteri(GLenum pname, GLint param)
{
	glTexParameteri(m_type, pname, param);
}

bool Texture::isValid()
{
	return m_valid;
}

void Texture::texUnit(GLuint program, const char* uniform, unsigned int unit)
{
	GLuint texUni = glGetUniformLocation(program, uniform);
	glUniform1i(texUni, unit);
}

void Texture::bind()
{
	glActiveTextureARB(GL_TEXTURE0 + m_textureSlot);
	glEnable(GL_TEXTURE_2D);

	if (m_valid)
	{
		glBindTexture(m_type, m_id);
	}
}

void Texture::unbind()
{
	if (m_valid)
	{
		glBindTexture(m_type, 0);
	}
}

void Texture::cleanup()
{
	if (m_valid)
	{
		glDeleteTextures(1, &m_id);
	}

	m_valid = false;
}

void Texture::apply(GLuint program, int i, bool flag)
{
    bind();
}

unsigned int Texture::getId()
{
	return m_id;
}