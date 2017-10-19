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
#include "Material.h"
#include "Mesh.h"
#include "Scene.h"
#include "Entity.h"
#include "UniformFormat.h"
#include "EntityUtils.h"
#include "ModelUtils.h"

#include <GLFW\glfw3.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

RenderState RenderSystem::s_renderState;

RenderSystem::RenderSystem(GLFWwindow* glContext, Scene& scene)
	: m_scene{ scene }
{
	m_renderState.glContext = glContext;
	m_renderState.uniformBindingPoint = 0;
	m_renderState.shaderParamsBindingPoint = 1;

	// Create buffer for camera parameters
	glGenBuffers(1, &m_renderState.uboUniforms);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_renderState.uniformBindingPoint, m_renderState.uboUniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformFormat), nullptr, GL_DYNAMIC_DRAW);

	// Create buffer for shader parameters
	glGenBuffers(1, &m_renderState.uboShaderParams);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_renderState.shaderParamsBindingPoint, m_renderState.uboShaderParams);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ShaderParams), nullptr, GL_DYNAMIC_DRAW);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& tip)
{
	vec3 direction = tip - base;
	drawDebugArrow(base, direction, glm::length(direction));
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& _direction, float magnitude)
{
	mat4 transform;
	vec3 direction = glm::normalize(_direction);
	transform[1] = vec4(direction, 1);
	// Find a perpendicular vector
	glm::vec3 tangent = vec3{ 1, 1, 1 };
	if (direction.x != 0)
		tangent.x = -(direction.y + direction.z) / direction.x;
	else if (direction.y != 0)
		tangent.y = -(direction.x + direction.z) / direction.y;
	else
		tangent.z = -(direction.y + direction.z) / direction.z;
	transform[0] = vec4(glm::normalize(glm::cross(direction, tangent)), 1);
	transform[2] = vec4(glm::normalize(glm::cross(vec3(transform[0]), direction)), 1);
	transform[3] = vec4(base, 1);

	transform *= glm::scale({}, vec3(1, magnitude, 1));

	ModelComponent& model = ModelUtils::loadModel("Assets/Models/red_arrow/red_arrow.obj");

	// Can't render anything without a camera set
	if (!s_renderState.cameraEntity) {
		return;
	}

	renderModel(model, transform);
}

void RenderSystem::beginRender()
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::endRender()
{
	glfwSwapBuffers(m_renderState.glContext);
}

void RenderSystem::update(const Entity& entity)
{
	// Filter renderable entities
	const size_t kRenderableMask = COMPONENT_MODEL;
	if ((entity.componentMask & kRenderableMask) != kRenderableMask)
		return;

	// Can't render anything without a camera set
	if (!m_renderState.cameraEntity) {
		return;
	}

	bool hasTransform = entity.hasAllComponents(COMPONENT_TRANSFORM);

	// Swap the current global render state with this RenderSystems state.
	s_renderState = m_renderState;

	// Render the 
	renderModel(entity.model, entity.transform);
}

void RenderSystem::setCamera(const Entity* entity)
{
	m_renderState.cameraEntity = entity;
}

void RenderSystem::setEnvironmentMap(const Entity& entity)
{
	// Take the first texture on the entity as the environment map
	// TODO: Do some error checking here to make sure this is actually
	// a cube map and we don't get index out of bounds.
	m_renderState.environmentMap = entity.model.materials.at(0).colorMaps.at(0).id;
	m_renderState.hasEnvironmentMap = true;
}

void RenderSystem::renderModel(const ModelComponent& model, const glm::mat4& transform)
{
	const mat4& cameraTransform = s_renderState.cameraEntity->transform;

	// Get Aspect ratio
	int width, height;
	GLFWwindow* glContext = glfwGetCurrentContext();
	glfwGetFramebufferSize(glContext, &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Get model, view and projection matrices
	UniformFormat uniforms;
	
	uniforms.model = transform;
	uniforms.view = glm::inverse(cameraTransform);
	uniforms.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.5f, 100.0f);
	uniforms.cameraPos = cameraTransform[3];

	// Loop over all the meshes in the model
	for (size_t i = 0; i < model.meshes.size(); ++i) {
		const Mesh& mesh = model.meshes.at(i);
		const Material& material = model.materials.at(mesh.materialIndex);

		// Tell the gpu what shader to use
		glUseProgram(material.shader);

		// Mostly here to ensure cubemaps don't draw on top of anything else
		if (material.willDrawDepth) {
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}
		else {
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
		}

		// Tell the gpu what diffuse textures to use
		// TODO: Send all textures to the GPU, not just 1
		GLuint textureUnit = 0;
		for (size_t j = 0; j < material.colorMaps.size(); ++j) {
			const Texture& texture = material.colorMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(glGetUniformLocation(material.shader, "sampler"), 0);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 texture currently
			break;
		}

		// Set environment map to use on GPU
		if (s_renderState.hasEnvironmentMap) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(glGetUniformLocation(material.shader, "environmentSampler"), 1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, s_renderState.environmentMap);
		}

		// Send shader parameters to gpu
		// TODO: Remove these and use texture maps instead
		GLuint blockIndex;
		blockIndex = glGetUniformBlockIndex(material.shader, "ShaderParams");
		glUniformBlockBinding(material.shader, blockIndex, s_renderState.shaderParamsBindingPoint);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_renderState.shaderParamsBindingPoint, s_renderState.uboShaderParams);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ShaderParams), &material.shaderParams);

		// Send the model view and projection matrices to the gpu
		// TODO: Rename Uniforms to better represent its purpos (MVP matrices + camera position)
		blockIndex = glGetUniformBlockIndex(material.shader, "Uniforms");
		glUniformBlockBinding(material.shader, blockIndex, s_renderState.uniformBindingPoint);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_renderState.uniformBindingPoint, s_renderState.uboUniforms);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformFormat), &uniforms);

		// Render the mesh
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
	}
}
