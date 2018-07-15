//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include "tinyxml.h"
#include "MathLib3D.h"
#include "NebulaSkeletonParser.h"
#include "Nax3Parser.h"
#include "Nvx2Parser.h"
#include "DDSParser.h"
#include "imgui.h"

#include <cstring>
#include <stdlib.h>


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
		// W A S D to translate the character
		if (key == GLFW_KEY_W)
		{
			tmpRotation.translateTo(tmpRotation[3], tmpRotation[7], tmpRotation[11] - 0.05f);
		}
		else if (key == GLFW_KEY_S)
		{
			tmpRotation.translateTo(tmpRotation[3], tmpRotation[7], tmpRotation[11] + 0.05f);
		}
		if (key == GLFW_KEY_D)
		{
			tmpRotation.translateTo(tmpRotation[3] + 0.05f, tmpRotation[7], tmpRotation[11]);
		}
		else if (key == GLFW_KEY_A)
		{
			tmpRotation.translateTo(tmpRotation[3] - 0.05f, tmpRotation[7], tmpRotation[11]);
		}


		// Esc or Space to exit the program
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_SPACE)
		{
			fprintf(stderr, "--- End of s0008e_skeleton ---\n");
			this->window->Close();
		}
		

		// Z to toggle between full, wireframe, or vertex render
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			if (drawMode == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				drawMode = 1;
			}
			else if(drawMode == 1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				drawMode = 2;
			}
			else if (drawMode == 2)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				drawMode = 0;
			}
		}
	});

	window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			lClicking = true;
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			lClicking = false;

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			rClicking = true;
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			rClicking = false;
	});

	window->SetMouseScrollFunction([this](double x, double y)
	{
		posZ -= y*0.2;
	});

	window->SetMouseMoveFunction([this](float64 x, float64 y)
	{
		if (lClicking)
		{
			this->cameraAngle -= (x-oldX) * 0.01f;
		}

		if (rClicking)
		{	
			tmpRotation = tmpRotation * Matrix3D('y', 0.01f * (x-oldX));
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

		// Set light for the scene
		light.position = Vector3D(5.0f, 5.0f, 5.0f);
		light.ambientColor = Vector3D(0.1f, 0.1f, 0.1f);
		light.diffuseColor = Vector3D(0.5f, 0.5f, 0.5f);
		light.specularColor = Vector3D(1.0f, 1.0f, 1.0f);
		light.intensity = 60.0f;



		// Read the unit files
		readNebulaSkeletonFile("../resources/units/footman/Unit_Footman.constants", &s); // Skeleton
		readNax3File("../resources/units/footman/Unit_Footman.nax3", &s); // Animation
		readNvx2File("../resources/units/footman/Unit_Footman.nvx2", &s); // Mesh

		// Apply skinning shaders
		s.setShaders("../resources/shaders/skinning.vertexshader",
					 "../resources/shaders/skinning.fragmentshader");

		// Load texture files for the unit (Needs to be done after shaders are loaded)
		readDDSFile("../resources/units/footman/Footman_Diffuse.dds", &s, DDSTYPE_DIFFUSE);
		readDDSFile("../resources/units/footman/Footman_Normal.dds", &s, DDSTYPE_NORMAL);
		readDDSFile("../resources/units/footman/Footman_Specular.dds", &s, DDSTYPE_SPECULAR);
		
		fprintf(stderr, "Unit loaded\n");

		// Send vertices to GPU
		s.generateBuffers();



		// Bind UI render function
		this->window->SetUiRender([this]()
		{
			this->renderUI();
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
	Matrix3D id;


	// Set the pos of each joint to its parents absolute pos * this relative pos
	s[0]->setAbsolutePosition(id);

	// Save the inverse bind pose of each joint
	s[0]->setInverseBindPose();



	while (this->window->IsOpen())
	{
		// Set frame start-time
		start = std::chrono::system_clock::now();


		// Set the polygon mode for my objects
		if (this->drawMode == 0)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		else if(this->drawMode == 1)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		else if (this->drawMode == 2)
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);


		// Reset the canvas to remove last frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();



		this->camera = Matrix3D(
		 	Vector3D
		 	(
		 		posZ * sin(cameraAngle), 
		 		2,
		 		posZ * cos(cameraAngle)
		 	), 	// Camera position
			Vector3D(0, 2, 0),	// Looking at
			Vector3D(0, 1, 0)	// Up
		);



		// ----- Render skeleton mesh -----

		// Set the joints absolute position to parents absolute * this relative
		s[0]->setAbsolutePosition(s[0]->relativeToParent);
		//
		// Animate the skeleton
		s.animate(dt, this->runAnim);

		// Update the shaders joint uniform
		s.sendJointsToShader();

		// Send model and camera pose to shader
		s.updateMVPMatrix(this->tmpRotation, this->camera);

		// Send updated light information to shader
		s.updateLight(light);

		// Draw the skeletons mesh
		s.render();




		// Show the rendered buffer on the screen
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		this->window->SwapBuffers();


		// Set dt from last frame
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime = end - start;
		dt = elapsedTime.count()*1000;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::renderUI()
{
	if(this->window->IsOpen())
	{
		// Create a window
		ImGui::Begin("Animation controls");
			ImGui::Text("Current animation: %s", s.currentAnimName());

			if(ImGui::Button("Prev", ImVec2(73, 25)))
			{
				s.prevAnimClip();
			}
			ImGui::SameLine(85);
			if(ImGui::Button("Next", ImVec2(73, 25)))
			{
				s.nextAnimClip();
			}
			
			ImGui::PushItemWidth(150);
			if (ImGui::SliderFloat("", &this->animSpeed, 0.01f, 2.0f, "Speed: %.3f"))
			{
				s.changeAnimSpeed(this->animSpeed);
			}

			if (ImGui::Checkbox("Run Animation", &this->runAnim))
			{}

			if (ImGui::Checkbox("Bind Pose", &this->bindPose))
			{
				s.setTPose(this->bindPose);
			}


			ImGui::Text("Step though animation");
			if(ImGui::Button("Prev Frame"))
			{
				s.prevFrame();
			}
			ImGui::SameLine();
			if(ImGui::Button("Next Frame"))
			{
				s.nextFrame();
			}
		ImGui::End();


		ImGui::Begin("Lighting");
			ImGui::Text("Light Position");

			ImGui::PushItemWidth(100);
			ImGui::Text("x:");
			ImGui::SameLine(25);
			ImGui::InputFloat("##x", &light.position[0], 0.5f, 1.0f, 2);

			ImGui::Text("y:");
			ImGui::SameLine(25);
			ImGui::InputFloat("##y", &light.position[1], 0.5f, 1.0f, 2);

			ImGui::Text("z:");
			ImGui::SameLine(25);
			ImGui::InputFloat("##z", &light.position[2], 0.5f, 1.0f, 2);


			ImGui::Text("Light Intensity");
			ImGui::PushItemWidth(120);
			ImGui::SliderFloat("", &light.intensity, 0.1f, 200.0f, "%.2f");


			ImGui::Text("Light Color");
			ImGui::ColorEdit3("diffuse", &light.diffuseColor[0]);
			ImGui::ColorEdit3("ambient", &light.ambientColor[0]);
			ImGui::ColorEdit3("specular", &light.specularColor[0]);

		ImGui::End();


		ImGui::Begin("Render Mode");
			if (ImGui::Button("Full Render"))
				s.shader.modifyInt(0, "renderMode");

			if (ImGui::Button("Diffuse"))
				s.shader.modifyInt(1, "renderMode");

			if (ImGui::Button("Normals"))
				s.shader.modifyInt(2, "renderMode");

			if (ImGui::Button("Specular"))
				s.shader.modifyInt(3, "renderMode");

		ImGui::End();		


		ImGui::Begin("Drawing");
			ImGui::Text("FPS: %.2f", 1/(this->dt*0.001));
			ImGui::Text("dt: %.4f ms", this->dt);
		ImGui::End();

		ImGui::Begin("Controls");
			ImGui::Text("Rotate camera: LMB");
			ImGui::Text("Rotate character: RMB");
			ImGui::Text("Toggle polygon mode: Z");
		ImGui::End();
	}
}

} // namespace Example