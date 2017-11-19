#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	frameCounter = 0;
	sprintf_s(fps, "%s", "0 fps");

	camera = new Camera();
	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		500.0f, RAW_WIDTH * HEIGHTMAP_X));

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	debugCoord = Mesh::GenerateDebugCoord();

	debugShader = new Shader(SHADERDIR"debugVertex.glsl",
		SHADERDIR"debugFragment.glsl");
	fpsShader = new Shader(SHADERDIR"fontVertex.glsl",
		SHADERDIR"debugFragment.glsl");
	bubbleShader = new Shader(SHADERDIR"bubbleVertex.glsl",
		SHADERDIR"bubbleFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader("../../Shaders/PerPixelVertex.glsl",
		"../../Shaders/PerPixelFragment.glsl");

	mountainScene = new SceneNode();

	heightMap = new HeightMap("../../Textures/terrain.raw");
	mountain = new SceneNode(heightMap);
	mountain->SetShader(lightShader);
	mountain->SetLight(light);
	mountain->AddUniformfv(new Uniformfv("", (float *)& camera->GetPosition()));
	mountainScene->AddChild(mountain);

	quad = Mesh::GenerateQuad();
	mountainSky = new SceneNode(quad);
	mountainSky->SetShader(skyboxShader);
	mountainSky->SetSkybox(true);
	mountainScene->AddChild(mountainSky);

	bubbleSphere = Mesh::GenerateSphere(80, 40);
	mountainEnviroSphere = new SceneNode(bubbleSphere);
	mountainEnviroSphere->SetModelScale(Vector3(500.0f, 500.0f, 500.0f));
	mountainEnviroSphere->SetTransform(Matrix4::Translation(Vector3(1000.0f, 2000.0f, 1000.0f)));
	mountainEnviroSphere->SetShader(bubbleShader);
	mountainScene->AddChild(mountainEnviroSphere);

	if (!debugShader->LinkProgram() || !bubbleShader->LinkProgram() || !lightShader->LinkProgram() || !skyboxShader->LinkProgram()) {
		return;
	}

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
	init = true;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete bubbleSphere;
	delete skyboxShader;
	delete lightShader;
	delete light;
	currentShader = 0;

}
void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameCounter = (frameCounter+1) % 60;
	if (frameCounter == 0) {
		sprintf_s(fps, "%d%s", (int)(1000/msec)," fps");
	}
}
void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawDebugCoord();
	DrawHeightmap();
	DrawBubbleSphere();
	DrawFrameRate();

	SwapBuffers();
}

void Renderer::DrawFrameRate() {
	SetCurrentShader(debugShader);

	UpdateShaderMatrices();

	printer.setPosition(Vector2(1.0f, 1.0f));
	printer.setColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	printer.print(fps);

	glUseProgram(0);
}

void Renderer::DrawDebugCoord() {
	SetCurrentShader(debugShader);

	//glLineWidth(10.0f);
	modelMatrix = Matrix4::Scale(Vector3(5000.0f, 5000.0f, 5000.0f));

	UpdateShaderMatrices();

	debugCoord->Draw();

	glUseProgram(0);
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawBubbleSphere() {
	SetCurrentShader(bubbleShader);

	UpdateShaderMatrices();

	bubbleSphere->Draw();

	glUseProgram(0);

	/*SetCurrentShader(debugShader);

	modelMatrix = Matrix4::Scale(Vector3(50.0f, 50.0f, 50.0f));

	UpdateShaderMatrices();

	blackHole.Draw();

	glUseProgram(0);*/
}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();

	glUseProgram(0);
}
