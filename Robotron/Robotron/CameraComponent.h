#pragma once

#include <glm\glm.hpp>

struct CameraComponent {
	glm::mat4 lookAt;
	glm::vec3 position;
};