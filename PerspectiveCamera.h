#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera
{
    public:
        PerspectiveCamera();
        virtual void apply(GLuint program) override;
};