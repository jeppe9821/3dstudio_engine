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

Application::Application(unsigned int width, unsigned height)
	: m_screenSize(width, height)
{
	m_camera = std::shared_ptr<Camera>(new Camera);
  	m_fpsCounter = std::make_shared<FPSCounter>();
}

bool Application::initResources(const std::string& model_filename, const std::string& vshader_filename, std::string& fshader_filename)
{
	m_loadedVShader = vshader_filename;
	m_loadedFShader = fshader_filename;
	m_loadedFilename = model_filename;

	m_program = glCreateProgram();
	m_toonProgram = glCreateProgram();

	m_renderVisitor = std::shared_ptr<RenderVisitor>(new RenderVisitor());
	m_updateVisitor = std::shared_ptr<UpdateVisitor>(new UpdateVisitor());

	if (!initShaders(&m_program, vshader_filename, fshader_filename))
	{
		return false;
	}

	if (!initShaders(&m_toonProgram, "shaders/toon-shading.vert.glsl", "shaders/toon-shading.frag.glsl"))
	{
		return false;
	}

	m_camera->init(m_program);
	m_camera->setScreenSize(m_screenSize);

	std::string ext = vr::FileSystem::getFileExtension(model_filename);
	std::shared_ptr<Obj> obj;

	std::shared_ptr<State> defaultState = std::shared_ptr<State>(new State());
	std::shared_ptr<State> defaultToonState = std::shared_ptr<State>(new State());

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
			std::shared_ptr<Transform> n = parseObj(obj, m_program);
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

	std::shared_ptr<Transform> lightTransform = parseObj(loadObj("models/Sun/13913_Sun_v2_l3.obj"), m_program);
	lightTransform->setInitialTransform(glm::mat4(1));
	lightTransform->scale(glm::vec3(0.1f, 0.1f, 0.1f));
	m_rootNode->addChild(lightTransform);

	m_lightMoveCallback = std::shared_ptr<LightMoveCallback>(new LightMoveCallback(light1, lightTransform));

	m_rootNode->getState()->add(light1);
	m_rootNode->addUpdateCallback(m_lightMoveCallback);

	return 1;
}

void Application::initView()
{
	BoundingBox box = m_rootNode->calculateBoundingBox();
	float radius = box.getRadius();

	float distance = radius * 1.5f;
	glm::vec3 eye = glm::vec3(0, radius, distance);
	glm::vec3 direction = glm::normalize(box.getCenter() - eye);

	std::shared_ptr<Light> light = m_rootNode->getState()->getLights().front();
	glm::vec4 position;
	position = glm::vec4(eye + glm::vec3(3, 2, 0), 1);
	light->setPosition(position);

	m_camera->set(eye, direction, glm::vec3(0.0, 1.0, 0.0));
	m_camera->setNearFar(glm::vec2(0.1, distance * 20));
	m_camera->setSceneScale(0.01f * radius);
	m_camera->setFov(90);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
}

void Application::update(GLFWwindow* window)
{
	//Animate the multi textured object

	std::vector<GLuint> programs;
	programs.push_back(m_program);
	programs.push_back(m_toonProgram);

	glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < 2; i++)
	{
		glUseProgram(programs[i]);

		glUniform1f(glGetUniformLocation(m_program, "sinTime"), glm::sin(glfwGetTime()));

		m_camera->init(programs[i]);
		m_camera->apply(programs[i]);

		glUseProgram(0);

		m_updateVisitor->visit(*m_rootNode);

		//render

		//We have to traverse the tree twice as an optimization in order to only run the objects with the program we want to render. However,
		//this has a fatal flaw, because this will make it so that two objects of different shader programs will not be able to be related to each other
		//on a transform level, unless you add the transform to both roots. The only solution to this is to ensure that the camera extends "Node".
		//This would make it easier to initilize and apply the camera when the program is changed in the middle of a tree. But atleast, this works and it's sortof optimized
		if(i == 0)
		{
			m_renderVisitor->resetState(m_program);
			m_renderVisitor->visit(*m_rootNode);
		}
		else if(i == 1)
		{
			m_renderVisitor->resetState(m_toonProgram);
			m_renderVisitor->visit(*m_rootNode);
		}
	}
	m_fpsCounter->render(window);
}

void Application::processInput(GLFWwindow* window)
{
	m_camera->processInput(window);
	m_lightMoveCallback->processInput(window);
}

void Application::reloadScene()
{
	initResources(m_loadedFilename, m_loadedVShader, m_loadedFShader);
	initView();
}

