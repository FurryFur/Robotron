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
#include "GLMUtils.h"
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
#include <glm\gtx\euler_angles.hpp>

#include <cmath>

using glm::mat4;
using glm::vec3;
using glm::vec4;

RenderState RenderSystem::s_renderState;

RenderSystem::RenderSystem(GLFWwindow* glContext, Scene& scene)
	: m_scene{ scene }
{
	m_scene.registerEventListener(this);
	m_renderState.glContext = glContext;
	m_renderState.uniformBindingPoint = 0;
	m_renderState.hasIrradianceMap = false;
	m_renderState.hasRadianceMap = false;

	// Create buffer for uniforms
	glGenBuffers(1, &m_renderState.uboUniforms);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_renderState.uniformBindingPoint, m_renderState.uboUniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformFormat), nullptr, GL_DYNAMIC_DRAW);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

RenderSystem::~RenderSystem()
{
	m_scene.removeEventListener(this);
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& tip,
	const glm::vec3& color)
{
	vec3 direction = tip - base;
	drawDebugArrow(base, direction, glm::length(direction));
}

void RenderSystem::drawDebugArrow(const glm::vec3& base, const glm::vec3& _direction, 
	float magnitude, const glm::vec3& color)
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
	for (size_t i = 0; i < model.materials.size(); ++i) {
		model.materials.at(i).shader = GLUtils::getDebugShader();
		model.materials.at(i).debugColor = color;
	}

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
	if (!entity.hasComponents(kRenderableMask))
		return;

	// Can't render anything without a camera set
	if (!m_renderState.cameraEntity) {
		return;
	}

	bool hasTransform = entity.hasComponents(COMPONENT_TRANSFORM);

	// Swap the current global render state with this RenderSystems state.
	s_renderState = m_renderState;

	// Render the 

	renderModel(entity.model, GLMUtils::transformToMat(entity.transform));
}

void RenderSystem::setCamera(const Entity* entity)
{
	m_renderState.cameraEntity = entity;
}

void RenderSystem::setRadianceMap(GLuint radianceMap)
{
	m_renderState.radianceMap = radianceMap;
	m_renderState.hasRadianceMap = true;
}

void RenderSystem::setIrradianceMap(GLuint irradianceMap)
{
	m_renderState.irradianceMap = irradianceMap;
	m_renderState.hasIrradianceMap = true;
}

void RenderSystem::renderModel(const ModelComponent& model, const glm::mat4& transform)
{
	// Get Aspect ratio
	int width, height;
	GLFWwindow* glContext = glfwGetCurrentContext();
	glfwGetFramebufferSize(glContext, &width, &height);
	float aspectRatio = static_cast<float>(width) / height;

	// Get model, view and projection matrices
	UniformFormat uniforms;
	
	uniforms.model = transform;
	uniforms.view = s_renderState.cameraEntity->camera.lookAt;
	uniforms.projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.5f, 100.0f);
	uniforms.cameraPos = glm::vec4(s_renderState.cameraEntity->camera.position, 1.0f);

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
			glUniform1i(glGetUniformLocation(material.shader, "colorSampler"), textureUnit);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 diffuse texture currently
			break;
		}

		for (size_t j = 0; j < material.metallicnessMaps.size(); ++j) {
			const Texture& texture = material.metallicnessMaps.at(j);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(glGetUniformLocation(material.shader, "metallicnessSampler"), textureUnit);
			glBindTexture(texture.target, texture.id);
			++textureUnit;

			// Just doing 1 specular texture currently
			break;
		}

		// Set environment map to use on GPU
		if (s_renderState.hasRadianceMap) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(glGetUniformLocation(material.shader, "radianceSampler"), textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, s_renderState.radianceMap);
			++textureUnit;
		}
		if (s_renderState.hasIrradianceMap) {
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glUniform1i(glGetUniformLocation(material.shader, "irradianceSampler"), textureUnit);
			glBindTexture(GL_TEXTURE_CUBE_MAP, s_renderState.irradianceMap);
			++textureUnit;
		}

		// Set shader parameters
		uniforms.metallicness = material.shaderParams.metallicness;
		uniforms.glossiness = material.shaderParams.glossiness;
		uniforms.specBias = material.shaderParams.specBias;

		// Set spotlights
		uniforms.numSpotlights = std::min(static_cast<GLuint>(s_renderState.spotlights.size()), UniformFormat::s_kMaxSpotlights);
		for (GLuint i = 0; i < uniforms.numSpotlights; ++i) {
			const Entity* spotlightEntity = s_renderState.spotlights.at(i);
			glm::vec4 spotlightDir = glm::vec4(s_renderState.spotlights.at(i)->spotlight.direction, 0);
			// Transform to local coordinates of containing entity
			glm::mat4 orientation = GLMUtils::eulerToMat(spotlightEntity->transform.eulerAngles);
			spotlightDir = orientation * spotlightDir;
			// Set spotlights in GPU uniform
			uniforms.spotlightDirections.at(i) = spotlightDir;
			uniforms.spotlightPositions.at(i) = glm::vec4(spotlightEntity->transform.position, 1);
			uniforms.spotlightColors.at(i) = glm::vec4(spotlightEntity->spotlight.color, 1);
		}

		// Send uniform data to the GPU
		GLuint blockIndex = glGetUniformBlockIndex(material.shader, "Uniforms");
		glUniformBlockBinding(material.shader, blockIndex, s_renderState.uniformBindingPoint);
		glBindBufferBase(GL_UNIFORM_BUFFER, s_renderState.uniformBindingPoint, s_renderState.uboUniforms);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformFormat), &uniforms);
		if (material.shader == GLUtils::getDebugShader()) {
			const glm::vec3& debugColor = material.debugColor;
			glUniform3f(glGetUniformLocation(material.shader, "debugColor"), debugColor.r, debugColor.g, debugColor.b);
		}

		// Render the mesh
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, 0);
	}
}

void RenderSystem::onAddComponents(Entity& entity, size_t componentMask)
{
	if (!Entity::matches(componentMask, COMPONENT_SPOTLIGHT))
		return;

	m_renderState.spotlights.push_back(&entity);
}

void RenderSystem::onRemoveComponents(Entity& entity, size_t componentMask)
{
	if (!Entity::matches(componentMask, COMPONENT_SPOTLIGHT))
		return;

	auto removeIt = std::remove(m_renderState.spotlights.begin(), m_renderState.spotlights.end(), &entity);
	if (removeIt != m_renderState.spotlights.end())
		m_renderState.spotlights.erase(removeIt);
}

void RenderSystem::onEntityCreation(Entity& entity)
{
	entity.registerEventListener(this);

	if (entity.hasComponents(COMPONENT_SPOTLIGHT)) {
		m_renderState.spotlights.push_back(&entity);
	}
}

void RenderSystem::onEntityDestruction(Entity& entity)
{
	entity.removeEventListener(this);

	if (entity.hasComponents(COMPONENT_SPOTLIGHT)) {
		auto removeIt = std::remove(m_renderState.spotlights.begin(), m_renderState.spotlights.end(), &entity);
		if (removeIt != m_renderState.spotlights.end())
			m_renderState.spotlights.erase(removeIt);
	}
}
