#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "BoundingBox.h"
#include "Node.h"
#include "State.h"

class Scene;

/// <summary>
/// The geometry class, handles all the rendering for a single geometry.
/// </summary>
class Geometry : public Node
{
	public:
		/// <summary>
		/// The constructor for states, given by Node
		/// </summary>
		Geometry(std::shared_ptr<State> state, bool useVAO = true);

		/// <summary>
		/// The constructor for  VAO
		/// </summary>
		Geometry(bool useVAO = true);

		/// <summary>
		/// The destructor, given by Node
		/// </summary>
		virtual ~Geometry() override;

		/// <summary>
		/// Calculates the bounding box, given by Node
		/// </summary>
		/// <returns> The calculated bounding box </returns>
		virtual BoundingBox calculateBoundingBox() override;

		/// <summary>
		/// The accept function, given by Node
		/// </summary>
		/// <param name="v">The nodevisitor</param>		
		virtual void accept(NodeVisitor &v) override;

		/// <summary>
		/// Initilizes the geometry
		/// </summary>
		/// <param name="program">The shader program</param>
		/// <returns> A flag if the geometry initilized correctly</returns>		
		bool init(GLint program);

		/// <summary>
		/// Applies the uniforms for the geometry
		/// </summary>
		/// <param name="obj2world">The world model matrix</param>
		void apply(glm::mat4 obj2world);

		/// <summary>
		/// Renders the geometry
		/// </summary>
		void render();

		/// <summary>
		/// Draws a bounding box around the object
		/// </summary>
		void draw_bbox();

		/// <summary>
		/// Checks if the geometry is initilized
		/// </summary>
		/// <returns> A flag if the geometry is initilized or not </returns>		
		bool isInitilized();

		/// <summary>
		/// Appends a vertex
		/// </summary>
		/// <param name="x">The x coordinate of the vertex</param>
		/// <param name="y">The y coordinate of the vertex</param>
		/// <param name="z">The z coordinate of the vertex</param>
		/// <param name="w">The w coordinate of the vertex</param>
		void addVertex(float x, float y, float z, float w);

		/// <summary>
		/// Appends a normal
		/// </summary>
		/// <param name="x">The x coordinate of the normal</param>
		/// <param name="y">The y coordinate of the normal</param>
		/// <param name="z">The z coordinate of the normal</param>
		void addNormal(float x, float y, float z);

		/// <summary>
		/// Appends a texture coordinate
		/// </summary>
		/// <param name="s">The s coordinate of the texture</param>
		/// <param name="t">The t coordinate of the texture</param>
		void addTexCoord(float s, float t);

		/// <summary>
		/// Appends an element
		/// </summary>
		/// <param name="element1">The element</param>
		void addElement(GLushort element1);

		/// <summary>
		/// Sets the vertices for the geometry
		/// </summary>
		/// <param name="vertices">The vertices for the geometry</param>
		void setVertices(std::vector<glm::vec4> vertices);

		/// <summary>
		/// Sets the normals for the geometry
		/// </summary>
		/// <param name="normals">The normals for the geometry</param>
		void setNormals(std::vector<glm::vec3> normals);

		/// <summary>
		/// Sets the texture coordinates for the geometry
		/// </summary>
		/// <param name="texCoords">The texture coordinates for the geometry</param>
		void setTexCoords(std::vector<glm::vec2> texCoords);

		/// <summary>
		/// Sets the elements for the geometry
		/// </summary>
		/// <param name="elements">The elements for the geometry</param>
		void setElements(std::vector<GLushort> elements);

	private:
		std::vector<glm::vec4> m_vertices;
		std::vector<glm::vec3> m_normals;
		std::vector<glm::vec2> m_texCoords;
		std::vector<GLushort> m_elements;

		GLuint m_vbo_vertices;
		GLuint m_vbo_normals;
		GLuint m_vbo_texCoords;
		GLuint m_ibo_elements;
		GLuint m_vao;

		GLint m_attribute_v_coord;
		GLint m_attribute_v_normal;
		GLint m_attribute_v_texCoords;

		GLint m_uniform_m;
		GLint m_uniform_m_3x3_inv_transp;

		bool m_useVAO;
		bool m_hasInitilizedShaders;
		bool m_hasUploaded;

		/// <summary>
		/// Initilizes the shaders with a given shader program
		/// </summary>
		/// <param name="program">The shader program</param>
		/// <returns> A flag if the shaders initilized correctly or not </returns>
		bool initShaders(GLint program);

		/// <summary>
		/// Uploads the geometry uniforms
		/// </summary>
		void upload();
};

