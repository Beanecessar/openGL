#include "Particle.h"

Particle::Particle(Mesh* mesh,float lifeTime)
{
	this->mesh = mesh;
	this->lifeTime = lifeTime;
}

Particle::~Particle()
{
	delete mesh;
}

void Particle::Update(float msec) {
	lifeTime -= msec;
}