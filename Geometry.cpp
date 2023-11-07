#include <iostream>
#include <sstream>

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vr/glErrorUtil.h>

#include "NodeVisitor.h"
#include "Geometry.h"


Geometry::Geometry(std::shared_ptr<State> state, bool useVAO) : Node(state), m_vbo_vertices(0), m_vbo_normals(0), m_vbo_texCoords(0), m_ibo_elements(0),
                           m_attribute_v_coord(-1), m_attribute_v_normal(-1), m_attribute_v_texCoords(-1), m_vao(-1), m_useVAO(useVAO), m_hasInitilizedShaders(false), m_hasUploaded(false)
{
}

Geometry::Geometry(bool useVAO) : Node(), m_vbo_vertices(0), m_vbo_normals(0), m_vbo_texCoords(0), m_ibo_elements(0),
                           m_attribute_v_coord(-1), m_attribute_v_normal(-1), m_attribute_v_texCoords(-1), m_vao(-1), m_useVAO(useVAO), m_hasInitilizedShaders(false), m_hasUploaded(false)
{
}

Geometry::~Geometry()
{
	if (m_vbo_vertices != 0)
	{
		glDeleteBuffers(1, &m_vbo_vertices);
	}

	if (m_vbo_normals != 0)
	{
		glDeleteBuffers(1, &m_vbo_normals);
	}

	if (m_ibo_elements != 0)
	{
		glDeleteBuffers(1, &m_ibo_elements);
	}
}

bool Geometry::init(GLint program)
{
	bool flag = initShaders(program);
	upload();
	return flag;
}

void Geometry::apply(glm::mat4 obj2World)
{
	glUniformMatrix4fv(m_uniform_m, 1, GL_FALSE, glm::value_ptr(obj2World));
	glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(obj2World)));
	glUniformMatrix3fv(m_uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
}

void Geometry::accept(NodeVisitor &v)
{
	v.visit(*this);
}

BoundingBox Geometry::calculateBoundingBox()
{
	BoundingBox box;
	for (auto v : m_vertices)
	{
		box.expand(v * glm::mat4(1));
	}
	return box;
}

bool Geometry::isInitilized()
{
	return m_hasUploaded;
}

void Geometry::addVertex(float x, float y, float z, float w)
{
	m_vertices.push_back(glm::vec4(x,y,z,w));
}

void Geometry::addNormal(float x, float y, float z)
{
	m_normals.push_back(glm::vec3(x,y,z));
}

void Geometry::addTexCoord(float s, float t)
{
	m_texCoords.push_back(glm::vec2(s,t));
}

void Geometry::addElement(GLushort element1)
{
	m_elements.push_back(element1);
}

void Geometry::setVertices(std::vector<glm::vec4> vertices)
{
	this->m_vertices = vertices;
}

void Geometry::setNormals(std::vector<glm::vec3> normals)
{
	this->m_normals = normals;
}

void Geometry::setTexCoords(std::vector<glm::vec2> texCoords)
{
	this->m_texCoords = texCoords;
}

void Geometry::setElements(std::vector<GLushort> elements)
{
	this->m_elements = elements;
}

