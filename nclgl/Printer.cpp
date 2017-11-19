#include "Printer.h"

Printer::Printer() {
	position = Vector2(0.0f, 0.0f);
	colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

Printer::~Printer() {
	glDeleteLists(lists, MAX_CHAR);
}

void Printer::print(const char* str) {
	glColor4fv((float *) &colour);
	glRasterPos2fv((float *) &position);

	for(;*str!='\0';++str)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str);
}
