#include "Application.h"

#include <iostream>
#include <sstream>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vr/FileSystem.h>
#include <vr/shaderUtils.h>
#include <vr/DrawText.h>

#include "Geometry.h"
#include "Transform.h"
#include "RotateCallback.h"
#include "LOD.h"
#include "LightMoveCallback.h"
#include "SortedGroup.h"
#include "DepthCameraSetPositionCallback.h"
#include "DrawCameraStatus.h"
#include "Skybox.h"
#include "glm/ext.hpp"
#include <cstdlib>

Application::Application(unsigned int width, unsigned height)
	: m_screenSize(width, height)
{
	m_fpsCamera = std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera);

	m_camera = m_fpsCamera;

  	m_fpsCounter = std::make_shared<FPSCounter>();
	m_drawCameraStatus = std::make_shared<DrawCameraStatus>();
}

bool Application::initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename)
{
	m_loadedVShader = vshader_filename;
	m_loadedFShader = fshader_filename;
	m_loadedFilename = model_filename;

	m_program = glCreateProgram();
	m_toonProgram = glCreateProgram();
	m_depthProgram = glCreateProgram();
	m_skyboxProgram = glCreateProgram();
	m_billboardProgram = glCreateProgram();
	m_gpuComputeProgram = glCreateProgram();
	m_gpuProgram = glCreateProgram();

	m_renderVisitor = std::shared_ptr<RenderVisitor>(new RenderVisitor());
	m_updateVisitor = std::shared_ptr<UpdateVisitor>(new UpdateVisitor());

	m_skybox = std::shared_ptr<Skybox>(new Skybox());
	m_gpuParticles = std::shared_ptr<GPUParticles>(new GPUParticles(1 << 21)); //2 million particles, bitshift gives us a nice number that is raised by 3 by (1 << 7).
																			   //Other good numbers to put here are (1 << 15), (1 << 9). Using a number that can't be raised by 3 will result in certian artifacts such as
																			   //certain particles not moving.

	m_renderParticles = false;

	if (!initShaders(&m_program, vshader_filename, fshader_filename))
	{
		std::cout << "Could not initilize phong program" << std::endl;
		return false;
	}

	if (!initShaders(&m_toonProgram, "shaders/toon-shading.vert.glsl", "shaders/toon-shading.frag.glsl"))
	{
		std::cout << "Could not initilize toon program" << std::endl;
		return false;
	}

	if(!initShaders(&m_depthProgram, "shaders/depth-shading.vert.glsl", "shaders/depth-shading.frag.glsl"))
	{
		std::cout << "Could not initilize depth program" << std::endl;
		return false;
	}

	if(!initShaders(&m_skyboxProgram, "shaders/skybox-shading.vert.glsl", "shaders/skybox-shading.frag.glsl"))
	{
		std::cout << "Could not initilize skybox program" << std::endl;
		return false;
	}

	if(!initShaders(&m_billboardProgram, "shaders/billboard-shading.vert.glsl", "shaders/billboard-shading.frag.glsl"))
	{
		std::cout << "Could not initilize billboard program" << std::endl;
		return false;
	}

	if(!initGpuShaders(&m_gpuProgram, "shaders/gpu-particles/gpu.vert.glsl", "shaders/gpu-particles/gpu.frag.glsl", "shaders/gpu-particles/gpu.geo.glsl"))
	{
		std::cout << "Could not initilize gpu default program" << std::endl;
		return false;
	}

	if(!initComputeShader(&m_gpuComputeProgram, "shaders/gpu-particles/gpu.comp.glsl"))
	{
		std::cout << "Could not initilize GPU compute program" << std::endl;
		return false;
	}

	m_fpsCamera->init(m_program);
	m_fpsCamera->setScreenSize(m_screenSize);

	m_shadowmap = std::shared_ptr<Shadowmap>(new Shadowmap(m_depthProgram));
	m_shadowmap->init(m_screenSize);

	std::string ext = vr::FileSystem::getFileExtension(model_filename);
	std::shared_ptr<Obj> obj;

	std::shared_ptr<State> defaultState = std::shared_ptr<State>(new State(m_program));

	m_rootNode = std::shared_ptr<Group>(new Group(defaultState));

	m_rootNode->setName("root");

	if (ext == "xml" || ext == "XML")
	{
		std::shared_ptr<XmlScene> scene = std::shared_ptr<XmlScene>(new XmlScene);

		std::cout << "Loading xml" << std::endl;

		if (!loadXml(model_filename, scene))
		{
			return false;
		}

		std::cout << "Finished loading xml. Objects: " << scene->objects.size() << std::endl;

		//parse XML
		for(int i = 0; i < scene->objects.size(); i++)
		{
			std::shared_ptr<Obj> obj = scene->objects[i];
			std::shared_ptr<Transform> n = parseObj(obj);
			m_rootNode->addChild(n);
		}

		if (m_rootNode->getChildren().empty())
		{
			std::cerr << "Empty scene, something when wrong when loading files" << std::endl;
			return false;
		}
	}
	else
	{
		buildGeometry(model_filename);
	}

	std::shared_ptr<Light> light1 = std::shared_ptr<Light>(new Light);
	light1->setDiffuse(glm::vec4(1, 1, 1, 1));
	light1->setSpecular(glm::vec4(1, 1, 1, 1));
	light1->setPosition(glm::vec4(0.0, 200.0f, 0.0f, 0.0));

	std::shared_ptr<DepthCameraSetPositionCallback> callback = std::shared_ptr<DepthCameraSetPositionCallback>(new DepthCameraSetPositionCallback(light1, m_shadowmap->getDepthCamera()));
	m_rootNode->addUpdateCallback(callback);

	std::shared_ptr<Transform> lightTransform = parseObj(loadObj("models/Sun/13913_Sun_v2_l3.obj"));
	lightTransform->setInitialTransform(glm::mat4(1));
	//m_rootNode->addChild(lightTransform);

	m_lightMoveCallback = std::shared_ptr<LightMoveCallback>(new LightMoveCallback(light1, lightTransform, m_camera));

	m_rootNode->getState()->add(light1);
	m_rootNode->addUpdateCallback(m_lightMoveCallback);

	m_gpuParticles->init(m_gpuProgram);

	initView(m_camera);

	return 1;
}

