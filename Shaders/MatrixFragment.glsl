#version 150 core

#define _CharNum 10
#define _FontColor vec4(0.4,1,0.4,1)

uniform sampler2D numTex;
uniform sampler2D lightTex;
uniform sampler2D fontTex;
uniform float time;
uniform float numTexSize;
uniform float lightTexSize; 
uniform float randomSeed;
uniform float speed;
uniform float texelSize;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 lightDir;
uniform float lightAngle;
uniform float lightRadius;

in Vertex{
	vec2 texCoord;
	vec3 worldPos;
	vec3 normal;
}IN;

out vec4 FragColor;

void main(void) {
	vec2 texelCoord=IN.texCoord/texelSize;

	float adjustSpeed=texture(numTex,texelCoord.xx*numTexSize).g*2+0.3;
	texelCoord.y-=(time+randomSeed)*speed*adjustSpeed;

	float changeSpeed=texture(numTex,texelCoord.xy*numTexSize).b*1+0.5;
	float num=round(texture(numTex,(texelCoord-vec2(round((time+randomSeed)*speed*changeSpeed),0))*numTexSize).r*_CharNum)/_CharNum;
	vec2 numTexOffset=vec2(num,0);
	vec4 flowingNum=texture(fontTex,vec2(texelCoord.x/_CharNum,texelCoord.y)+numTexOffset);

	vec4 light=texture(lightTex,texelCoord*lightTexSize);
 
	//FragColor=flowingNum*light*_FontColor;
	vec3 direction = normalize(lightPos-IN.worldPos);
	float theta = dot(direction, normalize(-lightDir));
	if(dot(direction ,normalize(IN.normal))>0&&theta > cos(radians(lightAngle))){
		FragColor = vec4(lightColour.rgb*0.6+(flowingNum*light*_FontColor).rgb*0.4,1);
	}
	else
		FragColor = flowingNum*light*_FontColor;

	//FragColor.rgb=IN.normal;
}