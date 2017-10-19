#version 450 core

in VertexData {
    vec3 textureDir;
} i;

out vec4 outColor;

layout (std140) uniform Uniforms {
    mat4 model;
    mat4 view;
    mat4 projection;
	vec4 cameraPos;
	float metallicness;
	float glossiness;
} u;

uniform samplerCube skybox;

void main(void)
{
    outColor = texture(skybox, i.textureDir);
}
