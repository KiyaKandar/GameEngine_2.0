#version 150 core

uniform sampler2D diffuseTex;
uniform vec3 colour;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	
	vec4 textColour = texture(diffuseTex, IN.texCoord);

	//if (textColour.r > 0.1f && textColour.g > 0.1f && textColour.b > 0.1f)
	if(textColour.rgb != vec3(0.0f, 0.0f, 0.0f))
	{
		textColour.rgb = colour;
	}
	else
	{
		textColour.a = 0.4f;
	}

	FragColor = textColour;
}