#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawBubbleSphere();

	Shader * lightShader;
	//Shader * reflectShader;
	Shader * bubbleShader;
	Shader * skyboxShader;

	HeightMap * heightMap;
	Mesh * quad;
	Mesh * bubbleSphere;

	Light * light;
	Camera * camera;

	GLuint cubeMap;

	float waterRotate;
};
