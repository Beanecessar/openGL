#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in  vec3 position;
in  vec4 colour;

out Vertex {
	vec4 colour;
} OUT;

void main(void)	{
	gl_Position	  = modelMatrix * viewMatrix * projMatrix * vec4(position, 1.0);
	OUT.colour    = colour;
}