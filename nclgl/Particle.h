#include "Mesh.h"

class Particle :public Mesh
{
public:
	Particle(Mesh* mesh,float lifeTime);
	~Particle();

	const float GetLifeTime() { return lifeTime; }

	void Update(float msec);

private:
	float lifeTime;

	Mesh* mesh;
};
