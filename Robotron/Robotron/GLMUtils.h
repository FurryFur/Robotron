//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A collection of utility functions that rely on the GLM libary
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

class OutBufferStream;
class InBufferStream;

namespace GLMUtils {
	// Limits a vector to the specified magnitude
	template <typename VecT>
	VecT limitVec(const VecT& vec, float maxMagnitude)
	{
		auto magnitude = glm::length(vec);
		if (magnitude > maxMagnitude) {
			return glm::normalize(vec) * maxMagnitude;
		}
		return vec;
	}
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec2& vec);
OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec3& vec);
OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec4& vec);
OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat3& mat);
OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat4& mat);

InBufferStream& operator>>(InBufferStream& ibs, glm::vec2& outVec);
InBufferStream& operator>>(InBufferStream& ibs, glm::vec3& outVec);
InBufferStream& operator>>(InBufferStream& ibs, glm::vec4& outVec);
InBufferStream& operator>>(InBufferStream& ibs, glm::mat3& outMat);
InBufferStream& operator>>(InBufferStream& ibs, glm::mat4& outMat);