void Application::initView(std::shared_ptr<Camera> camera)
{
	BoundingBox box = m_rootNode->calculateBoundingBox();
	float radius = box.getRadius();

	float distance = radius * 1.5f;
	glm::vec3 eye = glm::vec3(0, radius, distance);
	glm::vec3 direction = glm::normalize(box.getCenter() - eye);

	std::shared_ptr<Light> light = m_rootNode->getState()->getLights().front();
	glm::vec4 position;
	position = glm::vec4(eye + glm::vec3(3, 2, 100), 0);
	light->setPosition(position);

	camera->set(eye, direction, glm::vec3(0.0, 1.0, 0.0));
	camera->setNearFar(glm::vec2(0.1, 1e3f));
	camera->setSceneScale(0.01f * radius);
	camera->setFov(90);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
}

void Application::update(GLFWwindow* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_updateVisitor->visit(*m_rootNode);

	//Render skybox
	m_skybox->render(m_skyboxProgram, m_camera);

	//Apply sintime for animation on multi textured objects
	glUniform1f(glGetUniformLocation(m_program, "sinTime"), glm::sin(glfwGetTime()));

	//Apply shadowmap
	if(m_renderShadowmap)
	{
		std::shared_ptr<Texture> renderedShadowmap = m_shadowmap->render(m_program, m_camera, m_rootNode);
		renderedShadowmap->bind();
		render(m_camera, m_program);
		renderedShadowmap->unbind();
	}
	else
	{
		render(m_camera, m_program);
	}

	//Render toon without shadows
	render(m_camera, m_toonProgram);

	//Render billboard
	render(m_camera, m_billboardProgram);

	//Render FPS counter
	m_fpsCounter->render(window);

	//Render camera status
	m_drawCameraStatus->render(window, m_lightMoveCallback);

	std::shared_ptr<Light> light = m_rootNode->getState()->getLights().front();
	m_camera->init(m_program);
	m_camera->apply(m_program);
	m_gpuParticles->setActive(m_renderParticles);
	m_gpuParticles->render((glm::vec3(light->getPosition() * glm::vec4(15.0f, 15.0f, 15.0f, 1.0f))), m_camera, m_gpuProgram, m_gpuComputeProgram);
}

