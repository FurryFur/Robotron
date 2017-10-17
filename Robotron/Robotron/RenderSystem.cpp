//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A system which handles rendering each entity.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#include "RenderSystem.h"

#include "GLUtils.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "Scene.h"
#include "Entity.h"
#include "UniformFormat.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using glm::mat4;
using glm::vec3;

RenderSystem::RenderSystem(GLFWwindow* glContext, const Scene& scene)
	: m_glContext{ glContext }
	, m_scene{ scene }
	, m_uniformBindingPoint{ 0 }
	, m_shaderParamsBindingPoint{ 1 }
{
	// Create buffer for camera parameters
	glGenBuffers(1, &m_uboUniforms);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBindingPoint, m_uboUniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformFormat), nullptr, GL_DYNAMIC_DRAW);

	// Create buffer for shader parameters
	glGenBuffers(1, &m_uboShaderParams);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_shaderParamsBindingPoint, m_uboShaderParams);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderParams), nullptr, GL_DYNAMIC_DRAW);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void RenderSystem::beginRender()
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::endRender()
{
	glfwSwapBuffers(m_glContext);
}

void RenderSystem::update(Entity& entity)
{
	// Can't render anything without a camera set
	if (!m_cameraEntity) {
		return;
	}

	// Filter renderable entities
	const size_t kRenderableMask = COMPONENT_MESH | COMPONENT_MATERIAL;
	if ((entity.componentMask & kRenderableMask) != kRenderableMask)
		return;

	const mat4& cameraTransform = m_cameraEntity->transform;

	if (entity.material.enableDepth) {
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
	}
	else {
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
	}

	// Tell the gpu what material to use
	glUseProgram(entity.material.shader);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(entity.material.shader, "sampler"), 0);
	glBindTexture(entity.material.textureType, entity.material.texture);

	// Set environment map to use on GPU
	if (m_isEnvironmentMap) {
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(entity.material.shader, "environmentSampler"), 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	}

	// Send shader parameters to gpu
	GLuint blockIndex;
	blockIndex = glGetUniformBlockIndex(entity.material.shader, "ShaderParams");
	glUniformBlockBinding(entity.material.shader, blockIndex, m_shaderParamsBindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_shaderParamsBindingPoint, m_uboShaderParams);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderParams), &entity.material.shaderParams);
		
	// Get Aspect ratio
	int width, height;
	glfwGetFramebufferSize(m_glContext, &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Get model, view and projection matrices
	UniformFormat uniforms;
	bool hasTransform = (entity.componentMask & COMPONENT_TRANSFORM) ==  COMPONENT_TRANSFORM;
	uniforms.model = hasTransform ? entity.transform : glm::mat4{ 1 };
	uniforms.view = glm::inverse(cameraTransform);
	uniforms.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.5f, 100.0f);
	uniforms.cameraPos = cameraTransform[3];

	// Send the model view and projection matrices to the gpu
	blockIndex = glGetUniformBlockIndex(entity.material.shader, "Uniforms");
	glUniformBlockBinding(entity.material.shader, blockIndex, m_uniformBindingPoint);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBindingPoint, m_uboUniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformFormat), &uniforms);

	// Draw object
	glBindVertexArray(entity.mesh.VAO);
	glDrawElements(GL_TRIANGLES, entity.mesh.numIndices, GL_UNSIGNED_INT, 0);
}

void RenderSystem::setCamera(const Entity* entity)
{
	m_cameraEntity = entity;
}

void RenderSystem::setEnvironmentMap(const Entity& entity)
{
	// TODO: Error if not a cube map
	m_environmentMap = entity.material.texture;
	m_isEnvironmentMap = true;
}
