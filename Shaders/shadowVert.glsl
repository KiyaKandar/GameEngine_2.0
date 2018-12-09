#version 430

layout(location = 0) in vec3 aPos;
layout(location = 5) in ivec4 BoneIDs;
layout(location = 6) in vec4 Weights;

const int MAX_BONES = 200;
uniform int anim;
uniform mat4 gBones[MAX_BONES];

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main(void)
{
	mat4 animTransform = mat4(1.0);
	if (anim == 1)
	{
		animTransform = gBones[BoneIDs[0]] * Weights[0];

		for (int i = 1; i < 4; ++i)
		{
			animTransform += gBones[BoneIDs[i]] * Weights[i];
		}
	}

	gl_Position = (projMatrix * viewMatrix * modelMatrix * animTransform * vec4(aPos, 1.0));
}