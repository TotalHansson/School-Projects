#pragma once 
#include <map>
#include <iostream>
#include <fstream>
#include <string>

#ifndef GL_INCLUDE
#define GL_INCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"

class ShaderResource
{
private:
	GLchar* vs;
	GLchar* fs;
	std::string vsString;
	std::string fsString;

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	GLint shaderLogSize;

	Matrix matrix;


	std::map<std::string, GLuint> uniformLocations;
	std::map<std::string, GLuint>::iterator it;


public:

	ShaderResource();
	~ShaderResource();

	void LoadVertexShader(const char* filename);
	void LoadFragmentShader(const char* filename);
	void LinkProgram();

	void UseProgram();
	void GetHandles();

	void ModifyMatrix4vf(Matrix matrix, std::string name);
	void ModifyVector3fv(Vec3 vector, std::string name);
	void ModifyFloat(float f, std::string name);
};