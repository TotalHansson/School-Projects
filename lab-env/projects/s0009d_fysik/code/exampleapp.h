#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include "MathLib3D.h"
#include "MeshResource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "GraphicsNode.h"
#include "PointLight.h"
#include "physics.h"
#include "gameObject.h"
#include "lineSweep.h"
#include "GJK.h"

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

	void DrawAABB(GameObject *);
	void DrawOBB(GameObject *);
	void DrawGrid();

	void RenderUI();
	void ShootRay();

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
	float posZ = 10.0f;
	float mouseSpeed = 0.4f;

	float hCameraAngle = 10.0f;
	float vCameraAngle = -30.0f;
	Vector3D cameraPosition;
	Vector3D cameraLookAt;


	// LineSweep *ls;

	Ray *ray = nullptr;
	GameObject *go;

	std::vector<GameObject*> gameObjects;

	Vector3D intersectionPoint;
	GraphicsNode *intersectMarker;

	Matrix3D axisTransform;
	GraphicsNode *axisArrows;

	Matrix3D camera;
	Matrix3D projection;

	GraphicsNode gn[10];

	PointLight light;
	GraphicsNode *lightModel;
	Matrix3D lightTransform;


	double dt = 0; 		// Total frame time (includes waiting for vSync)
	double dt2 = 0; 	// Code execution time
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::time_point<std::chrono::system_clock> start2, end2;
};
} // namespace Example