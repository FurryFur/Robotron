#include "GLMUtils.h"

#include "BufferStream.h"

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec2& vec)
{
	obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec2));
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec3& vec)
{
	obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec3));
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::vec4& vec)
{
	obs.write(glm::value_ptr(vec), sizeof(vec) / sizeof(glm::vec4));
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat3& mat)
{
	obs.write(glm::value_ptr(mat), sizeof(mat) / sizeof(glm::mat3));
	return obs;
}

OutBufferStream& operator<<(OutBufferStream& obs, const glm::mat4& mat)
{
	obs.write(glm::value_ptr(mat), sizeof(mat) / sizeof(glm::mat4));
	return obs;
}