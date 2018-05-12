#version 150 core

uniform vec4 colour;

out vec4 FragColor;

void main(void) {
	FragColor = colour;
}