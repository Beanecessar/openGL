#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "Uniform.h"
#include <vector>

class SceneNode {
public:
	void (*BeforeDrawCall) ();
	void (*AfterDrawCall) ();

	SceneNode(Mesh * m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	void SetTransform(const Matrix4 & matrix) { transform = matrix; }
	const Matrix4 & GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	bool IsSkybox() const { return isSkybox; }
	void SetSkybox(bool b) { isSkybox = b; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* s) { shader = s; }

	Light* GetLight() const { return light; }
	void SetLight(Light* l) { light = l; }

	void AddUniform3fv(Uniform3fv* ufv) {
		uniform3fv.push_back(ufv);
	}
	std::vector <Uniform3fv *>::const_iterator GetUniform3fvIteratorStart() {
		return uniform3fv.begin();
	}
	std::vector <Uniform3fv *>::const_iterator GetUniform3fvIteratorEnd() {
		return uniform3fv.end();
	}
	bool Uniform3fvIsEmpty() {
		return uniform3fv.empty();
	}

	void AddUniformf(Uniformf* uf) {
		uniformf.push_back(uf);
	}
	std::vector <Uniformf *>::const_iterator GetUniformfIteratorStart() {
		return uniformf.begin();
	}
	std::vector <Uniformf *>::const_iterator GetUniformfIteratorEnd() {
		return uniformf.end();
	}
	bool UniformfIsEmpty() {
		return uniformf.empty();
	}

	void AddUniformi(Uniformi* ui) {
		uniformi.push_back(ui);
	}
	std::vector <Uniformi *>::const_iterator GetUniformiIteratorStart() {
		return uniformi.begin();
	}
	std::vector <Uniformi *>::const_iterator GetUniformiIteratorEnd() {
		return uniformi.end();
	}
	bool UniformiIsEmpty() {
		return uniformi.empty();
	}

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh * GetMesh() const { return mesh; }
	void SetMesh(Mesh * m) { mesh = m; }

	void AddChild(SceneNode * s);

	virtual void Update(float msec);
	virtual void Draw();

	std::vector <SceneNode *>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}
	std::vector <SceneNode *>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	static bool CompareByCameraDistance(SceneNode *a, SceneNode * b) {
		if (a->IsSkybox())
			return true;
		if (b->IsSkybox())
			return false;
		return (a->distanceFromCamera <
			b->distanceFromCamera) ? true : false;
	}
protected:
	SceneNode * parent;
	Mesh * mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	Shader * shader;
	Light * light;
	std::vector <SceneNode *> children;
	float distanceFromCamera;
	float boundingRadius;
	std::vector<Uniform3fv *> uniform3fv;
	std::vector<Uniformf *> uniformf;
	std::vector<Uniformi *> uniformi;
	bool isSkybox;
};
