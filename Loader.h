#pragma once

#include <memory>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
#include <string>

#include "Material.h"

struct Mesh
{
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<GLushort> elements;
	glm::mat4 object2world;
	std::shared_ptr<Material> material;
	std::shared_ptr<Texture> texture;
};

struct Obj
{
	std::string name;
	std::vector<Mesh> meshes;
	glm::mat4 initialTransform;
};

struct XmlScene
{
	std::vector<std::shared_ptr<Obj>> objects;
};

std::shared_ptr<Obj> loadObj(const std::string& filename);
bool loadXml(const std::string& xmlFile, std::shared_ptr<XmlScene>& scene);
