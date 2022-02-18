#pragma once

#include "Group.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

class Transform : public Group
{
    public:
        Transform();
        Transform(std::shared_ptr<State> state);
        virtual ~Transform() override;

        virtual void accept(NodeVisitor& v) override;
        virtual BoundingBox calculateBoundingBox() override;

        void translate(glm::vec3 translation);
        void rotate(float rad, glm::vec3 axis);
        void scale(glm::vec3 scaling);

        void setInitialTransform(const glm::mat4& m);
        void resetTransform();
        glm::mat4 getModelMatrix();

        glm::vec3 getTranslation();
    private:
        glm::vec3 m_translation;
        glm::mat4 m_initialTransform;
        glm::mat4 m_object2world;
};