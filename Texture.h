#pragma once

#include <GL/glew.h>
#include <string>
class Texture
{
public:

    /// <summary>
    /// Constructor for an image
    /// </summary>
    /// <param name="image">path to an image on disk</param>
    /// <param name="texType"></param>
    /// <param name="slot">texture slot (default 0)</param>
    /// <param name="format"></param>
    /// <param name="pixelType"></param>
    Texture();
    bool create(const char* image, unsigned int slot=0, bool flipVertical=true, GLenum texType=GL_TEXTURE_2D, GLenum pixelType=GL_UNSIGNED_BYTE, GLenum texFormat=GL_RGBA, GLint internalFormat=GL_RGBA, bool doDefault=true);
    bool create(unsigned int slot);

    void setParameteri(GLenum pname, GLint param);

    bool isValid();

    ~Texture();

    /// Assigns a texture unit to a texture
    void texUnit(GLuint program, const char* uniform, GLuint unit);

    /// Binds a texture
    void bind();

    /// Unbinds a texture
    void unbind();

    /// Deletes a texture
    void cleanup();

    void apply(GLuint program, int i, bool flag);

    unsigned int getId();

private:
    GLuint m_id;
    GLenum m_type;
    bool m_valid;
    GLuint m_textureSlot;
    int m_slot;
    int m_activeSlot;
};