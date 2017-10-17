#pragma once

#include <glad\glad.h>

#include <vector>

struct VertexFormat;
struct MeshComponent;

namespace GLPrimitives {
	// Returns the vertices to construct a quad.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getQuadVertices();

	// Returns the indices to construct a quad.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getQuadIndices();

	// Returns the vertices to construct a sphere.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getSphereVertices();

	// Returns the indices to construct a sphere.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<GLuint>& getSphereIndices();

	// Returns the vertices to construct a cylinder.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getCylinderVertices();

	// Returns the indices to construct a cylinder.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getCylinderIndices();

	// Returns the vertices to construct a pyramid.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getPyramidVertices();

	// Returns the indices to construct a pyramid.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getPyramidIndices();

	// Returns the vertices to construct a cube.
	// This function is cached for efficiency 
	// (only 1 set of vertices will be constructed).
	const std::vector<VertexFormat>& getCubeVertices();

	// Returns the indices to construct a cube.
	// This function is cached for efficiency 
	// (only 1 set of indices will be constructed).
	const std::vector<GLuint>& getCubeIndices();

	// Returns a Mesh Component containing the VAO for a quad.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	MeshComponent getQuadMesh();

	// Returns a Mesh Component containing the VAO for a sphere.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	MeshComponent getSphereMesh();

	// Returns a Mesh Component containing the VAO for a cylinder.
	// This function is cached for efficiency 
	// (Only 1 mesh will be buffered).
	MeshComponent getCylinderMesh();

	// Returns a Mesh Component containing the VAO for a pyramid.
	// This function is cached for efficiency
	// (Only 1 mesh will be buffered).
	MeshComponent getPyramidMesh();

	// Returns a Mesh Component containing the VAO for a cube.
	// This function is cached for efficiency
	// (Only 1 mesh will be buffered).
	MeshComponent getCubeMesh();
}