#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;

uniform int anim;
const int MAX_BONES = 200;
uniform mat4 gBones[MAX_BONES];

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 reflectionPos;
out vec3 ReflectionNormal;

void main(void) 
{
	mat4 animTransform(1.0f);

	if (anim == 1)
	{
		animTransform = gBones[BoneIDs[0]] * Weights[0];
		animTransform += gBones[BoneIDs[1]] * Weights[1];
		animTransform += gBones[BoneIDs[2]] * Weights[2];
		animTransform += gBones[BoneIDs[3]] * Weights[3];
	}

	vec4 viewPos = viewMatrix * modelMatrix * animTransform * vec4(aPos, 1.0);
	reflectionPos = vec3(modelMatrix * animTransform * vec4(aPos, 1.0));

	FragPos = viewPos.xyz;
	TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix * animTransform)));
	Normal = normalMatrix * animTransform * (vec4(aNormal, 1.0)).xyz;
	ReflectionNormal = mat3(transpose(inverse(modelMatrix * animTransform))) * aNormal;

	gl_Position = projMatrix * viewPos;
}