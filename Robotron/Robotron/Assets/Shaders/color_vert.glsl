#version 420 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inColor;

out VertexData {
    vec3 normal;
    vec2 texCoord;
	vec3 viewDir;
	vec3 color;
} o;

layout (std140) uniform Uniforms {
    mat4 model;
    mat4 view;
    mat4 projection;
	vec4 cameraPos;
	float metallicness;
	float glossiness;
} u;

void main()
{
	vec3 worldPos = (u.model * vec4(inPosition, 1)).xyz;

    o.normal = (u.model * vec4(inNormal, 0)).xyz; // TODO: Do inverse transpose
    o.texCoord = inTexCoord;
	o.viewDir = (u.cameraPos.xyz - worldPos).xyz;
	o.color = inColor;

    gl_Position = u.projection * u.view * vec4(worldPos, 1);
}
