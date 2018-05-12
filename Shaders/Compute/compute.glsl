#version 430 core

layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

uniform sampler2D paintTrailTexture;
uniform int arraySize;
uniform vec2 resolution;

layout(std430, binding = 4) buffer PlayerScoresBuffer
{
	int scores[];
};

layout(std430, binding = 5) buffer ColourBuffer
{
	vec4 colours[];
};

void main()
{
	int xCoord = int(gl_GlobalInvocationID.x);
	int yCoord = int(gl_GlobalInvocationID.y);

	float textureCoordX = float(xCoord) / resolution.x;
	float textureCoordY = float(yCoord) / resolution.y;

	vec4 colour = texture(paintTrailTexture, vec2(textureCoordX, textureCoordY));

	for (int i = 0; i < arraySize; i++)
	{
		if (length(colour.rgb - colours[i].rgb) < 0.01f)
		{
			atomicAdd(scores[i], 1);
		}
	}
}

