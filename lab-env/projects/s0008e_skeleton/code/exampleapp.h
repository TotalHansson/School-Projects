#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include "MeshResource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "GraphicsNode.h"
#include "PointLight.h"
#include "Skeleton.h"

#include <vector>
#include <chrono>

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open();
	/// run app
	void Run();
private:

	void renderUI();

	GLuint program;
	GLuint vertexShader;
	GLuint pixelShader;
	GLuint triangle;
	Display::Window* window;

	int drawMode = 0;
	bool lClicking = false;
	bool rClicking = false;
	Matrix3D tmpRotation;
	int oldX = 0;
	int oldY = 0;
	float posZ = 5;
	float mouseSpeed = 0.02f;

	float cameraAngle = 0.0f;


	Matrix3D camera;
	PointLight light;

	Skeleton s;
	float animSpeed = 1.0f;
	bool runAnim = false;
	bool bindPose = true;

	double dt = 0;
	std::chrono::time_point<std::chrono::system_clock> start, end;
};
} // namespace Example