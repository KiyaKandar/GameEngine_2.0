#version 430

out vec4 FragColor;

uniform int numberOfLights;

uniform float ambientLighting;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform sampler2DShadow shadows;
uniform mat4 texMatrices;
uniform mat4 camMatrix;
uniform mat4 viewMatrix;

uniform sampler2D ssaoTexture;
uniform bool ssaoApplied;
uniform bool shadowsApplied;

uniform int numXTiles;
uniform int numYTiles;

in vec2 TexCoords;
in mat4 textureMat;
in vec2 screenPos;

struct LightData
{
	vec4 pos;
	vec4 lightColour;
	float lightRadius;
	float isShadowCasting;
	float intensity;

	float fpadding;
};

layout (std430, binding = 1) buffer LightDataBuffer
{
	LightData lightData[];
};

void AddLighting(int index, vec3 position, vec3 normal, vec4 albedo, inout vec4 finalColour)
{
	vec3 lightPosition = lightData[index].pos.xyz;
	vec3 lightPosView = vec3(camMatrix * vec4(lightPosition, 1.0));
	float dist = length(lightPosView - position);

	if (dist <= lightData[index].lightRadius)
	{
		//Diffuse
		vec3 viewDir = normalize(-position);
		vec3 lightDir = normalize(lightPosView - position);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) *
			albedo.rgb * lightData[index].lightColour.rgb;

		//Specular
		vec3 halfDir = normalize(lightDir + viewDir);
		float specPower = pow(max(dot(normal, halfDir), 0.0), 50.0);
		vec3 specular = lightData[index].lightColour.rgb * specPower;

		//Attenuation
		float attenuation = 0;
		attenuation = 1.0 - clamp(dist / lightData[index].lightRadius, 0.0, 1.0);
		attenuation *= lightData[index].intensity;

		diffuse *= attenuation;
		specular *= attenuation;

		finalColour.rgb += diffuse.rgb + specular.rgb;

		//shadow
		if (shadowsApplied)
		{
			if (lightData[index].isShadowCasting > 0)
			{
				float lambert = max(0.0, dot(lightDir, normal));

				//Shadow
				vec4 shadowProj = (texMatrices * inverse(camMatrix) *
					vec4(position + (-normal * 1.0), 1));

				float shadow = 0.0;

				if (shadowProj.w > 0.0)
				{
					vec2 texelSize = vec2(1.0f, 1.0f) / vec2(4096.0f, 4096.0f);
					int sampleCount = 0;

					for (int x = -3; x <= 3; ++x)
					{
						for (int y = -3; y <= 3; ++y)
						{
							vec2 sampleCoord = vec2(x, y) * texelSize * 1000.0f;
							shadow += textureProj(shadows, shadowProj + vec4(sampleCoord, 0.0f, 0.0f));

							sampleCount++;
						}
					}

					shadow /= float(sampleCount);// pow((HALF_NUM_PCF_SAMPLES) * 2, 2);
				}

				lambert *= shadow;
				attenuation *= lambert;

				finalColour.rgb *= shadow;
			}
		}

	}
}

void main(void){
	vec3 position = texture(gPosition, TexCoords).xyz;
	vec3 normal = texture(gNormal, TexCoords).xyz;
	vec4 albedo = texture(gAlbedo, TexCoords);

	if (position.z > 0.0f)
	{
		//It's the skybox, dont touch it...
		FragColor = albedo;
	}
	else 
	{
		vec4 finalColour = vec4(0, 0, 0, 1);

		for (int i = 0; i < numberOfLights; i++)
		{
			AddLighting(i, position, normal, albedo, finalColour);
		}

		if (ssaoApplied)
		{
			float ambientComponent = 0.6f * texture(ssaoTexture, TexCoords).r;
			vec3 ambientColour = albedo.rgb * ambientComponent;

			finalColour.rgb += ambientColour;
		}
		else
		{
			vec3 ambientColour = albedo.rgb * 0.6f;
			finalColour.rgb += ambientColour;
		}

		FragColor = finalColour;
	}
}