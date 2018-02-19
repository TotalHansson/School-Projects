#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include <vector>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <chrono>

#include "../../../engine/core/app.h"
#include "../../../engine/render/window.h"
#include "imgui.h"

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"
#include "meshResource.h"
#include "textureResource.h"
#include "shaderResource.h"
#include "pointLight.h"
#include "graphicsNode.h"
#include "half-edge_mesh.h"


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

	void Subdivide();
	void RenderUI();

	Display::Window* window;

	// Controls
	int drawMode = 0;
	bool clicking = false;
	Matrix objTransform;
	int oldX = 0;
	int oldY = 0;
	float cameraDistance = 0;
	float mouseSpeed = 0.25f;

	Matrix autoRotateDir;
	bool autoRotate = false;

	std::string textOutput;

	int nrFaces = 1;

	GraphicsNode* gn;
	PointLight light;
};

} // namespace Example