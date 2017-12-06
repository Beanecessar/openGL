#version 150 core

uniform sampler2D diffuseTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightAngle;
uniform float lightRadius;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 FragColor;
void main(void) {
	vec3 direction = normalize(lightPos-IN.worldPos);
	float theta = dot(direction, normalize(-lightDir));
	if(dot(direction ,normalize(IN.normal))>0&&theta > cos(radians(lightAngle))){
		FragColor = lightColour;
	}
	else
		FragColor = vec4(0,0,0,1);
}