#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 modelMatrix;
uniform mat4 textureMatrix;


out Vertex {
	vec2 texCoord;
} OUT;

void main(void) {	
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(aPos, 1.0);
	OUT.texCoord = (textureMatrix * vec4(aTexCoords, 0.0, 1.0)).xy;
}