void Application::setScreenSize(unsigned int width, unsigned int height)
{
	m_camera->setScreenSize(glm::uvec2(width, height));
	glViewport(0, 0, width, height);
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

std::shared_ptr<Transform> Application::parseObj(std::shared_ptr<Obj> obj, GLuint program)
{
	std::shared_ptr<Transform> n = std::shared_ptr<Transform>(new Transform());
	n->setName("Transform_" + obj->name);
	n->setInitialTransform(obj->initialTransform);

	for(int i = 0; i < obj->meshes.size(); i++)
	{
		std::shared_ptr<State> geometryState = std::shared_ptr<State>(new State(program));
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

		if(!g->init(program))
		{
			std::cout << "Could not init " << g->getName() << std::endl;
		}

		std::shared_ptr<Transform> transform = std::shared_ptr<Transform>(new Transform());
		transform->setInitialTransform(obj->meshes[i].object2world);
		transform->setName("Transform2_" + obj->name);

		transform->addChild(g);
		n->addChild(transform);
	}

	return n;
}

std::shared_ptr<Transform> Application::buildSuzanne(std::string model_filename)
{
	std::shared_ptr<Obj> obj = loadObj(model_filename);

	if (!obj)
	{
		return nullptr;
	}

	std::shared_ptr<Transform> transform = parseObj(obj, m_program);
	transform->setState(std::shared_ptr<State>(new State()));
	transform->scale(glm::vec3(100, 100, 100));
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

		std::shared_ptr<Transform> transform = parseObj(obj, m_program);
		transform->setState(std::shared_ptr<State>(new State()));
		transform->getState()->setPolygonMode(GL_LINE);
		lod->addChild(transform);
	}

	lodModels.clear();

	m_rootNode->addUpdateCallback(lod);

	lod->translate(glm::vec3(500, 0, 0));

	lod->setName("LOD");

	return lod;
}

std::shared_ptr<Transform> Application::buildCube()
{
	std::shared_ptr<Obj> obj = loadObj("models/models/cube_brick.obj");
	std::shared_ptr<Transform> transform = parseObj(obj, m_program);

	std::shared_ptr<State> state = std::shared_ptr<State>(new State());

	std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(new Texture());
	texture->create("textures/pexels-anni-roenkae-2832432.jpg", 1);
	state->setTexture(texture, 1);

	transform->setState(state);

	transform->scale(glm::vec3(100,100,100));
	transform->translate(glm::vec3(-5, 0, 0));

	return transform;
}

std::shared_ptr<Transform> Application::buildPyramid()
{
	std::shared_ptr<State> geoState = std::shared_ptr<State>(new State(m_program));
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
	trans->scale(glm::vec3(100,100,100));
	trans->rotate(-(3.14159265359f)/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	trans->translate(glm::vec3(-10, 0, 0));
	trans->addChild(pyramid);

	return trans;
}

std::shared_ptr<Transform> Application::buildFloor()
{
	std::shared_ptr<State> geoState = std::shared_ptr<State>(new State(m_program));
	std::shared_ptr<Geometry> ground(new Geometry(geoState));

	BoundingBox box = m_rootNode->calculateBoundingBox();
	glm::vec3 size = glm::vec3(3000,1500,0);

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
	std::shared_ptr<Transform> transform = parseObj(obj, m_toonProgram);

	std::shared_ptr<State> state = std::shared_ptr<State>(new State(m_toonProgram));
	//std::shared_ptr<Texture> texture = std::shared_ptr<Texture>(new Texture());
	//texture->create("textures/pexels-anni-roenkae-2832432.jpg", 1);
	//state->setTexture(texture, 1);

	transform->setState(state);

	transform->scale(glm::vec3(100,100,100));
	transform->translate(glm::vec3(-24, 0, 0));

	return transform;
}

std::shared_ptr<Transform> Application::buildTree()
{
	std::shared_ptr<Obj> obj = loadObj("models/Tree/Tree.obj");
	std::shared_ptr<Transform> transform = parseObj(obj, m_program);
	transform->scale(glm::vec3(100,100,100));
	return transform;
}

void Application::buildGeometry(std::string model_filename)
{
	m_rootNode->addChild(buildCube());
	m_rootNode->addChild(buildSphere());

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
	parentTransform->addChild(pyramidTransform);
	parentTransform->addChild(buildSuzanne(model_filename));
	m_rootNode->addChild(parentTransform);

	std::shared_ptr<Transform> cowTransform = buildCow();
	m_rootNode->addChild(cowTransform);

	std::shared_ptr<Transform> instansiationDemoTransform(new Transform());
	instansiationDemoTransform->scale(glm::vec3(100, 100, 100));
	instansiationDemoTransform->translate(glm::vec3(-15, 0, 0));

	for(auto child : pyramidTransform->getChildren())
	{
		instansiationDemoTransform->addChild(child);
	}

	m_rootNode->addChild(instansiationDemoTransform);

	std::shared_ptr<SortedGroup> forest = std::shared_ptr<SortedGroup>(new SortedGroup(m_camera));
	std::shared_ptr<State> forestState(new State());
	forestState->enableAlphaBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	forest->setState(forestState);

	for(int i = 0; i < 10; i++)
	{
		std::shared_ptr<Transform> tree = buildTree();
		tree->setName("tree" + std::to_string(i));
		tree->translate(glm::vec3(i * 8, 0, -10));
		forest->addChild(tree);
	}
	std::shared_ptr<Transform> floorTransform = buildFloor();
	m_rootNode->addChild(floorTransform);

	m_rootNode->addUpdateCallback(forest);
	m_rootNode->addChild(forest);
}