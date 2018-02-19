#pragma once

#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <iostream>

#ifndef GL_INCLUDE
#define GL_INCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"
#include "textureResource.h"

class MeshResource
{
private:
	std::vector<Vec3> index;
	unsigned short indexCount;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	GLuint elementBuffer;
	float* softwareVertexBuffer;
	int vertexBufferSize;
	int* softwareIndexBuffer;
	int indexBufferSize;

public:
	MeshResource();
	~MeshResource();

	float* GetVertexBuffer();
	int GetVertexBufferSize();
	int* GetIndexBuffer();
	int GetIndexBufferSize();
	
	void LoadOBJ(
		const char* filePath,
		std::vector<float> &completeBuffer,
		std::vector<int> &indices);

	// buf = x, y, z, u, v, xn, yn, zn
	void LoadMeshFromFile(const char* filename);
	void GenBuffers(float* buf, int bufSize, int* indices, int indicesSize);
	void Render();


	template <typename T, size_t N>
	inline
		size_t SizeOfArray(const T(&)[N])
	{
		return N;
	}
};