#pragma once 
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "MathLib3D.h"

class ShaderResource
{
private:
	std::string vsString;
	std::string fsString;

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	GLint shaderLogSize;

	std::map<std::string, GLuint> uniformLocations;
	std::map<std::string, GLuint>::iterator it;


public:

	ShaderResource()
	{
	}

	void loadVertexShader(const char* filename)
	{
		// Read vertexShader from file
		std::ifstream in(filename, std::ios::in | std::ios::binary);
		std::string contents;

		if (in.is_open())
		{
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
		}
		else
		{
			std::cout << "Error reading vertex shader\n";
		}

		const GLchar* shaderString = contents.c_str();

		// Create vertexShader
		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = sizeof(shaderString);
		glShaderSource(this->vertexShader, 1, &shaderString, NULL);
		glCompileShader(this->vertexShader);

		// Get error log
		glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
			printf("[VERTEX SHADER COMPILE ERROR]: %s\n", buf);
			delete[] buf;
		}
		
	}

	void loadFragmentShader(const char* filename)
	{
		std::ifstream in(filename);
		std::string contents;
		
		if (in.is_open())
		{
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
		}

		else
		{
			std::cout << "Error reading fragment shader\n";
		}

		const GLchar* shaderString = contents.c_str();

		// Create fragmentShader
		this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLint length = sizeof(shaderString);
		glShaderSource(this->fragmentShader, 1, &shaderString, NULL); //&length
		glCompileShader(this->fragmentShader);

		// Get error log
		glGetShaderiv(this->fragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->fragmentShader, shaderLogSize, NULL, buf);
			printf("[FRAGMENT SHADER COMPILE ERROR]: %s\n", buf);
			delete[] buf;
		}
		
	}

	void linkProgram()
	{
		// link the shaders to the program
		this->program = glCreateProgram();
		glAttachShader(this->program, this->vertexShader);
		glAttachShader(this->program, this->fragmentShader);
		glLinkProgram(this->program);

		// Get any error
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
			printf("[PROGRAM LINK ERROR]: %s\n", buf);
			delete[] buf;
		}
	}

	void useProgram()
	{
		glUseProgram(this->program);
	}

	void getHandles()
	{	
		GLuint handle;

		// Camera and model
		handle = glGetUniformLocation(program, "MVP");
		uniformLocations.insert(std::pair<std::string, GLuint>("MVP", handle));
		handle = glGetUniformLocation(program, "V");
		uniformLocations.insert(std::pair<std::string, GLuint>("V", handle));
		handle = glGetUniformLocation(program, "M");
		uniformLocations.insert(std::pair<std::string, GLuint>("M", handle));
		handle = glGetUniformLocation(program, "MV");
		uniformLocations.insert(std::pair<std::string, GLuint>("MV", handle));


		// Light
		handle = glGetUniformLocation(program, "lightPosition_worldspace");
		uniformLocations.insert(std::pair<std::string, GLuint>("lightPosition_worldspace", handle));
		handle = glGetUniformLocation(program, "ambientColor");
		uniformLocations.insert(std::pair<std::string, GLuint>("ambientColor", handle));
		handle = glGetUniformLocation(program, "diffuseColor");
		uniformLocations.insert(std::pair<std::string, GLuint>("diffuseColor", handle));
		handle = glGetUniformLocation(program, "specularColor");
		uniformLocations.insert(std::pair<std::string, GLuint>("specularColor", handle));
		handle = glGetUniformLocation(program, "lightIntensity");
		uniformLocations.insert(std::pair<std::string, GLuint>("lightIntensity", handle));


		// Texture
		handle = glGetUniformLocation(program, "diffuseTextureSampler");
		uniformLocations.insert(std::pair<std::string, GLuint>("diffuseTextureSampler", handle));
		handle = glGetUniformLocation(program, "normalTextureSampler");
		uniformLocations.insert(std::pair<std::string, GLuint>("normalTextureSampler", handle));
		handle = glGetUniformLocation(program, "specularTextureSampler");
		uniformLocations.insert(std::pair<std::string, GLuint>("specularTextureSampler", handle));
		

		// Rendering
		handle = glGetUniformLocation(program, "renderMode");
		uniformLocations.insert(std::pair<std::string, GLuint>("renderMode", handle));
		handle = glGetUniformLocation(program, "selected");
		uniformLocations.insert(std::pair<std::string, GLuint>("selected", handle));


		// Joints
		handle = glGetUniformLocation(program, "multipliedJoints");
		uniformLocations.insert(std::pair<std::string, GLuint>("multipliedJoints", handle));
	}



	void addMatrixArray(float *matriceData, int numJoints, std::string name)
	{
		this->useProgram();
		GLuint matrixLocation = uniformLocations[name];
		glUniformMatrix4fv(matrixLocation, numJoints, GL_TRUE, matriceData);
	}

	void modifyMatrix4vf(Matrix3D matrix, std::string name)
	{
		this->useProgram();
		GLuint matrixLocation = uniformLocations[name];
		glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, matrix.getArray());
	}

	void modifyVector3fv(Vector3D vector, std::string name)
	{
		this->useProgram();
		GLuint vectorLocation = uniformLocations[name];
		glUniform3fv(vectorLocation, 1, vector.getArray());
	}

	void modifyFloat(float f, std::string name)
	{
		this->useProgram();
		GLuint floatLocation = uniformLocations[name];
		glUniform1f(floatLocation, f);
	}

	void modifyInt(int i, std::string name)
	{
		this->useProgram();
		GLuint intLocation = uniformLocations[name];
		glUniform1i(intLocation, i);
	}
};