void Geometry::render()
{
	if(!isEnabled())
	{
		return;
	}

	if (m_useVAO)
	{
		glBindVertexArray(m_vao);
	}

	if (m_normals.size() == 0)
	{
		draw_bbox();
		return;
	}

	if(!m_hasInitilizedShaders)
	{
		std::cerr << "Geometry " << this->getName() << " has not initilized shaders. Cannot render" << std::endl;
		return;
	}

	if(!m_hasUploaded)
	{
		std::cerr << "Geometry has not been uploaded. Cannot render" << std::endl;
		return;
	}

	if (!m_useVAO)
	{
		if (this->m_vbo_vertices != 0)
		{
			glEnableVertexAttribArray(m_attribute_v_coord);
			glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
			glVertexAttribPointer(m_attribute_v_coord,4,GL_FLOAT,GL_FALSE,0,0);
		}

		if (this->m_vbo_normals != 0)
		{
			glEnableVertexAttribArray(m_attribute_v_normal);
			glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
			glVertexAttribPointer(m_attribute_v_normal,3,GL_FLOAT,GL_FALSE,0,0);
		}
		if (this->m_vbo_texCoords != 0)
		{
			glEnableVertexAttribArray(m_attribute_v_texCoords);
			glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
			glVertexAttribPointer(m_attribute_v_texCoords,2,GL_FLOAT,GL_FALSE,0,0);
		}
	}
	else
	{
		glEnableVertexAttribArray(m_attribute_v_coord);
		//CHECK_GL_ERROR_LINE_FILE();
		glEnableVertexAttribArray(m_attribute_v_normal);
		//CHECK_GL_ERROR_LINE_FILE();

		if (m_vbo_texCoords != 0)
		{
			glEnableVertexAttribArray(m_attribute_v_texCoords);
		}

		//CHECK_GL_ERROR_LINE_FILE();
	}

	//CHECK_GL_ERROR_LINE_FILE();

	/* Push each element in buffer_vertices to the vertex shader */
	if (this->m_ibo_elements != 0)
	{
		if (!m_useVAO)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
		}

		GLuint size = GLuint(this->m_elements.size());
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_SHORT, 0);
		//CHECK_GL_ERROR_LINE_FILE();
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->m_vertices.size());
	}

	if (this->m_vbo_normals != 0)
	{
		glDisableVertexAttribArray(m_attribute_v_normal);
	}

	if (this->m_vbo_vertices != 0)
	{
		glDisableVertexAttribArray(m_attribute_v_coord);
	}

	if (this->m_vbo_texCoords != 0)
	{
		glDisableVertexAttribArray(m_attribute_v_texCoords);
	}

	if (m_useVAO)
	{
		glBindVertexArray(0);
	}

	glDisable(GL_BLEND);
}

void Geometry::draw_bbox()
{
	if (this->m_vertices.size() == 0)
	{
		return;
	}

	// Cube 1x1x1, centered on origin
	GLfloat vertices[] =
	{
		-0.5, -0.5, -0.5, 1.0,
		0.5, -0.5, -0.5, 1.0,
		0.5, 0.5, -0.5, 1.0,
		-0.5, 0.5, -0.5, 1.0,
		-0.5, -0.5, 0.5, 1.0,
		0.5, -0.5, 0.5, 1.0,
		0.5, 0.5, 0.5, 1.0,
		-0.5, 0.5, 0.5, 1.0,
	};

	GLuint vbo_vertices;
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLushort elements[] =
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	GLuint ibo_elements;
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLfloat min_x;
	GLfloat max_x;
	GLfloat min_y;
	GLfloat max_y;
	GLfloat min_z;
	GLfloat max_z;

	min_x = max_x = this->m_vertices[0].x;
	min_y = max_y = this->m_vertices[0].y;
	min_z = max_z = this->m_vertices[0].z;

	for (unsigned int i = 0; i < this->m_vertices.size(); i++)
	{
		if (this->m_vertices[i].x < min_x) min_x = this->m_vertices[i].x;
		if (this->m_vertices[i].x > max_x) max_x = this->m_vertices[i].x;
		if (this->m_vertices[i].y < min_y) min_y = this->m_vertices[i].y;
		if (this->m_vertices[i].y > max_y) max_y = this->m_vertices[i].y;
		if (this->m_vertices[i].z < min_z) min_z = this->m_vertices[i].z;
		if (this->m_vertices[i].z > max_z) max_z = this->m_vertices[i].z;
	}

	glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
	glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	glm::mat4 transform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);

	/* Apply object's transformation matrix */
	glm::mat4 m = glm::mat4(1) * transform;
	glUniformMatrix4fv(m_uniform_m, 1, GL_FALSE, glm::value_ptr(m));

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glEnableVertexAttribArray(m_attribute_v_coord);
	glVertexAttribPointer(m_attribute_v_coord,4,GL_FLOAT,GL_FALSE,0,0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(m_attribute_v_coord);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &ibo_elements);
}

