#version 420 core

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
	float specBias;
} u;

uniform samplerCube colorSampler;

void main(void)
{
    outColor = texture(colorSampler, i.textureDir);
}
