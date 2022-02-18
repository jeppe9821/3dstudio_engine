#ifndef _WIN32
/*
Workaround for some kind of linker bug. See https://bugs.launchpad.net/ubuntu/+source/nvidia-graphics-drivers-319/+bug/1248642
*/
#include <pthread.h>
void junk() {
  int i;
  i = pthread_getconcurrency();
};
#endif


#ifdef _WIN32
#include <windows.h>
#endif

/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "Application.h"

#include <glm/vec2.hpp>

// Weak pointer to the application, so we can access it in callbacks
std::weak_ptr<Application> g_applicationPtr;


void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  int shouldClose = 0;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  if (key == GLFW_KEY_R && action == GLFW_PRESS)
    if (auto app = g_applicationPtr.lock())
      app->reloadScene();

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
  if (auto app = g_applicationPtr.lock())
    app->setScreenSize(width, height);
}


GLFWwindow* initializeWindows(int width, int height)
{
  GLFWwindow* window = nullptr;

  // Initialize GLFW
  if (!glfwInit())
    return nullptr;

  window = glfwCreateWindow(width, height, "ObjViewer", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, keyboard_callback);

  glfwSetWindowSizeCallback(window, window_size_callback);


  // Initialize the "OpenGL Extension Wrangler" library
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    glfwTerminate();
    glfwDestroyWindow(window);

    return nullptr;
  }

  printf("GL version: %s\n", glGetString(GL_VERSION));
  printf("GL shading language version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  return window;
}

void cleanupWindows(GLFWwindow *window)
{
  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
}


int main(int argc, char** argv) 
{
  const unsigned SCREEN_WIDTH = 1920;
  const unsigned SCREEN_HEIGHT = 1080;

  GLFWwindow *window = initializeWindows(SCREEN_WIDTH, SCREEN_HEIGHT);

  std::shared_ptr<Application> application = std::make_shared<Application>(SCREEN_WIDTH, SCREEN_HEIGHT);
  g_applicationPtr = application;

  std::string model_filename = (char*) "models/monkey.obj";
  if (argc > 1)
    model_filename = argv[1];

  std::string v_shader_filename = "shaders/phong-shading.vert.glsl";
  std::string  f_shader_filename = "shaders/phong-shading.frag.glsl";

  if (argc < 2 ) {
    std::cerr << "Loading default model: " << model_filename << std::endl;
    std::cerr << "\n\nUsage: " << argv[0] << " <model-file>" << std::endl;
  }

  if (!application->initResources(model_filename, v_shader_filename, f_shader_filename))
  {
    cleanupWindows(window);
    return 1;
  }

  application->initView();

  //glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window))
  {
    application->update(window);

    glfwSwapBuffers(window);
    glfwPollEvents();

    application->processInput(window);
  }

  cleanupWindows(window);

  return 0;
}

