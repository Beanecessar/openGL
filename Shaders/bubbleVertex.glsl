#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in  vec3 position;
in  vec3 normal;

out Vertex {
	vec3 worldPos;
	vec3 normal;
} OUT;

void main(void)	{
	gl_Position	  = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	OUT.worldPos    = (modelMatrix * vec4(position, 1.0)).xyz;
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
 	OUT.normal = normalize(normalMatrix  * normal);
}