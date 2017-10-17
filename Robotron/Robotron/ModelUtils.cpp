#include "ModelUtils.h"

#include "ModelComponent.h"
#include "Mesh.h"
#include "VertexFormat.h"
#include "Texture.h"
#include "GLUtils.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <iostream>
#include <unordered_map>

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	static std::unordered_map<std::string, Texture> s_texturesLoaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < s_texturesLoaded.size(); j++)
		{
			if (s_texturesLoaded.find(str.C_Str()) != s_texturesLoaded.end())
			{
				textures.push_back(s_texturesLoaded.at(str.C_Str()));
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture = GLUtils::loadTexture(str.C_Str(), type);
			s_texturesLoaded.insert(std::make_pair(str.C_Str(), texture));  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			textures.push_back(std::move(texture));
		}
	}
	return textures;
}

Mesh processMesh(aiMesh* _mesh, const aiScene* scene)
{
	// Data to fill
	Mesh mesh;

	// Walk through each of the mesh's vertices
	std::vector<VertexFormat> vertices;
	for (GLuint i = 0; i < _mesh->mNumVertices; i++)
	{
		VertexFormat vertex;
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // Positions
		vector.x = _mesh->mVertices[i].x;
		vector.y = _mesh->mVertices[i].y;
		vector.z = _mesh->mVertices[i].z;
		vertex.position = vector;
		// Normals
		vector.x = _mesh->mNormals[i].x;
		vector.y = _mesh->mNormals[i].y;
		vector.z = _mesh->mNormals[i].z;
		vertex.normal = vector;
		// Texture Coordinates
		if (_mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = _mesh->mTextureCoords[0][i].x;
			vec.y = _mesh->mTextureCoords[0][i].y;
			vertex.texCoord = vec;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
		vertices.push_back(std::move(vertex));
	}

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	std::vector<GLuint> indices;
	for (GLuint i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Process materials
	if (_mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[_mesh->mMaterialIndex];
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);

		mesh.textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void processNode(aiNode* node, const aiScene* scene, ModelComponent& outModel) 
{
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		outModel.meshes.push_back(processMesh(mesh, scene));
	}


	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, outModel);
	}
}

ModelComponent ModelUtils::loadModel(const std::string& path)
{
	static Assimp::Importer s_importer;
	const aiScene* scene = s_importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	// TODO: Throw an exception here
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << s_importer.GetErrorString() << std::endl;
		return ModelComponent();
	}
	// Retrieve the directory path of the filepath
	std::string directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	//this->processNode(scene->mRootNode, scene);

	ModelComponent model;
	return proccessNode(scene->mRootNode, scene, model);
}
