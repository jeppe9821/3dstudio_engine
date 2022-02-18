

#pragma once

#include <memory>
#include <vector>
#include <sstream>

#include "Light.h"
#include "Camera.h"
#include "Group.h"
#include "FPSCounter.h"
#include "Camera.h"
#include "Loader.h"
#include "RenderVisitor.h"
#include "UpdateVisitor.h"

class LightMoveCallback;

/// <summary>
/// The application
/// </summary>
class Application
{
    public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="width">The width for the application</param>
        /// <param name="width">The height for the application</param>
        Application(unsigned int width, unsigned int height);


        /// <summary>
        /// Initilize the resources. This starts the program
        /// </summary>
        /// <param name="model_filename">The filename for the model to init</param>
        /// <param name="vshader_filename">The filename for the vertex shader to init</param>
        /// <param name="fshader_filename">The filename for the fragment shader to init</param>
        /// <returns>Flag for if init resources completed or not</returns>
        bool initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename);

        /// <summary>
        /// Initilize the view
        /// </summary>
        void initView();

        /// <summary>
        /// Updates the application
        /// </summary>
        /// <param name="window">The GLFW window</param>
        void update(GLFWwindow* window);

        /// <summary>
        /// Processes the input for the application
        /// </summary>
        /// <param name="window">The GLFW window</param>
        void processInput(GLFWwindow* window);

        /// <summary>
        /// Sets the screen size of the application
        /// </summary>
        /// <param name="width">The width of the screen</param>
        /// <param name="height">The height of the screen</param>
        void setScreenSize(unsigned int width, unsigned int height);

        /// <summary>
        /// Reloads the application
        /// </summary>
        void reloadScene();
    private:
        //Variables
        std::shared_ptr<Group> m_rootNode;
        std::shared_ptr<RenderVisitor> m_renderVisitor;
        std::shared_ptr<UpdateVisitor> m_updateVisitor;
        std::shared_ptr<FPSCounter> m_fpsCounter;
        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<LightMoveCallback> m_lightMoveCallback;

        std::string m_loadedFilename;
        std::string m_loadedVShader;
        std::string m_loadedFShader;
        glm::uvec2 m_screenSize;

        GLuint m_program;
        GLuint m_toonProgram;

        /// <summary>
        /// Initilizes the application shaders for a given program
        /// </summary>
        /// <param name="program">The program to init</param>
        /// <param name="vshader_filename">The vertex shader</param>
        /// <param name="fshader_filename">The fragment shader</param>
        /// <returns>A flag depending on if the shaders initilized or not</returns>
        bool initShaders(GLuint *program, const std::string& vshader_filename, const std::string& fshader_filename);

        /// <summary>
        /// Parses an object file
        /// </summary>
        /// <param name="model_filename">The model to parse</param>
        /// <param name="program">The program to give for the loaded geometry</param>
        /// <returns>The transform for the parsed object</returns>
        std::shared_ptr<Transform> parseObj(std::shared_ptr<Obj> model_filename, GLuint program);

        /// <summary>
        /// Builds suzanne (the monkey)
        /// </summary>
        /// <param name="model_filename">The model to parse</param>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildSuzanne(std::string model_filename);

        /// <summary>
        /// Builds cow
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildCow();

        /// <summary>
        /// Builds Cube
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildCube();

        /// <summary>
        /// Builds Pyramid
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildPyramid();

        /// <summary>
        /// Builds the floor
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildFloor();

        /// <summary>
        /// Builds the sphere
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildSphere();

        /// <summary>
        /// Builds the tree
        /// </summary>
        /// <returns>The transform for the built object</returns>
        std::shared_ptr<Transform> buildTree();

        /// <summary>
        /// Builds all the geometry for scenes loaded without xml files (API scene)
        /// </summary>
        void buildGeometry(std::string model_filename);
};