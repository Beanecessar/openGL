#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "GLTools.h"
#include "../../nclgl/Printer.h"
#include "../../nclgl/SceneNode.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void DrawHeightmap();
	void DrawDebugCoord();
	void DrawSkybox();
	void DrawBubbleSphere();
	void DrawFrameRate();

	Shader* debugShader;
	Shader* lightShader;
	Shader* fpsShader;
	Shader* bubbleShader;
	Shader* skyboxShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* bubbleSphere;
	Mesh* debugCoord;

	Printer printer;

	SceneNode* root;
	SceneNode* mountainScene;
	SceneNode* mountain;
	SceneNode* mountainSky;
	SceneNode* mountainEnviroSphere;
	SceneNode* matrixScene;
	SceneNode* matrixFloor;
	SceneNode** matrixBuildings;
	unsigned numOfBuildings;

	Light* light;
	Camera* camera;

	GLuint cubeMap;

	short frameCounter;

	char fps[20];
};
