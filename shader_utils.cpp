/**
* From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
* This file is in the public domain.
* Contributors: Sylvain Beucler
*/
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
/**
* Store all the file's contents in memory, useful to pass shaders
* source code to OpenGL
*/



GLint get_attrib(GLuint program, const char *name) {
  GLint attribute = glGetAttribLocation(program, name);
  if (attribute == -1)
    fprintf(stderr, "Could not bind attribute %s\n", name);
  return attribute;
}
GLint get_uniform(GLuint program, const char *name) {
  GLint uniform = glGetUniformLocation(program, name);
  if (uniform == -1)
    fprintf(stderr, "Could not bind uniform %s\n", name);
  return uniform;
}