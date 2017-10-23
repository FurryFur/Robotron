#version 450 core

in VertexData {
	vec3 normal;
	vec2 texCoord;
	vec3 viewDir;
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

uniform sampler2D colorSampler;
uniform sampler2D metallicnessSampler;
uniform samplerCube radianceSampler;
uniform samplerCube irradianceSampler;

vec3 lightDir = vec3(1, 1, -1);
const vec3 LiDirect = vec3(0.64, 0.39, 0.31);
const int pmremMipCount = 11;

vec3 fresnel(vec3 specColor, vec3 lightDir, vec3 halfVector)
{
	return specColor + (1.0f - specColor) * pow(1.0f - clamp(dot(lightDir, halfVector), 0, 1), 5);
}

vec3 fresnelWithGloss(vec3 specColor, vec3 lightDir, vec3 halfVector, float gloss)
{
    return specColor + (max(vec3(gloss, gloss, gloss), specColor) - specColor) * pow(1.0f - clamp(dot(lightDir, halfVector), 0, 1), 5);
}

void main(void)
{
	vec3 normal;
	if (gl_FrontFacing)
		normal = normalize(i.normal);
	else
		normal = -normalize(i.normal);

	vec3 color = texture(colorSampler, i.texCoord).rgb;

	// Direct Lighting variables
	vec3 viewDir = normalize(i.viewDir);
	lightDir = normalize(lightDir);
	vec3 halfVector = normalize(lightDir + viewDir);
	float ndotl = clamp(dot(lightDir, normal), 0, 1);
	float ndoth = clamp(dot(normal, halfVector), 0, 1);

	// Reflection variables
	float specPow = exp2(10 * u.glossiness + 1);
	float specNorm = (specPow + 8) / 8;
	float mipmapIndex = (1 - u.glossiness) * (pmremMipCount - 1); 
	vec3 LiReflDir = normalize(reflect(-viewDir, normal)); // The light direction that reflects directly into the camera
	vec3 LiRefl = textureLod(radianceSampler, LiReflDir, mipmapIndex).rgb;
	vec3 LiIrr = texture(irradianceSampler, normal).rgb;

	vec3 metallicness = clamp(u.metallicness + texture(metallicnessSampler, i.texCoord).rgb, vec3(0, 0, 0), vec3(1, 1, 1));
	vec3 Cspec = mix(vec3(0.04, 0.04, 0.04), color, metallicness);
	vec3 Cdiff = mix(vec3(0, 0, 0), color, 1 - metallicness);
	vec3 Fspec = fresnel(Cspec, lightDir, halfVector);
	vec3 Fdiff = Cdiff * (1 - Fspec) / (1.0000001 - Cspec);
	vec3 FspecRefl = fresnelWithGloss(Cspec, LiReflDir, normal, u.glossiness);
	vec3 FdiffRefl = Cdiff * (1 - FspecRefl) / (1.0000001 - Cspec);

	vec3 BRDFspec = specNorm * Fspec * pow(ndoth, specPow);

	vec3 LrDirect = LiDirect * (Fdiff + BRDFspec) * ndotl;
	vec3 LrAmbDiff= LiIrr * FdiffRefl;
	vec3 LrAmbSpec = LiRefl * FspecRefl;

	outColor = vec4(LrDirect + LrAmbDiff + LrAmbSpec, 1);
}
