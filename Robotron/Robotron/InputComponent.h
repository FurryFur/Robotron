//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A component for receving input.
//                Input mappings are also set in this component.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

#include <array>

class OutBufferStream;
class InBufferStream;

struct InputComponent {
	glm::vec3 axis;
	glm::vec3 orientationDelta;
	bool btn1Down;
	bool btn2Down;
	bool btn3Down;
	bool btn4Down;

	// These represent buttons that are currently down
	// std::array<bool, 4> btnsDown;
	// These represent action buttons that have been pressed and have
	// not yet been handled. These should be set to false once the action
	// has been handled.
	// std::array<bool, 4> actionBtnsPressed;
};