#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "TextMesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/MD5Mesh.h"
#include <algorithm>
#include <math.h>
#include <sstream>

#define SHADOWSIZE 2048
#define POST_PASSES 4

class Renderer : public OGLRenderer {
public:
	//for pausing shader animation in matrixScene.
	bool pause;
	//for automatically changing scene.
	bool autoChangeScene;
	//for switch on/off post process effect.
	bool postProcessEnable;
	
	Renderer(Window & parent);
	virtual ~Renderer(void);

	void SetCurrentScene(unsigned s);
	unsigned const GetCurrentScene() { return currentScene; };
	unsigned const GetNumOfScene() { return MAX_SCENE; };

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void DrawText(const std::string &text, Font* font, const Vector3 &position, const float size = 10.0f, const bool perspective = false);
protected:
	enum Scene {
		MOUNTAIN_SCENE,
		MATRIX_SCENE,
		SHADOW_SCENE,
		MAX_SCENE
	};

	bool shadowOnDraw;

	void DrawDebugCoord();
	void DrawFrameRate();

	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawPostProcess();
	void PresentScene();

	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);

	void GenerateBuildingNode(float height,float x,float y);

	Shader* debugShader;
	Shader* lightShader;
	Shader* spotLightShader;
	Shader* texturedShader;
	Shader* bubbleShader;
	Shader* skyboxShader;
	Shader* matrixShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* processShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* screen;
	Mesh* bubbleSphere;
	Mesh* debugCoord;
	Mesh* floor;
	Mesh* ground;
	Mesh* wall;
	Mesh* roof;
	MD5FileData * hellData;

	Scene currentScene;

	SceneNode* root;

	SceneNode* mountainScene;
	SceneNode* mountain;
	SceneNode* mountainSky;
	SceneNode* mountainEnviroSphere;

	SceneNode* matrixScene;
	SceneNode* matrixFloor;
	SceneNode* matrixBuilding;
	SceneNode* matrixBuildingWall;
	SceneNode* matrixBuildingRoof;
	unsigned numOfBuildings;

	SceneNode* shadowScene;
	SceneNode* shadowFloor;
	MD5Node * hellNode;

	Frustum frameFrustum;

	vector <SceneNode *> transparentNodeList;
	vector <SceneNode *> nodeList;

	Light* mountainLight;
	Vector3 lightDirection;
	Light* matrixLight;
	Light* shadowLight;

	Camera* camera;
	Camera* subCamera;

	GLuint cubeMap;

	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;

	short frameCounter;
	float timeCounter;
	float time;

	std::stringstream fps;
	Font* fpsFont;
};
