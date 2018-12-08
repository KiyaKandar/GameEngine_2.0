#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

uniform sampler2D texture_diffuse;
uniform int hasTexture;
uniform vec2 resolution;

uniform sampler2D paintTrailTexture;
uniform mat4 paintTrailTextureMatrix;
uniform int isPaintSurface;

uniform vec3 cameraPos;
uniform vec4 baseColour;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform int isReflective;
uniform float reflectiveStrength;
uniform samplerCube skybox;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 reflectionPos;
in vec3 ReflectionNormal;

void main(void) {
	gPosition = FragPos;
	gNormal = normalize(Normal);

	vec4 col = baseColour;
	float alpha = col.a;

	if (hasTexture == 1)
	{
		col = texture(texture_diffuse, TexCoords);
	}

	if (col.a < 0.01f)
	{
		discard;
	}

	if (isPaintSurface == 1)
	{
		vec4 paintTrailProjection = (paintTrailTextureMatrix * inverse(viewMatrix) *
			vec4(gPosition + (-gNormal * 1.0), 1));

		vec4 paintColour = vec4(0.0f, 0.0f, 0.0f, 0.0f);

		if (paintTrailProjection.w > 0.0)
		{
			vec2 texelSize = vec2(1.0f, 1.0f) / resolution;

			int sampleCount = 0;

			for (int x = -11; x <= 11; ++x)
			{
				for (int y = -11; y <= 11; ++y)
				{
					vec2 sampleCoord = vec2(x, y) * texelSize * 100.0f;
					paintColour += textureProj(paintTrailTexture, paintTrailProjection + vec4(sampleCoord, 0.0f, 0.0f));

					sampleCount++;
				}
			}

			paintColour /= sampleCount;

		}

		if (length(paintColour) > 1.2f)
		{
			col *= paintColour;

			vec3 i = normalize(reflectionPos - cameraPos);
			vec3 r = reflect(i, normalize(ReflectionNormal));
			vec4 reflectioncolour = vec4(texture(skybox, r).rgb, 1.0);

			col.rgb += reflectioncolour.rgb * 1.0f;
			col /= 1.7;
		}
	}
	else if (isReflective == 1)
	{
		vec3 i = normalize(reflectionPos - cameraPos);
		vec3 r = reflect(i, normalize(ReflectionNormal));
		vec4 reflectioncolour = vec4(texture(skybox, r).rgb, 1.0);

		col.rgb += reflectioncolour.rgb * reflectiveStrength;
		col /= 2;
	}

	gAlbedo = vec4(col.rgb, 1.0);
}