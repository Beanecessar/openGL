#include "Renderer.h"

void DisableDepthMask() {
	glDepthMask(GL_FALSE);
}

void EnableDepthMask() {
	glDepthMask(GL_TRUE);
}

void EnableNearestFliter(GLuint tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DisableNearestFliter(GLuint tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	currentScene = MOUNTAIN_SCENE;
	frameCounter = 0;
	timeCounter = 0;
	time = 0;

	pause = false;
	autoChangeScene = false;
	shadowOnDraw = false;
	postProcessEnable = false;

	//--------initializing shaders--------//
	debugShader = new Shader(SHADERDIR"debugVertex.glsl",
		SHADERDIR"debugFragment.glsl");
	texturedShader = new Shader(SHADERDIR"TexturedVertex.glsl",
		SHADERDIR"TexturedFragment.glsl");
	bubbleShader = new Shader(SHADERDIR"bubbleVertex.glsl",
		SHADERDIR"bubbleFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"PerPixelFragment.glsl");
	spotLightShader = new Shader(SHADERDIR"PerPixelVertex.glsl",
		SHADERDIR"SpotLightFragment.glsl");
	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl",
		SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", 
		SHADERDIR"shadowFrag.glsl");
	matrixShader = new Shader(SHADERDIR"MatrixVertex.glsl",
		SHADERDIR"MatrixFragment.glsl");
	processShader = new Shader(SHADERDIR"TexturedVertex.glsl", 
		SHADERDIR"processfrag.glsl");

	if (!debugShader->LinkProgram() || !texturedShader->LinkProgram() || !bubbleShader->LinkProgram() ||
		!lightShader->LinkProgram() || !skyboxShader->LinkProgram() || !matrixShader->LinkProgram()||
		!sceneShader->LinkProgram() || !shadowShader->LinkProgram() || !processShader->LinkProgram()||
		!spotLightShader->LinkProgram()) {
		return;
	}
	//-----------------------------------------//

	//--------initializing shadow--------//
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//-----------------------------------------//

	camera = new Camera();
	subCamera = new Camera();

	fpsFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	debugCoord = Mesh::GenerateDebugCoord();

	//--------initializing mountainScene--------//
	mountainScene = new SceneNode();

	mountainLight = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	heightMap = new HeightMap("../../Textures/terrain.raw");

	heightMap->SetTexture(SOIL_load_OGL_texture("../../Textures/Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture("../../Textures/Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(
		"../../Textures/skybox/blood_ft.tga", "../../Textures/skybox/blood_bk.tga",
		"../../Textures/skybox/blood_up.tga", "../../Textures/skybox/blood_dn.tga",
		"../../Textures/skybox/blood_rt.tga", "../../Textures/skybox/blood_lf.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	quad = Mesh::GenerateQuad();
	mountainSky = new SceneNode(quad);
	mountainSky->SetShader(skyboxShader);
	mountainSky->SetSkybox(true);
	void DisableDepthMask();
	mountainSky->BeforeDrawCall = DisableDepthMask;
	void EnableDepthMask();
	mountainSky->AfterDrawCall = EnableDepthMask;
	mountainScene->AddChild(mountainSky);

	mountain = new SceneNode(heightMap);
	mountain->SetShader(lightShader);
	mountain->SetLight(mountainLight);
	mountain->AddUniform3fv(new Uniform3fv("cameraPos", (float *) camera->GetPositionPtr()));
	mountain->AddUniformi(new Uniformi("diffuseTex", 0));
	mountain->AddUniformi(new Uniformi("bumpTex", 1));
	mountainScene->AddChild(mountain);

	bubbleSphere = Mesh::GenerateSphere(80, 40);
	mountainEnviroSphere = new SceneNode(bubbleSphere);
	mountainEnviroSphere->AddUniform3fv(new Uniform3fv("cameraPos", (float *) camera->GetPositionPtr()));
	mountainEnviroSphere->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
	mountainEnviroSphere->SetTransform(Matrix4::Translation(Vector3(1000.0f, 2000.0f, 1000.0f)));
	mountainEnviroSphere->SetShader(bubbleShader);
	mountainScene->AddChild(mountainEnviroSphere);
	//-----------------------------------------//

	//--------initializing matrixScene--------//
	matrixScene = new SceneNode();

	lightDirection = Vector3(3.0f, -1.0f, 0);
	matrixLight = new Light(Vector3(0,500.0f,0),
		Vector4(0.9f, 0, 0, 1),
		5000.0f, &lightDirection,15.0f);

	floor = Mesh::GenerateBuildingRoof();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"floor.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	matrixFloor = new SceneNode(floor);
	matrixFloor->SetShader(spotLightShader);
	matrixFloor->SetLight(matrixLight); //just use the light in the mountainScene.
	matrixFloor->AddUniform3fv(new Uniform3fv("cameraPos", (float *)camera->GetPositionPtr()));
	matrixFloor->SetTransform(Matrix4::Scale(Vector3(8000.0f, 8000.0f, 8000.0f)));
	matrixScene->AddChild(matrixFloor);

	for (unsigned i=0;i<8;++i)
	{
		for (unsigned j = 0; j<8; ++j)
		{
			GenerateBuildingNode(rand()%20/20.0f+2.0f, i*800.0f-2800.0f, j*800.0f - 2800.0f);
		}
	}
	//-----------------------------------------//
	//--------initializing shadowScene--------//
	shadowScene = new SceneNode();

	shadowLight = new Light(Vector3(-450.0f, 200.0f, 280.0f),
		Vector4(1, 1, 1, 1), 5500.0f);

	ground = Mesh::GenerateQuad();
	ground->SetTexture(SOIL_load_OGL_texture("../../Textures/brick.tga"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	ground->SetBumpMap(SOIL_load_OGL_texture("../../Textures/brickDOT3.tga"
		, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	shadowFloor = new SceneNode(ground);
	shadowFloor->SetTransform(Matrix4::Rotation(90, Vector3(1, 0, 0)) *Matrix4::Scale(Vector3(450, 450, 1)));
	shadowScene->AddChild(shadowFloor);
	
	hellData = new MD5FileData("../../Meshes/hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData->AddAnim("../../Meshes/idle2.md5anim");
	hellNode->PlayAnim("../../Meshes/idle2.md5anim");
	shadowScene->AddChild(hellNode);
	//-----------------------------------------//

	//--------initializing post process--------//
	screen = Mesh::GenerateQuad();

	// Generate our scene depth texture ...
	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	// And our colour texture ...
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glGenFramebuffers(1, &bufferFBO); // We'll render the scene into this
	glGenFramebuffers(1, &processFBO); // And do post processing in this

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex[0], 0);
	// We can check FBO attachment success using this command !
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourTex[0]) {
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//--------------------------------------//

	SetCurrentScene(MOUNTAIN_SCENE);

	init = true;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}


Renderer ::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);

	delete camera;
	delete debugShader;
	delete lightShader;
	delete texturedShader;
	delete bubbleShader;
	delete skyboxShader;
	delete matrixShader;

	delete mountainLight;
	delete matrixLight;
	delete shadowLight;

	//delete meshes

	delete fpsFont;

	delete mountainScene;
	delete matrixScene;
	delete shadowScene;

	currentShader = 0;
}

void Renderer::SetCurrentScene(unsigned s) {
	switch (s)
	{
	case MOUNTAIN_SCENE:
		root = mountainScene;
		camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH * HEIGHTMAP_X));
		currentScene = MOUNTAIN_SCENE;
		break;
	case MATRIX_SCENE:
		root = matrixScene;
		camera->SetPosition(Vector3(250.0f,300.0f,3500.0f));
		currentScene = MATRIX_SCENE;
		break;
	case SHADOW_SCENE:
		root = shadowScene;
		camera->SetPosition(Vector3(-200.0f, 50.0f, 250.0f));
		subCamera->SetYaw(270);
		subCamera->SetPitch(0);
		subCamera->SetPosition(Vector3(-200.f, 60.f, 0.f));
		currentScene = SHADOW_SCENE;
	default:
		break;
	}
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec,0.5);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	
	root->Update(msec);

	frameCounter += 1;
	timeCounter += msec;
	if (timeCounter > 1000) {
		fps.str("");
		fps << frameCounter << " fps";
		timeCounter = 0;
		frameCounter = 0;
	}

	if (!pause)
		time += 0.01 * (msec * 60.0f / 1000); //make sure it has increasing speed in different frame rate.

	lightDirection.x = (((int)time) % 12 < 6) ? ((time - (int)time) + (int)time % 6 - 3) : (3 - ((time - (int)time) + (int)time % 6));

	lightDirection.z = (((int)time) % 12 < 6) ? sqrt(9 - pow(lightDirection.x,2)): -sqrt(9 - pow(lightDirection.x, 2));

	if (autoChangeScene&&time > 2) {
		SetCurrentScene((currentScene + 1) % MAX_SCENE);
		time = 0;
	}
	else if (time > (float)600000) {
		time = 0;
	}
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	switch (currentScene)
	{
	case Renderer::MOUNTAIN_SCENE:
		DrawNodes();
		break;

	case Renderer::MATRIX_SCENE:
		if(postProcessEnable) {
			glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
			DrawNodes();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			DrawPostProcess();
			PresentScene();
		}
		else {
			DrawNodes();
		}
		break;

	case Renderer::SHADOW_SCENE:
		shadowOnDraw = true;
		DrawShadowScene();
		DrawCombinedScene();
		shadowOnDraw = false;
		break;

	case Renderer::MAX_SCENE:
	default:
		break;
	}

	//DrawDebugCoord();
	DrawFrameRate();

	SwapBuffers();
	ClearNodeLists();
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(processShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

		screen->SetTexture(bufferColourTex[0]);
		screen->Draw();
		// Now to swap the colour buffers , and do the second blur pass
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, bufferColourTex[0], 0);

		screen->SetTexture(bufferColourTex[1]);
		screen->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(texturedShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	//projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	screen->SetTexture(bufferColourTex[0]);
	screen->Draw();
	glUseProgram(0);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);
	viewMatrix = Matrix4::BuildViewMatrix(
		shadowLight->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix *(projMatrix * viewMatrix);

	UpdateShaderMatrices();

	DrawNodes();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"shadowTex"), 3);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	SetShaderLight(*shadowLight);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();

	DrawNodes();

	textureMatrix.ToIdentity();

	viewMatrix = subCamera->BuildViewMatrix();
	UpdateShaderMatrices();

	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, 0.25*width, 0.25*height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glScissor(0.75*width, 0, 0.25*width, 0.25*height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glScissor(0, 0.75*height, 0.25*width, 0.25*height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glScissor(0.75*width, 0.75*height, 0.25*width, 0.25*height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	glViewport(0, 0, 0.25*width, 0.25*height);
	hellNode->DebugDrawSkeleton();
	glViewport(0.75*width, 0, 0.25*width, 0.25*height);
	hellNode->DebugDrawSkeleton();
	glViewport(0, 0.75*height, 0.25*width, 0.25*height);
	hellNode->DebugDrawSkeleton();
	glViewport(0.75*width, 0.75*height, 0.25*width, 0.25*height);
	hellNode->DebugDrawSkeleton();
	glViewport(0, 0, width, height);
	viewMatrix = camera->BuildViewMatrix();

	glUseProgram(0);
}

void Renderer::GenerateBuildingNode(float height, float x, float y) {
	wall = Mesh::GenerateBuildingWall(height);
	wall->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"random_vecs.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	wall->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"flowing_noise.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	wall->SetExtraTexture(SOIL_load_OGL_texture(TEXTUREDIR"numbers_atlas.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	glBindTexture(GL_TEXTURE_2D, wall->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, wall->GetBumpMap());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, wall->GetExtraTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	SetTextureRepeating(wall->GetBumpMap(), true);
	SetTextureRepeating(wall->GetTexture(), true);
	SetTextureRepeating(wall->GetExtraTexture(), true);

	matrixBuilding = new SceneNode();

	matrixBuildingWall = new SceneNode(wall);
	matrixBuildingWall->SetShader(matrixShader);
	matrixBuildingWall->SetLight(matrixLight);
	matrixBuildingWall->AddUniformi(new Uniformi("numTex", 0));
	matrixBuildingWall->AddUniformi(new Uniformi("lightTex", 1));
	matrixBuildingWall->AddUniformi(new Uniformi("fontTex", 2));
	matrixBuildingWall->AddUniformf(new Uniformf("time", &time));
	matrixBuildingWall->AddUniformf(new Uniformf("numTexSize", 0.015625f));
	matrixBuildingWall->AddUniformf(new Uniformf("lightTexSize", 0.00390625f));
	matrixBuildingWall->AddUniformf(new Uniformf("texelSize", 0.015f));
	matrixBuildingWall->AddUniformf(new Uniformf("speed", 10.0f));
	matrixBuildingWall->AddUniformf(new Uniformf("randomSeed", rand()/10000.0f));

	matrixBuilding->AddChild(matrixBuildingWall);

	roof = Mesh::GenerateBuildingRoof();
	matrixBuildingRoof = new SceneNode(roof);
	matrixBuildingRoof->SetShader(debugShader);
	matrixBuildingRoof->SetTransform(Matrix4::Translation(Vector3(0, height, 0)));

	matrixBuilding->AddChild(matrixBuildingRoof);

	matrixBuilding->SetTransform(Matrix4::Translation(Vector3(x,0,y))*Matrix4::Scale(Vector3(200.0f, 200.0f, 200.0f)));

	matrixScene->AddChild(matrixBuilding);
}

void Renderer::DrawFrameRate() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	SetCurrentShader(texturedShader);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	DrawText(fps.str(), fpsFont, Vector3(0, 0, 0), 25.0f);

	glUseProgram(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::DrawDebugCoord() {
	SetCurrentShader(debugShader);

	//glLineWidth(10.0f);
	modelMatrix = Matrix4::Scale(Vector3(5000.0f, 5000.0f, 5000.0f));

	UpdateShaderMatrices();

	debugCoord->Draw();

	glUseProgram(0);
}

void Renderer::DrawText(const std::string &text,Font* font, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *font);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		glClear(GL_DEPTH_BUFFER_BIT);
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	if (!perspective) {
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
			(float)width / (float)height, 45.0f);
	}

	delete mesh; //Once it's drawn, we don't need it anymore!
}

//TODO: find a way to determine if draw a huge object like skybox.
void Renderer::BuildNodeLists(SceneNode * from) {
	/*if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() -
			camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}*/
	Vector3 dir = from->GetWorldTransform().GetPositionVector() -
		camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));

	if (from->GetColour().w < 1.0f) {
		transparentNodeList.push_back(from);
	}
	else {
		nodeList.push_back(from);
	}

	for (vector < SceneNode * >::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));

	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(),
		transparentNodeList.end(),
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes() {
	for (vector < SceneNode * >::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector <SceneNode *>::const_reverse_iterator i =
		transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));

	}
}

//set shader, light, uniform, openGL triggler and draw node.
void Renderer::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		if (n->GetShader()&&!shadowOnDraw) {
			SetCurrentShader(n->GetShader());
		}

		if (n->GetLight()) {
			SetShaderLight(*n->GetLight());
		}

		if (!n->Uniform3fvIsEmpty()) {
			for (auto i=n->GetUniform3fvIteratorStart(); i!=n->GetUniform3fvIteratorEnd();++i) {
				glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
					(*i)->GetName().c_str()), 1, (*i)->GetValue());
			}
		}

		if (!n->UniformfIsEmpty()) {
			for (auto i = n->GetUniformfIteratorStart(); i != n->GetUniformfIteratorEnd(); ++i) {
				glUniform1f(glGetUniformLocation(currentShader->GetProgram(),
					(*i)->GetName().c_str()), (*i)->GetValue());
			}
		}

		if (!n->UniformiIsEmpty()) {
			for (auto i=n->GetUniformiIteratorStart();i!=n->GetUniformiIteratorEnd();++i) {
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
					(*i)->GetName().c_str()), (*i)->GetValue());
			}
		}

		if (shadowOnDraw)
		{
			modelMatrix = n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale());
			Matrix4 tempMatrix = textureMatrix * modelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
				, "textureMatrix"), 1, false, *& tempMatrix.values);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram()
				, "modelMatrix"), 1, false, *& modelMatrix.values);
		}
		else
		{
			modelMatrix = n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale());

			UpdateShaderMatrices();
		}
		

		if (n->BeforeDrawCall) {
			n->BeforeDrawCall();
		}

		n->Draw();
		
		if (n->AfterDrawCall) {
			n->AfterDrawCall();
		}

		if (!shadowOnDraw) {
			glUseProgram(0);
		}
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}
