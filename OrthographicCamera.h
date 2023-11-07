#pragma once

#include "Camera.h"

class OrthographicCamera : public Camera
{
    public:
        OrthographicCamera();

        virtual void apply(GLuint program) override;
        void setTop(float top);
    private:
        float m_top;
        float m_oblscale;
        float m_oblrad;
};