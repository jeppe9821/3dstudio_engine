#include "RotateCallback.h"
#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

RotateCallback::RotateCallback()
{
}

void RotateCallback::update(Node &n)
{
	Transform &t = dynamic_cast<Transform&>(n);
	t.rotate(0.01f, glm::vec3(0, 1, 0));
}