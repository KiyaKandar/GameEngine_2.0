#version 150 core

uniform sampler2D diffuseTex;
uniform vec3 colour;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	
	vec4 textColour = texture(diffuseTex, IN.texCoord);
	FragColor = textColour;
}