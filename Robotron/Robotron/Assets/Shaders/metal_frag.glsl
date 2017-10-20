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
uniform samplerCube reflectionSampler;
uniform samplerCube irradianceSampler;

const float PI = 3.1415926535897932384626433832795;
const vec3 lightDir = vec3(1, 1, -1);
const vec3 LiDirect = vec3(0.64, 0.39, 0.31) * 4;
const float kDiffNorm = 1 / PI;

vec3 fresnel(vec3 specColor, vec3 lightDir, vec3 halfVector)
{
	return specColor + (1.0f - specColor) * pow(1.0f - clamp(dot(lightDir, halfVector), 0, 1), 5);
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
	vec3 halfVector = normalize(normalize(lightDir) + viewDir);
	float ndotl = clamp(dot(normalize(lightDir), normal), 0, 1);
	float ndoth = clamp(dot(normal, halfVector), 0, 1);

	// Reflection variables
	vec3 LiReflDir = normalize(reflect(-viewDir, normal)); // The light direction that reflects directly into the camera
	vec3 LiRefl = texture(reflectionSampler, LiReflDir).rgb;
	float ndotRl = clamp(dot(LiReflDir, normal), 0, 1);
	// vec3 LiReflHalfVec = normalize(LiReflDir + viewDir); // Same as normal
	// float ndotRh = clamp(dot(normal, LiReflHalfVec), 0, 1);
	float specPow = u.glossiness;
	float specNorm = (specPow + 4) * (specPow + 2) / (8 * PI * (specPow + pow(2, -specPow / 2)));

	vec3 metallicness = u.metallicness + texture(metallicnessSampler, i.texCoord).rgb;
	vec3 Cspec = metallicness * color;
	vec3 Cdiff = (1 - metallicness) * color;
	vec3 Fdirect = fresnel(Cspec, lightDir, halfVector);
	vec3 Frefl = fresnel(Cspec, LiReflDir, normal);
	vec3 CdiffDirect = Cdiff * (1 - Fdirect) / (1 - Cspec);
	vec3 CdiffAmb = Cdiff * (1 - Frefl) / (1 - Cspec);
	vec3 BRDFdiff = kDiffNorm * CdiffDirect;
	vec3 BRDFglob = CdiffAmb; // CubeMapGen already applies normalization for irradiance maps
	vec3 BRDFspec =  specNorm * Fdirect * pow(ndoth, specPow);
	vec3 BRDFreflSpec = specNorm * Frefl; // pow(ndotRh, specPow) is always 1 becuase the reflected half angle is always aligned with the normal
	vec3 BRDFdirect = BRDFdiff + BRDFspec;

	vec3 LrDirect = LiDirect * BRDFdirect * ndotl;
	vec3 LrGlobal = texture(irradianceSampler, normal).rgb * BRDFglob; // Assume ndotl has already been integrated in irradiance map
	vec3 LrReflSpec = LiRefl * BRDFreflSpec * ndotRl;

	outColor = vec4(LrDirect + LrReflSpec + LrGlobal, 1);
}