void Application::processInput(GLFWwindow* window)
{
	if(m_wait)
	{
		m_tick++;
		if(m_tick > 60)
		{
			m_tick = 0;
			m_wait = false;
		}
	}
	else
	{
		if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		{
			m_shadowmap->setEnabled(false);
			m_renderShadowmap = !m_renderShadowmap;
			m_wait = true;
		}
		if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		{
			m_wait = true;
			m_renderParticles = !m_renderParticles;
		}
	}

	m_fpsCamera->processInput(window);
	m_lightMoveCallback->processInput(window);
}

void Application::reloadScene()
{
	initResources(m_loadedFilename, m_loadedVShader, m_loadedFShader);
	initView(m_fpsCamera);
}

void Application::setScreenSize(unsigned int width, unsigned int height)
{
	m_camera->setScreenSize(glm::uvec2(width, height));
	glViewport(0, 0, width, height);
}

void Application::render(std::shared_ptr<Camera> camera, GLuint program)
{
	glUseProgram(program);
	camera->init(program);
	camera->apply(program);
	glUseProgram(0);

	m_renderVisitor->resetState();
	m_renderVisitor->visit(*m_rootNode);
}

bool Application::initShaders(GLuint *program, const std::string& vshader_filename, const std::string& fshader_filename)
{
	/* Compile and link shaders */
	GLint link_ok = GL_FALSE;
	GLint validate_ok = GL_FALSE;
	GLuint vs, fs;
	if ((vs = vr::loadShader(vshader_filename, GL_VERTEX_SHADER)) == 0) return false;
	if ((fs = vr::loadShader(fshader_filename, GL_FRAGMENT_SHADER)) == 0) return false;

	glAttachShader(*program, vs);
	glAttachShader(*program, fs);
	glLinkProgram(*program);
	glGetProgramiv(*program, GL_LINK_STATUS, &link_ok);

	if (!link_ok)
	{
		fprintf(stderr, "glLinkProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	glValidateProgram(*program);
	glGetProgramiv(*program, GL_VALIDATE_STATUS, &validate_ok);

	if (!validate_ok)
	{
		fprintf(stderr, "glValidateProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	return true;
}

bool Application::initComputeShader(GLuint *program, const std::string& filename)
{
	GLint link_ok = GL_FALSE;
	GLint validate_ok = GL_FALSE;

	GLuint cs;

	cs = vr::loadShader("shaders/gpu-particles/gpu.comp.glsl", GL_COMPUTE_SHADER);

	glAttachShader(*program, cs);
	glLinkProgram(*program);

	glGetProgramiv(*program, GL_LINK_STATUS, &link_ok);

	if (!link_ok)
	{
		fprintf(stderr, "initComputeShader_glLinkProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	glValidateProgram(*program);
	glGetProgramiv(*program, GL_VALIDATE_STATUS, &validate_ok);

	if (!validate_ok)
	{
		fprintf(stderr, "initComputeShader_glValidateProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	return true;
}

bool Application::initGpuShaders(GLuint *program, const std::string& vshader_filename, const std::string& fshader_filename, const std::string& gshader_filename)
{
	/* Compile and link shaders */
	GLint link_ok = GL_FALSE;
	GLint validate_ok = GL_FALSE;
	GLuint vs, fs, gs;

	vs = vr::loadShader(vshader_filename, GL_VERTEX_SHADER);
	gs = vr::loadShader(gshader_filename, GL_GEOMETRY_SHADER);
	fs = vr::loadShader(fshader_filename, GL_FRAGMENT_SHADER);

	glAttachShader(*program, vs);
	glAttachShader(*program, gs);

	glProgramParameteriEXT(*program, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
	glProgramParameteriEXT(*program, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	glProgramParameteriEXT(*program, GL_GEOMETRY_VERTICES_OUT_EXT, 4);

	glAttachShader(*program, fs);
	glLinkProgram(*program);

	glGetProgramiv(*program, GL_LINK_STATUS, &link_ok);

	if (!link_ok)
	{
		fprintf(stderr, "initGpuShaders_glLinkProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	glValidateProgram(*program);
	glGetProgramiv(*program, GL_VALIDATE_STATUS, &validate_ok);

	if (!validate_ok)
	{
		fprintf(stderr, "initGpuShaders_glValidateProgram:");
		vr::printCompilationError(*program);
		return false;
	}

	return true;
}

std::shared_ptr<Transform> Application::parseObj(std::shared_ptr<Obj> obj)
{
	std::shared_ptr<Transform> n = std::shared_ptr<Transform>(new Transform());
	n->setName("Transform_" + obj->name);
	n->setInitialTransform(obj->initialTransform);

	for(int i = 0; i < obj->meshes.size(); i++)
	{
		std::shared_ptr<State> geometryState = std::shared_ptr<State>(new State());
		geometryState->setMaterial(obj->meshes[i].material);

		if(obj->meshes[i].texture != nullptr)
		{
			geometryState->setTexture(obj->meshes[i].texture, 0);
		}

		std::shared_ptr<Geometry> g = std::shared_ptr<Geometry>(new Geometry(geometryState));

		g->setVertices(obj->meshes[i].vertices);
		g->setNormals(obj->meshes[i].normals);
		g->setTexCoords(obj->meshes[i].texCoords);
		g->setElements(obj->meshes[i].elements);

		g->setName("Geo_"+ obj->name);

		g->init(m_program);

		std::shared_ptr<Transform> transform = std::shared_ptr<Transform>(new Transform());
		transform->setInitialTransform(obj->meshes[i].object2world);
		transform->setName("Transform2_" + obj->name);

		transform->addChild(g);
		n->addChild(transform);
	}

	return n;
}

std::shared_ptr<Transform> Application::buildQuad()
{
	std::shared_ptr<Transform> transform = std::shared_ptr<Transform>(new Transform());
	std::shared_ptr<Geometry> geometry = std::shared_ptr<Geometry>(new Geometry());

	geometry->addVertex(0.5f, -0.5f, 0.0f, 1.0f);
	geometry->addVertex(0.5f,  0.5f, 0.0f, 1.0f);
	geometry->addVertex(-0.5f,  0.5f, 0.0f, 1.0f);
	geometry->addVertex(-0.5f, -0.5f, 0.0f, 1.0f);

	glm::vec3 normal = glm::cross(glm::vec3(0.5f, -0.5f, 0.0f) - glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f) - glm::vec3(-0.5f, 0.5f, 0.0f));

	geometry->addNormal(normal.x, normal.y, normal.z);
	geometry->addNormal(normal.x, normal.y, normal.z);
	geometry->addNormal(normal.x, normal.y, normal.z);
	geometry->addNormal(normal.x, normal.y, normal.z);

	geometry->addTexCoord(1.0f, 1.0f);
	geometry->addTexCoord(1.0f, 0.0f);
	geometry->addTexCoord(0.0f, 0.0f);
	geometry->addTexCoord(0.0f, 1.0f);

	geometry->addElement(0);
	geometry->addElement(1);
	geometry->addElement(3);
	geometry->addElement(1);
	geometry->addElement(2);
	geometry->addElement(3);

	if(!geometry->init(m_program))
	{
		std::cerr << "Could not initilize Quad" << std::endl;
	}

	std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(new Texture());
	texture->create("textures/tree.png", 0, false);

	transform->setState(std::shared_ptr<State>(new State(m_billboardProgram)));
	transform->getState()->setTexture(texture, 0);

	transform->getState()->enableAlphaBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	transform->scale(glm::vec3(5, 5, 5));
	transform->translate(glm::vec3(0, 0.3f, 0));

	transform->addChild(geometry);

	return transform;
}

std::shared_ptr<Transform> Application::buildSuzanne(std::string model_filename)
{
	std::shared_ptr<Obj> obj = loadObj(model_filename);

	if (!obj)
	{
		return nullptr;
	}

	std::shared_ptr<Transform> transform = parseObj(obj);
	transform->setState(std::shared_ptr<State>(new State()));
	transform->scale(glm::vec3(1, 1, 1));
	transform->translate(glm::vec3(10, 0, 0));
	transform->addUpdateCallback(std::shared_ptr<RotateCallback>(new RotateCallback()));

	return transform;
}

std::shared_ptr<Transform> Application::buildCow()
{
	std::shared_ptr<LOD> lod(new LOD(m_camera));

	std::vector<std::string> lodModels;

	lodModels.push_back("models/cow/cow_0.99.obj");
	lodModels.push_back("models/cow/cow_0.5.obj");
	lodModels.push_back("models/cow/cow_0.3.obj");
	lodModels.push_back("models/cow/cow_0.1.obj");
	lodModels.push_back("models/cow/cow_0.05.obj");
	lodModels.push_back("models/cow/cow_0.01.obj");

	for(int i = 0; i < 6; i++)
	{
		std::shared_ptr<Obj> obj = loadObj(lodModels[i]);

		if (!obj)
		{
			continue;
		}

		std::shared_ptr<Transform> transform = parseObj(obj);
		transform->setState(std::shared_ptr<State>(new State()));
		transform->getState()->setPolygonMode(GL_LINE);
		lod->addChild(transform);
	}

	lodModels.clear();

	m_rootNode->addUpdateCallback(lod);

	lod->scale(glm::vec3(0.01f, 0.01f, 0.01f));

	lod->translate(glm::vec3(500, -100, 0));

	lod->setName("LOD");

	return lod;
}

std::shared_ptr<Transform> Application::buildCube()
{
	std::shared_ptr<Obj> obj = loadObj("models/models/cube_brick.obj");
	std::shared_ptr<Transform> transform = parseObj(obj);

	std::shared_ptr<State> state = std::shared_ptr<State>(new State());

	std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(new Texture());
	texture->create("textures/pexels-anni-roenkae-2832432.jpg", 1);
	state->setTexture(texture, 1);

	transform->setState(state);

	transform->scale(glm::vec3(1,1,1));
	transform->translate(glm::vec3(-5, 0, 0));

	return transform;
}

std::shared_ptr<Transform> Application::buildPyramid()
{
	std::shared_ptr<State> geoState = std::shared_ptr<State>(new State());
	std::shared_ptr<Geometry> pyramid(new Geometry(geoState));

	pyramid->addVertex(1.0f, 0.0f, 0.0f, 1.0f);
	pyramid->addVertex(0.0f, 0.0f, 0.0f, 1.0f);
	pyramid->addVertex(0.0f, 1.0f, 0.0f, 1.0f);
	pyramid->addVertex(1.0f, 1.0f, 0.0f, 1.0f);
	pyramid->addVertex(0.5f, 0.5f, 1.6f, 1.0f);

	pyramid->addNormal(0.490171f, -0.490171f, -0.720739f);
	pyramid->addNormal(-0.677492f, -0.677492f, -0.28637f);
	pyramid->addNormal(-0.490171f, 0.490171f, -0.720739f);
	pyramid->addNormal(0.677492f, 0.677492f, -0.28637f);
	pyramid->addNormal(0.0f, 0.0f, 1.0f);

	pyramid->addTexCoord(0.0f, 0.0f);
	pyramid->addTexCoord(0.0f, 0.0f);
	pyramid->addTexCoord(0.5f, 0.0f);
	pyramid->addTexCoord(0.25f, 0.0f);
	pyramid->addTexCoord(0.40773f, 0.403589f);

	pyramid->addElement(0);
	pyramid->addElement(1);
	pyramid->addElement(2);
	pyramid->addElement(0);
	pyramid->addElement(2);
	pyramid->addElement(3);
	pyramid->addElement(1);
	pyramid->addElement(0);
	pyramid->addElement(4);
	pyramid->addElement(1);
	pyramid->addElement(4);
	pyramid->addElement(2);
	pyramid->addElement(2);
	pyramid->addElement(4);
	pyramid->addElement(3);
	pyramid->addElement(0);
	pyramid->addElement(3);
	pyramid->addElement(4);

	pyramid->setName("Pyramid");

	if(!pyramid->init(m_program))
	{
		std::cerr << "Could not initilize pyramid" << std::endl;
	}

	std::shared_ptr<Transform> trans(new Transform());
	trans->scale(glm::vec3(1,1,1));
	trans->rotate(-(3.14159265359f)/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	trans->translate(glm::vec3(-10, 0, 0));
	trans->addChild(pyramid);

	return trans;
}

std::shared_ptr<Transform> Application::buildFloor()
{
	std::shared_ptr<State> geoState = std::shared_ptr<State>(new State());
	std::shared_ptr<Geometry> ground(new Geometry(geoState));

	BoundingBox box = m_rootNode->calculateBoundingBox();
	glm::vec3 size = glm::vec3(25,25,0);

	ground->addVertex(size.x, 0, size.y, 1.0);
	ground->addVertex(size.x, 0, -size.y, 1.0);
	ground->addVertex(-size.x, 0, size.y, 1.0);
	ground->addVertex(-size.x, 0, size.y, 1.0);
	ground->addVertex(size.x, 0, -size.y, 1.0);
	ground->addVertex(-size.x, 0, -size.y, 1.0);

  	for (unsigned int k = 0; k < 6; k++)
	{
		ground->addNormal(0.0, 1.0, 0.0);
	}

	std::shared_ptr<Transform> groundTransform = std::shared_ptr<Transform>(new Transform);
	groundTransform->addChild(ground);
	groundTransform->setInitialTransform(glm::translate(glm::mat4(), glm::vec3(box.getCenter().x, box.min().y, box.getCenter().z)));

	ground->setName("ground");

	if(!ground->init(m_program))
	{
		std::cerr << "Could not initilize ground" << std::endl;
	}

	return groundTransform;
}

std::shared_ptr<Transform> Application::buildSphere()
{
	std::shared_ptr<Obj> obj = loadObj("models/sphere_large.obj");
	std::shared_ptr<Transform> transform = parseObj(obj);

	std::shared_ptr<State> state = std::shared_ptr<State>(new State(m_toonProgram));

	transform->setState(state);

	transform->scale(glm::vec3(1,1,1));
	transform->translate(glm::vec3(-24, 0, 0));

	return transform;
}

std::shared_ptr<Transform> Application::buildTree()
{
	std::shared_ptr<Obj> obj = loadObj("models/Tree/Tree.obj");
	std::shared_ptr<Transform> transform = parseObj(obj);
	transform->scale(glm::vec3(1,1,1));
	return transform;
}

void Application::buildGeometry(std::string model_filename)
{
	m_rootNode->addChild(buildCube());

	std::shared_ptr<Transform> parentTransform(new Transform());
	std::shared_ptr<State> parentState(new State());
	parentTransform->setState(parentState);

	//Set Light on parent transform. This will demonstrate merging of light sources (this will only affect objects under this subtree)
	std::shared_ptr<Light> light2 = std::shared_ptr<Light>(new Light);
	light2->setDiffuse(glm::vec4(1, 0, 0, 1));
	light2->setSpecular(glm::vec4(1, 1, 1, 1));
	light2->setPosition(glm::vec4(4.0, -2.0, 2.0, 0.0));
	parentTransform->getState()->add(light2);

	std::shared_ptr<Transform> pyramidTransform = buildPyramid();
	std::shared_ptr<State> pyramidState(new State());
	std::shared_ptr<Texture> pyramidTexture(new Texture());
	pyramidTexture->create("models/House01/House01_Textures/00_MyCust_1.jpg", 0);
	pyramidState->setTexture(pyramidTexture, 0);
	pyramidTransform->setState(pyramidState);

	parentTransform->addChild(pyramidTransform);
	parentTransform->addChild(buildSuzanne(model_filename));
	m_rootNode->addChild(parentTransform);


	m_rootNode->addChild(buildSphere());

	std::shared_ptr<Transform> cowTransform = buildCow();
	m_rootNode->addChild(cowTransform);

	std::shared_ptr<Transform> instansiationDemoTransform(new Transform());
	instansiationDemoTransform->scale(glm::vec3(1, 1, 1));
	instansiationDemoTransform->translate(glm::vec3(-15, 0, 0));

	for(auto child : pyramidTransform->getChildren())
	{
		instansiationDemoTransform->addChild(child);
	}

	m_rootNode->addChild(instansiationDemoTransform);

	std::shared_ptr<Transform> floorTransform = buildFloor();
	m_rootNode->addChild(floorTransform);

	std::shared_ptr<SortedGroup> forest = std::shared_ptr<SortedGroup>(new SortedGroup(m_fpsCamera));
	std::shared_ptr<State> forestState(new State());
	forestState->enableAlphaBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	forest->setState(forestState);

	for(int i = 0; i < 5; i++)
	{
		std::shared_ptr<Transform> tree = buildTree();
		tree->translate(glm::vec3((i * 8) - 25, -1, -10));
		forest->addChild(tree);
	}

	m_rootNode->addUpdateCallback(forest);
	m_rootNode->addChild(forest);
	m_rootNode->addChild(buildQuad());
}