bool Geometry::initShaders(GLint program)
{
	m_hasInitilizedShaders = true;

	const char* attribute_name;
	attribute_name = "vertex.position";
	m_attribute_v_coord = glGetAttribLocation(program, attribute_name);

	if (m_attribute_v_coord == -1)
	{
		//fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		//return false;
	}

	attribute_name = "vertex.normal";
	m_attribute_v_normal = glGetAttribLocation(program, attribute_name);

	if (m_attribute_v_normal == -1)
	{
		//fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		//return false;
	}

	attribute_name = "vertex.texCoord";
	m_attribute_v_texCoords = glGetAttribLocation(program, attribute_name);

	if (m_attribute_v_texCoords == -1)
	{
		//fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		//return false;
	}

	const char* uniform_name;
	uniform_name = "m";
	m_uniform_m = glGetUniformLocation(program, uniform_name);

	if (m_uniform_m == -1)
	{
		//fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		//return 0;
	}

	uniform_name = "m_3x3_inv_transp";
	m_uniform_m_3x3_inv_transp = glGetUniformLocation(program, uniform_name);

	if (m_uniform_m_3x3_inv_transp == -1)
	{
		//fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		//return false;
	}

	return true;
}

void Geometry::upload()
{
	m_hasUploaded = true;

	if (m_useVAO)
	{
		// Create a Vertex Array Object that will handle all VBO:s of this Geometry
		glGenVertexArrays(1, &m_vao);
		//CHECK_GL_ERROR_LINE_FILE();
		glBindVertexArray(m_vao);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	if (this->m_vertices.size() > 0)
	{
		glGenBuffers(1, &this->m_vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, this->m_vertices.size() * sizeof(this->m_vertices[0]),
			this->m_vertices.data(), GL_STATIC_DRAW);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	if (this->m_normals.size() > 0)
	{
		glGenBuffers(1, &this->m_vbo_normals);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_normals);
		glBufferData(GL_ARRAY_BUFFER, this->m_normals.size() * sizeof(this->m_normals[0]),this->m_normals.data(), GL_STATIC_DRAW);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	if (this->m_texCoords.size() > 0)
	{
		glGenBuffers(1, &this->m_vbo_texCoords);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_vbo_texCoords);
		glBufferData(GL_ARRAY_BUFFER, this->m_texCoords.size() * sizeof(this->m_texCoords[0]),this->m_texCoords.data(), GL_STATIC_DRAW);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	if (m_useVAO)
	{
		//Vertices
		glEnableVertexAttribArray(m_attribute_v_coord);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
		glVertexAttribPointer(m_attribute_v_coord,4,GL_FLOAT,GL_FALSE,0,0);
		glDisableVertexAttribArray(m_attribute_v_coord);

		//normals
		glEnableVertexAttribArray(m_attribute_v_normal);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
		glVertexAttribPointer(m_attribute_v_normal,3,GL_FLOAT,GL_FALSE,0,0);
		glDisableVertexAttribArray(m_attribute_v_normal);

		//texCoords
		glEnableVertexAttribArray(m_attribute_v_texCoords);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_texCoords);
		glVertexAttribPointer(m_attribute_v_texCoords,2,GL_FLOAT,GL_FALSE,0,0);
		glDisableVertexAttribArray(m_attribute_v_texCoords);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	if (this->m_elements.size() > 0)
	{
		glGenBuffers(1, &this->m_ibo_elements);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_ibo_elements);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_elements.size() * sizeof(this->m_elements[0]),this->m_elements.data(), GL_STATIC_DRAW);
		//CHECK_GL_ERROR_LINE_FILE();
	}

	//CHECK_GL_ERROR_LINE_FILE();

	if (m_useVAO)
	{
		// Now release VAO
		glEnableVertexAttribArray(0);  // Disable our Vertex Array Object
		glBindVertexArray(0); // Disable our Vertex Buffer Object
		//CHECK_GL_ERROR_LINE_FILE();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}