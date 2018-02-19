//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "../../../engine/config.h"
#include "exampleapp.h"
// #include "MathLib3D.h"
// #include "imgui.h"


using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mods)
	{

		// Esc or Space to exit the program
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_SPACE)
		{
			this->window->Close();
		}

		// Z to toggle between full, wireframe, and point render
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			if (this->drawMode == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				this->drawMode = 1;
			}
			else if(this->drawMode == 1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				this->drawMode = 2;
			}
			else if (this->drawMode == 2)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				this->drawMode = 0;
			}
		}
	});

	window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			clicking = true;
			this->autoRotate = false;
		}

		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			clicking = false;
	});

	window->SetMouseScrollFunction([this](double x, double y)
	{
		cameraDistance += y*0.1;
		objTransform[11] = cameraDistance;
	});

	window->SetMouseMoveFunction([this](float64 x, float64 y)
	{
		if (clicking)
		{
			this->objTransform = objTransform * 
							Matrix(Vec3(0,1,0), (x - oldX) * mouseSpeed) *
							Matrix(Vec3(1,0,0), (y - oldY) * mouseSpeed);
		}

		oldX = x;
		oldY = y;
	});



	if (this->window->Open())
	{
		// Set clear color to gray (Background)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// GL_DEPTH_TEST to only draw the faces closest to the camera if there is overlap
		glEnable(GL_DEPTH_TEST);

		glLineWidth(2);
		glPointSize(5);

		// Set light for the scene
		light.position = Vec3(5.0f, 5.0f, 5.0f);
		light.ambientColor = Vec3(0.1f, 0.1f, 0.1f);
		light.diffuseColor = Vec3(0.5f, 0.5f, 0.5f);
		light.specColor = Vec3(1.0f, 1.0f, 1.0f);
		light.specularPower = 50.0f;

		// Create a new Graphics Node
		gn = new GraphicsNode();

		// Set vertex and fragment shaders from files
		gn->SetShader("../resources/shaders/standard.vertexshader",
					  "../resources/shaders/standard.fragmentshader");
		gn->SetHEmesh("../resources/models/quads/cube.obj");
		gn->SetTexture("../resources/textures/polygon.png");
			


		// Set the imgui render function
		this->window->SetUiRender([this]()
		{
			this->RenderUI();
		});

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	Matrix id;
	this->autoRotateDir = Matrix(Vec3(0,1,0), 0.25f);

	while (this->window->IsOpen())
	{
		// Set the polygon mode to the chosen
		if (this->drawMode == 0)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		else if(this->drawMode == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		else if (this->drawMode == 2)
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

		// Reset the canvas to remove last frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();


		// Auto rotation
		if (this->autoRotate)
			objTransform = objTransform * this->autoRotateDir;

	
		// Apply light to object
		gn->SetLight(light);


		// Apply view and projection, sent values to shaders
		gn->Transform(objTransform, id);


		// Send vertices to GPU for rendering
		gn->DrawHE(this->nrFaces);
		

		// Swap back to GL_FILL for UI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Show the rendered frame
		this->window->SwapBuffers();
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Subdivide()
{
	this->textOutput = std::string("Some text");
	gn->SubdivideHEmesh(1);
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::RenderUI()
{
	if(this->window->IsOpen())
	{
		// Create a window
		ImGui::Begin("Controls");
			ImGui::Text("Rotate: LMB + Drag");
			ImGui::Text("Toggle draw mode: Z");
			ImGui::Text("Exit:   Esc/Space");
		ImGui::End();


		ImGui::Begin("Subdivision");
			ImGui::Text("Nr Subdivides: %d\nEdges: %u\nVerts: %u\nFaces: %u",
				gn->GetHEmesh()->nrSubdivs, gn->GetHEmesh()->NrEdges(),
				gn->GetHEmesh()->NrVerts(), gn->GetHEmesh()->NrFaces());
			ImGui::Text("Time for last subdiv: %f s", gn->GetHEmesh()->LastTime());

			if(ImGui::Button("Subdivide"))
				this->Subdivide();

			ImGui::Checkbox("Automatic Rotation", &this->autoRotate);
		ImGui::End();
	}
}

} // namespace Example