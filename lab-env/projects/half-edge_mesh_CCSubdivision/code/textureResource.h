#pragma once
#include "stb_image.h"

#ifndef GL_INCLUDE
#define GL_INCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

class TextureResource
{
private:
	char* filename;
	unsigned char* data;
	GLuint texture;
	int x;
	int y;
	int n;


public:

	TextureResource();
	~TextureResource();

	void LoadFromFile(const char* filename);
	void BindTexture();
	void ApplyTexture(const unsigned char* texture, const int x, const int y);
	unsigned char* GetTexture();

	int GetWidth();
	int GetHeight();
};