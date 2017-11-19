#pragma once
#include <windows.h>
#include "OGLRenderer.h"
#include "glut.h"

#define MAX_CHAR 128

class Printer {
public:
	Printer();
	~Printer();
	void setPosition(Vector2 pos) { position = pos; }
	void setColour(Vector4 color) { colour = color; }
	void print(const char* str);
protected:
	GLuint lists;
	bool init;
	Vector2 position;
	Vector4 colour;
};