#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light(Vector3 position, Vector4 colour, float radius,Vector3* direction=NULL,float angle=0) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->direction = direction;
		this->angle = angle;
	}

	~Light(void) {
	};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	Vector3* GetDirection() const { return direction; }
	void SetPosition(Vector3* val) { direction = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	float GetAngle() const { return angle; }
	void SetAngle(float val) { angle = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

protected:
	Vector3 position;
	Vector3* direction;
	Vector4 colour;
	float radius;	float angle;};