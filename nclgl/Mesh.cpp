#include "Mesh.h"

#ifndef PI
#define PI 3.1415926
#endif
#ifndef D_PI
#define D_PI 6.2831853 //2*PI
#endif // !D_PI


Mesh::Mesh(void) {
	for (int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;

	texture = 0;
	textureCoords = NULL;

	indices = NULL;
	numIndices = 0;

	normals = NULL;

	tangents = NULL;
	bumpTexture = 0;
	extraTexture = 0;
}

Mesh::~Mesh(void) {
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;

	glDeleteTextures(1, &texture);
	delete[] textureCoords;

	delete[] indices;

	delete[] normals;

	delete[] tangents;
	glDeleteTextures(1, &bumpTexture); // Just like the texture map ...
}

Mesh* Mesh::GenerateDebugCoord() {
	Mesh* m = new Mesh();
	m->numVertices = 6;
	m->type = GL_LINES;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[1] = Vector3(1.0f, 0.0f, 0.0f);

	m->vertices[2] = Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[3] = Vector3(0.0f, 1.0f, 0.0f);

	m->vertices[4] = Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[5] = Vector3(0.0f, 0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	m->colours[2] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[3] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

	m->colours[4] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[5] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateRainDrop(float length) {
	Mesh* m = new Mesh();
	m->numVertices = 2;
	m->type = GL_LINES;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[1] = Vector3(0.0f, length, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m->colours[1] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateTriangle() {
	Mesh* m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh * Mesh::GenerateQuad() {
	/*Mesh * m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
	m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	m->BufferData();
	return m;*/
	Mesh * m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	m->normals = new Vector3[m->numVertices];
	m->tangents = new Vector3[m->numVertices];

	m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		m->normals[i] = Vector3(0.0f, 0.0f, -1.0f);
		m->tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
	}
	m->BufferData();

	return m;
}

Mesh* Mesh::GenerateBuildingRoof() {
	Mesh * m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	m->normals = new Vector3[m->numVertices];
	m->tangents = new Vector3[m->numVertices];

	m->vertices[0] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[1] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[2] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[3] = Vector3(0.5f, 0.0f, 0.5f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		m->normals[i] = Vector3(0.0f, 1.0f, 0.0f);
		m->tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
	}
	m->BufferData();

	return m;
}

Mesh* Mesh::GenerateBuildingWall(float height) {
	/*Mesh* m = new Mesh();
	m->numVertices = 10;
	m->numIndices = 24;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[1] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[2] = Vector3(0.5f, 0.0f, 0.5f);
	m->vertices[3] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[4] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[5] = Vector3(-0.5f, height, -0.5f);
	m->vertices[6] = Vector3(-0.5f, height, 0.5f);
	m->vertices[7] = Vector3(0.5f, height, 0.5f);
	m->vertices[8] = Vector3(0.5f, height, -0.5f);
	m->vertices[9] = Vector3(-0.5f, height, -0.5f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.0f, height*0.25);
	m->textureCoords[1] = Vector2(0.25f, height*0.25);
	m->textureCoords[2] = Vector2(0.5f, height*0.25);
	m->textureCoords[3] = Vector2(0.75f, height*0.25);
	m->textureCoords[4] = Vector2(1.0f, height*0.25);
	m->textureCoords[5] = Vector2(0.0f, 0.0f);
	m->textureCoords[6] = Vector2(0.25f, 0.0f);
	m->textureCoords[7] = Vector2(0.5f, 0.0f);
	m->textureCoords[8] = Vector2(0.75f, 0.0f);
	m->textureCoords[9] = Vector2(1.0f, 0.0f);

	m->normals = new Vector3[m->numVertices];
	m->normals[0] = Vector3(-1.0f,0,-1.0f);
	m->normals[1] = Vector3(-1.0f, 0, 1.0f);
	m->normals[2] = Vector3(1.0f, 0, 1.0f);
	m->normals[3] = Vector3(1.0f, 0, -1.0f);
	m->normals[4] = Vector3(-1.0f, 0, -1.0f);
	m->normals[5] = Vector3(-1.0f, 0, -1.0f);
	m->normals[6] = Vector3(-1.0f, 0, 1.0f);
	m->normals[7] = Vector3(1.0f, 0, 1.0f);
	m->normals[8] = Vector3(1.0f, 0, -1.0f);
	m->normals[9] = Vector3(-1.0f, 0, -1.0f);

	m->indices = new GLuint[m->numIndices];

	unsigned index = 0;

	auto addIndex = [&index,&m](GLuint x, GLuint y, GLuint z ,GLuint w) {
		m->indices[index++] = x;
		m->indices[index++] = y;
		m->indices[index++] = z;

		m->indices[index++] = y;
		m->indices[index++] = z;
		m->indices[index++] = w;
	};

	addIndex(0, 1, 5, 6);
	addIndex(1, 2, 6, 7);
	addIndex(2, 3, 7, 8);
	addIndex(3, 4, 8, 9);

	m->BufferData();
	return m;*/
	Mesh* m = new Mesh();
	m->numVertices = 16;
	m->numIndices = 24;
	m->type = GL_TRIANGLES;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[1] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[2] = Vector3(-0.5f, height, -0.5f);
	m->vertices[3] = Vector3(-0.5f, height, 0.5f);

	m->vertices[4] = Vector3(-0.5f, 0.0f, 0.5f);
	m->vertices[5] = Vector3(0.5f, 0.0f, 0.5f);;
	m->vertices[6] = Vector3(-0.5f, height, 0.5f);
	m->vertices[7] = Vector3(0.5f, height, 0.5f);

	m->vertices[8] = Vector3(0.5f, 0.0f, 0.5f);
	m->vertices[9] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[10] = Vector3(0.5f, height, 0.5f);
	m->vertices[11] = Vector3(0.5f, height, -0.5f);

	m->vertices[12] = Vector3(0.5f, 0.0f, -0.5f);
	m->vertices[13] = Vector3(-0.5f, 0.0f, -0.5f);
	m->vertices[14] = Vector3(0.5f, height, -0.5f);
	m->vertices[15] = Vector3(-0.5f, height, -0.5f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.0f, height*0.25);
	m->textureCoords[1] = Vector2(0.25f, height*0.25);
	m->textureCoords[2] = Vector2(0.0f, 0.0f);
	m->textureCoords[3] = Vector2(0.25f, 0.0f);

	m->textureCoords[4] = Vector2(0.25f, height*0.25);
	m->textureCoords[5] = Vector2(0.5f, height*0.25);
	m->textureCoords[6] = Vector2(0.25f, 0.0f);
	m->textureCoords[7] = Vector2(0.5f, 0.0f);

	m->textureCoords[8] = Vector2(0.5f, height*0.25);
	m->textureCoords[9] = Vector2(0.75f, height*0.25);
	m->textureCoords[10] = Vector2(0.5f, 0.0f);
	m->textureCoords[11] = Vector2(0.75f, 0.0f);

	m->textureCoords[12] = Vector2(0.75f, height*0.25);
	m->textureCoords[13] = Vector2(1.0f, height*0.25);
	m->textureCoords[14] = Vector2(0.75f, 0.0f);
	m->textureCoords[15] = Vector2(1.0f, 0.0f);

	m->normals = new Vector3[m->numVertices];
	m->normals[0] = Vector3(-1.0f, 0, 0);
	m->normals[1] = Vector3(-1.0f, 0, 0);
	m->normals[2] = Vector3(-1.0f, 0, 0);
	m->normals[3] = Vector3(-1.0f, 0, 0);

	m->normals[4] = Vector3(0, 0, 1.0f);
	m->normals[5] = Vector3(0, 0, 1.0f);
	m->normals[6] = Vector3(0, 0, 1.0f);
	m->normals[7] = Vector3(0, 0, 1.0f);

	m->normals[8] = Vector3(1.0f, 0, 0);
	m->normals[9] = Vector3(1.0f, 0, 0);
	m->normals[10] = Vector3(1.0f, 0, 0);
	m->normals[11] = Vector3(1.0f, 0, 0);

	m->normals[12] = Vector3(0, 0, -1.0f);
	m->normals[13] = Vector3(0, 0, -1.0f);
	m->normals[14] = Vector3(0, 0, -1.0f);
	m->normals[15] = Vector3(0, 0, -1.0f);

	m->indices = new GLuint[m->numIndices];

	unsigned index = 0;

	auto addIndex = [&index, &m](GLuint x, GLuint y, GLuint z, GLuint w) {
		m->indices[index++] = x;
		m->indices[index++] = y;
		m->indices[index++] = z;

		m->indices[index++] = y;
		m->indices[index++] = z;
		m->indices[index++] = w;
	};

	addIndex(0, 1, 2, 3);
	addIndex(4, 5, 6, 7);
	addIndex(8, 9, 10, 11);
	addIndex(12, 13, 14, 15);

	/*for(unsigned i=0;i<m->numIndices;++i)
	{
		Vector3 t = m->normals[m->indices[i]];
		std::cout << i << ": " << t.x << ", " << t.y << ", " <<t.z<< std::endl;
	}*/

	m->BufferData();
	return m;
}

//subdivision_m, subdivision_n: how many vertices in each direction.
Mesh* Mesh::GenerateSphere(unsigned subdivision_m, unsigned subdivision_n) {
	float addAngleZ = PI / subdivision_m;
	float addAngleXY = D_PI / subdivision_n;

	float angleZ = 0;
	float angleXY = 0;

	Mesh* m = new Mesh();

	m->numVertices = (subdivision_m + 1)*subdivision_n * 4;
	m->numIndices = (subdivision_m + 1)*subdivision_n * 6;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];
	m->normals = new Vector3[m->numVertices];
	m->indices = new GLuint[m->numIndices];

	unsigned index = 0;
	for (unsigned i = 0; i < (subdivision_m + 1); i++)
	{
		for (unsigned j = 0; j < subdivision_n; j++)
		{
			m->vertices[index] = Vector3(sin(angleZ)*cos(angleXY), sin(angleZ)*sin(angleXY), cos(angleZ));
			m->textureCoords[index] = Vector2(angleZ / PI, angleXY / D_PI);
			m->colours[index] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			m->normals[index] = m->vertices[index];

			++index;

			angleXY += addAngleXY;
		}
		angleZ += addAngleZ;
	}

	index = 0;
	for (unsigned i = 0; i < (subdivision_m + 1); i++)
	{
		for (unsigned j = 0; j < subdivision_n; j++)
		{
			m->indices[index++] = i*subdivision_m + j;
			m->indices[index++] = (i + 1)*subdivision_m + j;
			m->indices[index++] = (i + 1)*subdivision_m + (j + 1) % subdivision_n;

			m->indices[index++] = i*subdivision_m + j;
			m->indices[index++] = (i + 1)*subdivision_m + (j + 1) % subdivision_n;
			m->indices[index++] = i*subdivision_m + (j + 1) % subdivision_n;
		}
	}

	m->BufferData();
	return m;
}

void Mesh::GenerateNormals() {
	if (!normals) {
		normals = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i] = Vector3();

	}
	if (indices) { // Generate per - vertex normals
		for (GLuint i = 0; i < numIndices; i += 3) {
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross(
				(vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else { // It ¡¯s just a list of triangles , so generate face normals
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3 & a = vertices[i];
			Vector3 & b = vertices[i + 1];
			Vector3 & c = vertices[i + 2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i + 1] = normal;
			normals[i + 2] = normal;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i].Normalise();
	}
}

void Mesh::GenerateTangents() {
	if (!tangents) {
		tangents = new Vector3[numVertices];
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		tangents[i] = Vector3();
	}

	if (indices) {
		for (GLuint i = 0; i < numIndices; i += 3) {
			int a = indices[i];
			int b = indices[i + 1];
			int c = indices[i + 2];

			Vector3 tangent = GenerateTangent(vertices[a], vertices[b],
				vertices[c], textureCoords[a],
				textureCoords[b], textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else {
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3 tangent = GenerateTangent(vertices[i], vertices[i + 1],
				vertices[i + 2], textureCoords[i],
				textureCoords[i + 1], textureCoords[i + 2]);

			tangents[i] += tangent;
			tangents[i + 1] += tangent;
			tangents[i + 2] += tangent;
		}
	}
	for (GLuint i = 0; i < numVertices; ++i) {
		tangents[i].Normalise();
	}
}

Vector3 Mesh::GenerateTangent(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector2 & ta, const Vector2 & tb, const Vector2 & tc) {
	Vector2 coord1 = tb - ta;
	Vector2 coord2 = tc - ta;

	Vector3 vertex1 = b - a;
	Vector3 vertex2 = c - a;

	Vector3 axis = Vector3(vertex1 * coord2.y - vertex2 * coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}


void Mesh::BufferData() {
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2),
			textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours) { // Just in case the data has no colour attribute ...
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4),
			colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	if (normals) {
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
			normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	if (tangents) {
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
			tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	if (indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
			bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint),
			indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void Mesh::Draw() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, extraTexture);

	glBindVertexArray(arrayObject);
	if (bufferObject[INDEX_BUFFER]) {// Added by the index buffers tut ...
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray(0);
}