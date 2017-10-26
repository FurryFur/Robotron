#include "GLMUtils.h"

#include "BufferStream.h"

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec2& vec)
{
	return obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec2::value_type));
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec3& vec)
{
	return obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec3::value_type));
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec4& vec)
{
	return obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec4::value_type));
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat3& mat)
{
	return obs.write(glm::value_ptr(mat), sizeof(mat) / sizeof(glm::mat3::value_type));
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat4& mat)
{
	return obs.write(glm::value_ptr(mat), sizeof(mat) / sizeof(glm::mat4::value_type));
}

InBufferStream& operator>>(InBufferStream& ibs, glm::vec2 & outVec)
{
	return ibs.read(glm::value_ptr(outVec), sizeof(outVec) / sizeof(glm::vec2::value_type));
}

InBufferStream& operator>>(InBufferStream& ibs, glm::vec3 & outVec)
{
	return ibs.read(glm::value_ptr(outVec), sizeof(outVec) / sizeof(glm::vec3::value_type));
}

InBufferStream& operator>>(InBufferStream& ibs, glm::vec4 & outVec)
{
	return ibs.read(glm::value_ptr(outVec), sizeof(outVec) / sizeof(glm::vec4::value_type));
}

InBufferStream& operator>>(InBufferStream& ibs, glm::mat3 & outMat)
{
	return ibs.read(glm::value_ptr(outMat), sizeof(outMat) / sizeof(glm::mat3::value_type));
}

InBufferStream& operator>>(InBufferStream& ibs, glm::mat4 & outMat)
{
	return ibs.read(glm::value_ptr(outMat), sizeof(outMat) / sizeof(glm::mat4::value_type));
}
