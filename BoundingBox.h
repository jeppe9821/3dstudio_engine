#pragma once

/// <summary>
/// Simple class to store and handle a AABB (Axis aligned bounding box)
/// </summary>
class BoundingBox
{
	public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="min">Minimum size of box</param>
        /// <param name="max">Maximum size of box</param>
		BoundingBox(const glm::vec3& min, const glm::vec3&max)
		: m_max(max), m_min(min)
		{
		}

        /// <summary>
        /// Constructor
        /// </summary>
		BoundingBox()
		: m_max(-1E30f), m_min(1E30f)
		{
		}

        /// <summary>
        /// Multiplication operator for matrices
        /// </summary>
        /// <param name="mat">The matrix to multiply with</param>
		/// <returns>The new bounding box given by the multiplication</returns>
		BoundingBox operator*(const glm::mat4& mat) const
		{
			BoundingBox box;

			box.m_min = mat * glm::vec4(m_min, 1);
			box.m_max = mat * glm::vec4(m_max, 1);
			return box;
		}

		/// <summary>
        /// Expands the box given by a size v
        /// </summary>
        /// <param name="v">The size to expand of</param>
		void expand(const glm::vec3& v)
		{
			m_min = glm::min(m_min, v);
			m_max = glm::max(m_max, v);
		}

		/// <summary>
        /// Expands the box given by another box
        /// </summary>
        /// <param name="other">The box to expand by</param>
		void expand(const BoundingBox& other)
		{
			m_min = glm::min(m_min, other.min());
			m_max = glm::max(m_max, other.max());
		}

        /// <summary>
        /// Returns the center of the bounding box
        /// </summary>
		/// <returns>The center of the bounding box</returns>
		glm::vec3 getCenter()
		{
			return (m_max - m_min)*0.5f + m_min;
		}

		/// <summary>
        /// Returns the radius of the bounding box
        /// </summary>
		/// <returns>The radius</returns>
		float getRadius()
		{
			return glm::length((m_max - m_min))*0.5f;
		}

		/// <summary>
        /// Returns the min size of the boundingbox
        /// </summary>
		/// <returns>The minimum size of the boundingbox</returns>
		const glm::vec3& min() const
		{
			return m_min;
		}

		/// <summary>
        /// Returns the max size of the boundingbox
        /// </summary>
		/// <returns>The maximum size of the boundingbox</returns>
		const glm::vec3& max() const
		{
			return m_max;
		}

	private:
		glm::vec3 m_min, m_max, m_center;
};
