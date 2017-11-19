#version 150 core

uniform samplerCube cubeTex;
uniform vec3 cameraPos;

in Vertex{
	vec3 worldPos;
	vec3 normal;
} IN;

out vec4 FragColor;

void main(void) {
	vec3 incidentDir = normalize(cameraPos - IN.worldPos);
	vec3 reflectDir = reflect(incidentDir, IN.normal);
	FragColor = texture (cubeTex, normalize(reflectDir));
	//FragColor = vec4(IN.normal, 1.0);
}