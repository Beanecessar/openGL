#include "SceneNode.h"

SceneNode::SceneNode(Mesh * mesh, Vector4 colour) {
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;
	light = NULL;
	shader = NULL;
	modelScale = Vector3(1, 1, 1);
	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
	isSkybox = false;
}

SceneNode::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
	for (unsigned int i = 0; i < uniformfv.size(); ++i) {
		delete uniformfv[i];
	}
	for (unsigned int i = 0; i < uniformi.size(); ++i) {
		delete uniformi[i];
	}
	delete shader;
	delete light;
}

void SceneNode::AddChild(SceneNode * s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::Draw() {
	if (mesh) { mesh->Draw(); };
}

void SceneNode::Update(float msec) {
	if (parent) { // This node has a parent ...
		worldTransform = parent -> worldTransform * transform;
	}
	else { // Root node , world transform is local transform !
		worldTransform = transform;
	}
	for (vector < SceneNode * >::iterator i = children.begin();
		i != children.end(); ++i) {
		(*i) -> Update(msec);
	}
}