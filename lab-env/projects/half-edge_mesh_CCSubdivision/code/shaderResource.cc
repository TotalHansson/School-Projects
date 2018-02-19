#include "shaderResource.h"

ShaderResource::ShaderResource()
{}

ShaderResource::~ShaderResource()
{}

void ShaderResource::LoadVertexShader(const char* filename)
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
	
	const GLchar* aaa = contents.c_str();

	// Create vertexShader
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLint length = sizeof(aaa);
	glShaderSource(this->vertexShader, 1, &aaa, NULL);
	glCompileShader(this->vertexShader);

	// get error log
	glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
		printf("[SHADER COMPILE ERROR]: %s\n", buf);
		delete[] buf;
	}
}

void ShaderResource::LoadFragmentShader(const char* filename)
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

	const GLchar* bbb = contents.c_str();

	// Create fragmentShader
	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLint length = sizeof(bbb);
	glShaderSource(this->fragmentShader, 1, &bbb, NULL); //&length
	glCompileShader(this->fragmentShader);

	// get error log
	glGetShaderiv(this->fragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(this->fragmentShader, shaderLogSize, NULL, buf);
		printf("[SHADER COMPILE ERROR]: %s\n", buf);
		delete[] buf;
	}
}

void ShaderResource::LinkProgram()
{
	// link the shaders to the program
	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertexShader);
	glAttachShader(this->program, this->fragmentShader);
	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);

	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s\n", buf);
		delete[] buf;
	}
}

void ShaderResource::UseProgram()
{
	glUseProgram(this->program);
}

void ShaderResource::GetHandles()
{
	GLuint handle;

	handle = glGetUniformLocation(program, "MVP");
	uniformLocations.insert(std::pair<std::string, GLuint>("MVP", handle));

	handle = glGetUniformLocation(program, "V");
	uniformLocations.insert(std::pair<std::string, GLuint>("V", handle));

	handle = glGetUniformLocation(program, "M");
	uniformLocations.insert(std::pair<std::string, GLuint>("M", handle));

	handle = glGetUniformLocation(program, "MV");
	uniformLocations.insert(std::pair<std::string, GLuint>("MV", handle));


	handle = glGetUniformLocation(program, "LightPosition_worldspace");
	uniformLocations.insert(std::pair<std::string, GLuint>("LightPosition_worldspace", handle));
	

	handle = glGetUniformLocation(program, "ambientColor");
	uniformLocations.insert(std::pair<std::string, GLuint>("ambientColor", handle));

	handle = glGetUniformLocation(program, "diffuseColor");
	uniformLocations.insert(std::pair<std::string, GLuint>("diffuseColor", handle));

	handle = glGetUniformLocation(program, "specColor");
	uniformLocations.insert(std::pair<std::string, GLuint>("specColor", handle));
}

void ShaderResource::ModifyMatrix4vf(Matrix matrix, std::string name)
{
	// Apply the transformation
	GLuint matrixLocation = uniformLocations[name];

	float *arr = new float[16];
	Matrix::GetArray(matrix, arr);
	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, arr);
	delete[] arr;
}

void ShaderResource::ModifyVector3fv(Vec3 vector, std::string name)
{
	GLuint vectorLocation = uniformLocations[name];

	float *arr = new float[3];
	Vec3::GetArray(vector, arr);
	glUniform3fv(vectorLocation, 1, arr);
	delete[] arr;
}

void ShaderResource::ModifyFloat(float f, std::string name)
{
	GLuint floatLocation = uniformLocations[name];
	glUniform1f(floatLocation, f);
}