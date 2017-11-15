#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera();
	heightMap = new HeightMap("../../Textures/terrain.raw");
	quad = Mesh::GenerateQuad();
	bubbleSphere = Mesh::GenerateSphere(40,20);

	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f,
		500.0f, RAW_WIDTH * HEIGHTMAP_X));

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	bubbleShader = new Shader(SHADERDIR"bubbleVertex.glsl",
		SHADERDIR"bubbleFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl",
		SHADERDIR"skyboxFragment.glsl");
	lightShader = new Shader("../../Shaders/PerPixelVertex.glsl",
		"../../Shaders/PerPixelFragment.glsl");

	if (!bubbleShader->LinkProgram() || !lightShader->LinkProgram() ||
		!skyboxShader->LinkProgram()) {

		return;
	}

	heightMap->SetTexture(SOIL_load_OGL_texture(
		"../../Textures/Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		"../../Textures/Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	cubeMap = SOIL_load_OGL_cubemap(
		"../../Textures/skybox/blood_ft.tga", "../../Textures/skybox/blood_bk.tga",
		"../../Textures/skybox/blood_up.tga", "../../Textures/skybox/blood_dn.tga",
		"../../Textures/skybox/blood_rt.tga", "../../Textures/skybox/blood_lf.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {

		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);
	init = true;
	waterRotate = 0.0f;

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
	waterRotate += msec / 1000.0f;
}
void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightmap();
	DrawBubbleSphere();
	//DrawWater();

	SwapBuffers();
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

	modelMatrix.ToIdentity();

	UpdateShaderMatrices();
	bubbleSphere->Draw();

	glUseProgram(0);
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
