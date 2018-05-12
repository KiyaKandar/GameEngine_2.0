#version 150 core

uniform sampler2D texture_diffuse;
uniform int hasTexture;

in Vertex {
 vec4 colour;
vec2 TexCoords;
}IN;

out vec4 gl_FragColor;

void main(void) {
	vec4 finalColour = IN.colour;

	if (hasTexture == 1)
	{
		finalColour = texture(texture_diffuse, IN.TexCoords);
	}

	gl_FragColor = finalColour;
}