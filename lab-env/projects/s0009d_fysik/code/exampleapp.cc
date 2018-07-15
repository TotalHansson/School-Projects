//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include "physics.h"
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
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			gameObjects[0]->rigidBody->AddForce(Vector3D(0, 10.0f, 0));
		}
		else if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
		}


		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
		}
		else if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
		}


		// Esc or Space to exit the program
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_SPACE)
		{
			fprintf(stderr, "--- End of s0009d_fysik ---\n");
			this->window->Close();
		}
		

		// Z to toggle between full, wireframe, or vertex render
		// if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		// {
		// 	if (drawMode == 0)
		// 	{
		// 		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// 		drawMode = 1;
		// 	}
		// 	else if(drawMode == 1)
		// 	{
		// 		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		// 		drawMode = 2;
		// 	}
		// 	else if (drawMode == 2)
		// 	{
		// 		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// 		drawMode = 0;
		// 	}
		// }
	});

	window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
	{
		// Left click
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			lClicking = true;
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			lClicking = false;

		// Right click
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			ShootRay();
			
			rClicking = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			rClicking = false;
	});

	window->SetMouseScrollFunction([this](double x, double y)
	{
		posZ -= y*0.2;
		if (posZ < 1.5f)
			posZ = 1.5f;
	});

	window->SetMouseMoveFunction([this](float64 x, float64 y)
	{
		// Rotate the camera while holding the left mouse button
		if (lClicking)
		{
			this->hCameraAngle -= (x-oldX) * mouseSpeed;
			this->vCameraAngle -= (y-oldY) * mouseSpeed;

			if (vCameraAngle < -80.0f)
				vCameraAngle = -80.0f;
			else if(vCameraAngle > 80.0f)
				vCameraAngle = 80.0f;
		}

		if (rClicking)
		{
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

		// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


		// Set the projection matrix for the scene
		this->projection = Matrix3D(60.0f, 1.0f, 100.0f);


		// Set light for the scene
		light.position = Vector3D(5.0f, 5.0f, 5.0f);
		light.ambientColor = Vector3D(0.1f, 0.1f, 0.1f);
		light.diffuseColor = Vector3D(0.5f, 0.5f, 0.5f);
		light.specularColor = Vector3D(1.0f, 1.0f, 1.0f);
		light.intensity = 60.0f;
		this->lightTransform.translateTo(light.position);



		// Create new shader programs (I should create the shaders only... but I'm to lazy)
		for (unsigned int i = 0; i < 4; i++)
		{
			// Create a new graphics node
			gn[i] = GraphicsNode();

			// Set vertex and fragment shaders from files
			gn[i].setShader("../resources/shaders/standard.vertexshader",
							"../resources/shaders/standard.fragmentshader");

			// Load mesh from file
			gn[i].setMesh("../resources/models/tris/quad_small.obj");
		}
		gn[0].setTexture("../resources/textures/yellow.png");
		gn[1].setTexture("../resources/textures/red.png");
		gn[2].setTexture("../resources/textures/green.png");
		gn[3].setTexture("../resources/textures/gray.png");



		// Game objects
		//-----------------------------------------------------------------------------------------

		// Main object that can be interacted with
		GameObject *newGameObject = new GameObject("../resources/models/tris/goStone.obj", "../resources/textures/blue.png");
		// GameObject *newGameObject = new GameObject("../resources/models/tris/cube.obj", "../resources/textures/blue.png");
		newGameObject->rigidBody->SetMass(2.0f);

		// Initial values
		newGameObject->rigidBody->SetPosition(Vector3D(-10.0f, 1, 0));
		newGameObject->rigidBody->AddForce(Vector3D(10, 10, 0));
		newGameObject->rigidBody->AddAngularMomentum(Vector3D(0, 0, 0.4f));

		newGameObject->Update(0);
		gameObjects.push_back(newGameObject);



		// Object 2 that will be collided with mid air
		GameObject *newGameObject2 = new GameObject("../resources/models/tris/goStone.obj", "../resources/textures/blue.png");
		// GameObject *newGameObject2 = new GameObject("../resources/models/tris/cube.obj", "../resources/textures/blue.png");
		newGameObject2->rigidBody->SetMass(2.0f);

		newGameObject2->rigidBody->SetPosition(Vector3D(10.0f, 1, 0));
		newGameObject2->rigidBody->AddForce(Vector3D(-10, 10, 0));
		// newGameObject2->rigidBody->AddAngularMomentum(Vector3D(1, 0, 0));

		newGameObject2->Update(0);
		// newGameObject2->rigidBody->SetStatic(true);
		gameObjects.push_back(newGameObject2);



		// Floor
		GameObject *newGameObject3 = new GameObject("../resources/models/tris/floor_large.obj", "../resources/textures/gray.png");
		newGameObject3->rigidBody->SetPosition(Vector3D(0, -0.75f, 0));
		
		newGameObject3->rigidBody->Update(0);
		newGameObject3->rigidBody->SetStatic(true); // Floor is not affected by physics
		gameObjects.push_back(newGameObject3);



		// Axis arrows
		//-----------------------------------------------------------------------------------------
		axisArrows = new GraphicsNode();
		axisArrows->setShader("../resources/shaders/axisArrow.vertexshader",
					  		  "../resources/shaders/axisArrow.fragmentshader");
		axisArrows->setMesh("../resources/models/tris/axisArrows.obj");
		axisArrows->setTexture("../resources/textures/gray.png");


		// Light model
		//-----------------------------------------------------------------------------------------
		lightModel = new GraphicsNode();
		lightModel->setShader("../resources/shaders/basic.vertexshader",
					  		  "../resources/shaders/basic.fragmentshader");
		lightModel->setMesh("../resources/models/tris/icosphere_small.obj");
		lightModel->setTexture("../resources/textures/white.png");


		// Intersection marker
		//-----------------------------------------------------------------------------------------

		// Starting position
		this->intersectionPoint = Vector3D(10000.0f, 0.0f, 0.0f);

		intersectMarker = new GraphicsNode();
		intersectMarker->setShader("../resources/shaders/standard.vertexshader",
					  			   "../resources/shaders/standard.fragmentshader");
		intersectMarker->setMesh("../resources/models/tris/icosphere_small.obj");
		intersectMarker->setTexture("../resources/textures/green.png");




		// Bind UI render function
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
	Matrix3D id;

	Vector3D camPos   (0.0f, 0.0f, 1.0f, 0.0f);
	Vector3D camRight (1.0f, 0.0f, 0.0f, 0.0f);
	Vector3D camUp 	  (0.0f, 1.0f, 0.0f, 0.0f);

	// Axis arrow screen position
	this->axisTransform.translateTo(-1.4f, -1.4f, -3.0f);


	Matrix3D intersectMarkerMatrix;
	intersectMarkerMatrix.scale(0.5f);


	// Set the light for the gray texture (it's static, so no need to update it)
	gn[3].setLight(light);


	while (this->window->IsOpen())
	{
		// Set frame start-time
		start = std::chrono::system_clock::now();



		// Reset the canvas to remove last frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get inputs
		this->window->Update();


		// Set the start time of my code
		start2 = std::chrono::system_clock::now();

		// Camera default values
		camPos   = Vector3D(0.0f, 0.0f, 1.0f, 0.0f);
		camRight = Vector3D(1.0f, 0.0f, 0.0f, 0.0f);
		camUp 	 = Vector3D(0.0f, 1.0f, 0.0f, 0.0f);
		
		// Camera rotation matrices
		Matrix3D yaw(camUp, this->hCameraAngle);
		Matrix3D pitch(camRight, this->vCameraAngle);
		Matrix3D camRotation(yaw * pitch);

		// Set the position of the camera
		this->cameraPosition = camRotation * camPos;
		this->cameraPosition[3] = 0;
		this->cameraPosition = this->cameraPosition.normal() * posZ;

		// Calculate the right vector for the camera
		camRight = camUp.cross(this->cameraPosition.normal());
		camRight[3] = 0;
		camRight = camRight.normal();

		// Calculate the up vector for the camera
		camUp = this->cameraPosition.cross(camRight);
		camUp[3] = 0;
		camUp = camUp.normal();


		// Create the view matrix
		this->camera = Matrix3D(
		 	this->cameraPosition,	// Camera position
			this->cameraLookAt,		// Looking at
			camUp					// Up
		);


		
		// Move all GameObjects
		for (unsigned int i = 0; i < this->gameObjects.size(); i++)
		{
			// Physics
			this->gameObjects[i]->Update(dt);
		}


		// Find collisions
		CollisionInfo info;
		std::vector<GameObject*> possibleCollisions;
		LineSweep::FindOverlapingAABB(this->gameObjects, possibleCollisions);

		// Loop over all possible collisions
		for (unsigned int i = 0; i < possibleCollisions.size(); i += 2)
		{
			GameObject *A = possibleCollisions[i];
			GameObject *B = possibleCollisions[i+1];
			bool collision = GJK::GJKIntersect(A, B, info);

			
			// Collision response
			// https://gafferongames.com/post/collision_response_and_coulomb_friction/
			if (collision)
			{
				this->intersectionPoint = info.worldPosition;

				// Coefficient of restitution (elasticity). 1 = perfect bounce, 0 = no bounce
				// Should be a value on the GameObjects. Get the lowest of the two
				float e = 0.5f;


				Vector3D normal = info.collisionNormal;

				// Separeate the objects
				// if B is static, move only A
				if(!A->rigidBody->IsStatic() && B->rigidBody->IsStatic())
					A->rigidBody->SetPosition(A->rigidBody->GetPosition() + normal * info.penetrationDepth);

				// if A is static, move only B
				else if(A->rigidBody->IsStatic() && !B->rigidBody->IsStatic())
					B->rigidBody->SetPosition(B->rigidBody->GetPosition() - normal * info.penetrationDepth);

				// if neither is static, move both
				else if (!A->rigidBody->IsStatic() && !B->rigidBody->IsStatic())
				{
					A->rigidBody->SetPosition(A->rigidBody->GetPosition() + normal * info.penetrationDepth * 0.5f);
					B->rigidBody->SetPosition(B->rigidBody->GetPosition() - normal * info.penetrationDepth * 0.5f);
				}


				// Object A
				Vector3D velocity1 = A->rigidBody->VelocityAtWorldPoint(info.worldPosition);
				float iMass1 = A->rigidBody->GetInverseMass();
				Matrix3D iInertia1 = A->rigidBody->GetInverseInertia();
				Vector3D r1 = info.worldPosition - A->rigidBody->GetPosition();

				// Object B
				Vector3D velocity2 = B->rigidBody->VelocityAtWorldPoint(info.worldPosition);
				float iMass2 = B->rigidBody->GetInverseMass();
				Matrix3D iInertia2 = B->rigidBody->GetInverseInertia();
				Vector3D r2 = info.worldPosition - B->rigidBody->GetPosition();

				// Relative velocity
				Vector3D relativeVelocity = velocity1 - velocity2;


				float vn = fminf(0.0f, relativeVelocity.dot3(normal));


				// Object vs object
				float k = iMass1 + iMass2 + 
						  (iInertia1 * r1.cross(normal).cross(r1) +
						   iInertia2 * r2.cross(normal).cross(r2)).dot3(normal);

				float j1 = -(1+e) * vn / k;


				// Apply impulse
				A->rigidBody->AddForce(j1 * normal);
				A->rigidBody->AddAngularMomentum(j1 * r1.cross(normal));

				B->rigidBody->AddForce(-j1 * normal);
				B->rigidBody->AddAngularMomentum(-j1 * r2.cross(normal));
				
				



				// Coulomb friction

				// Coefficient of friction
				float u = 0.8f;

				float epsilon = 0.001f;


				// Get updated velocity
				velocity1 = A->rigidBody->VelocityAtWorldPoint(info.worldPosition);
				velocity2 = B->rigidBody->VelocityAtWorldPoint(info.worldPosition);


				// Amount of velocity going tangent to the normal
				Vector3D tangentVelocity1 = velocity1 - normal * velocity1.dot3(normal);
				Vector3D tangentVelocity2 = velocity2 - normal * velocity2.dot3(normal);

				// If there is any sliding
				if (tangentVelocity1.squareLength() > epsilon * epsilon)
				{
					Vector3D tangent = tangentVelocity1.normal();

					float vt = velocity1.dot3(tangent);
					float kt = iMass1 + r1.cross(tangent).dot3(iInertia1 * r1.cross(tangent));

					//		   clamp(-vt / kt, -u * j, u * j);
					float jt = fminf(fmaxf(-vt / kt, -u * j1), u * j1);

					// Apply the friction
					A->rigidBody->AddForce(jt * tangent);
					A->rigidBody->AddAngularMomentum(jt * r1.cross(tangent));
				}
				if (tangentVelocity2.squareLength() > epsilon * epsilon)
				{
					Vector3D tangent = tangentVelocity2.normal();

					float vt = velocity2.dot3(tangent);
					float kt = iMass2 + r2.cross(tangent).dot3(iInertia2 * r2.cross(tangent));

					//		   clamp(-vt / kt, -u * j, u * j);
					float jt = fminf(fmaxf(-vt / kt, -u * j1), u * j1);

					// Apply the friction
					B->rigidBody->AddForce(jt * tangent);
					B->rigidBody->AddAngularMomentum(jt * r2.cross(tangent));
				}
			
			}
		}
		

		// Draw all GameObjects
		for (unsigned int i = 0; i < this->gameObjects.size(); i++)
		{
			// Rendering
			this->gameObjects[i]->graphicsNode->setLight(light);
			this->gameObjects[i]->graphicsNode->transform(this->gameObjects[i]->rigidBody->GetTransform(), this->camera, this->projection);
			this->gameObjects[i]->graphicsNode->draw();
		}


		
		// Draw the world grid
		this->DrawGrid();



		// Debug drawing, bounding boxes

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			this->DrawAABB(gameObjects[i]);
			this->DrawOBB(gameObjects[i]);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		

		// Use the green shader
		gn[2].setLight(light);
		gn[2].transform(id, this->camera, this->projection);
		gn[2].getTexture()->bindTexture();

		// Draw the last ray cast
		if(this->ray != nullptr && (this->intersectionPoint[0] != 10000.0f && this->intersectionPoint[1] != 0.0f && this->intersectionPoint[2] != 0.0f))
		{
			Vector3D startPoint = this->ray->start;
			Vector3D endPoint = startPoint + this->ray->direction * 100.0f;

			// glBegin should not be used... but it's a quick solution for now
			glBegin(GL_LINES);
				glVertex3f(startPoint[0], startPoint[1], startPoint[2]);
				glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
			glEnd();
		}

		// Render the intersection point
		intersectMarkerMatrix.translateTo(this->intersectionPoint[0], this->intersectionPoint[1], this->intersectionPoint[2]);
		intersectMarker->setLight(light);
		intersectMarker->transform(intersectMarkerMatrix, this->camera, this->projection);
		intersectMarker->draw();


		// Render light model
		lightModel->transform(lightTransform, this->camera, this->projection);
		lightModel->draw();


		// Render the axis arrows
		axisArrows->transform(this->camera.invert() * this->axisTransform * camRotation.invert(), this->camera, this->projection);
		axisArrows->draw();


		// End of my code
		end2 = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime2 = end2 - start2;
		dt2 = elapsedTime2.count();


		// Show the rendered buffer on the screen
		this->window->SwapBuffers();


		// Unbind the current program
		glUseProgram(0);


		// Set dt from last frame
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime = end - start;
		dt = elapsedTime.count();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::DrawGrid()
{
	// Use the shader program for gray
	gn[3].transform(Vector3D(), this->camera, this->projection);
	gn[3].getTexture()->bindTexture();


	// Draw world grid
	glLineWidth(0.2f);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_LINES);
		for (int i = -20; i < 20; i++)
		{
			glVertex3f(-20.0f, 0.0f, i);
			glVertex3f(20.0f, 0.0f, i);

			glVertex3f(i, 0.0f, -20.0f);
			glVertex3f(i, 0.0f, 20.0f);
		}
	glEnd();
	glLineWidth(1.5f);
	glBegin(GL_LINES);
		glVertex3f(-20.0f, 0.0f, 0.0f);
		glVertex3f(20.0f, 0.0f, 0.0f);

		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);

		glVertex3f(0.0f, 0.0f, -20.0f);
		glVertex3f(0.0f, 0.0f, 20.0f);
	glEnd();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::DrawAABB(GameObject *go)
{
	// Use the program for yellow
	this->gn[0].setLight(this->light);
	this->gn[0].transform(Vector3D(), this->camera, this->projection);
	this->gn[0].getTexture()->bindTexture();
	
	const Vector3D *aabb = go->aabb;
	Vector3D min = aabb[0];
	Vector3D max = aabb[1];
	
	glBegin(GL_QUAD_STRIP);
		glVertex3f(min[0], min[1], min[2]); // left down back
		glVertex3f(min[0], max[1], min[2]); // left up back

		glVertex3f(max[0], min[1], min[2]); // right down back
		glVertex3f(max[0], max[1], min[2]); // right up back

		glVertex3f(max[0], min[1], max[2]); // right down front
		glVertex3f(max[0], max[1], max[2]); // right up front

		glVertex3f(min[0], min[1], max[2]); // left down front
		glVertex3f(min[0], max[1], max[2]); // left up front

		glVertex3f(min[0], min[1], min[2]); // left down back
		glVertex3f(min[0], max[1], min[2]); // left up back
	glEnd();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::DrawOBB(GameObject *go)
{
	// Use the program for red
	gn[1].setLight(light);
	gn[1].transform(Vector3D(), this->camera, this->projection);
	gn[1].getTexture()->bindTexture();

	const Vector3D *obb = go->graphicsNode->getMesh()->getBoundingBox();
	Vector3D box[8];
	Matrix3D transform = go->rigidBody->GetTransform();
	for (unsigned int i = 0; i < 8; i++)
	{
		box[i] = transform * obb[i];
	}

	glBegin(GL_QUAD_STRIP);
		glVertex3f(box[0][0], box[0][1], box[0][2]); // left down back
		glVertex3f(box[1][0], box[1][1], box[1][2]); // left up back
		glVertex3f(box[2][0], box[2][1], box[2][2]); // right down back
		glVertex3f(box[3][0], box[3][1], box[3][2]); // right up back
		glVertex3f(box[4][0], box[4][1], box[4][2]); // right down front
		glVertex3f(box[5][0], box[5][1], box[5][2]); // right up front
		glVertex3f(box[6][0], box[6][1], box[6][2]); // left down front
		glVertex3f(box[7][0], box[7][1], box[7][2]); // left up front
		glVertex3f(box[0][0], box[0][1], box[0][2]); // left down back
		glVertex3f(box[1][0], box[1][1], box[1][2]); // left up back
	glEnd();
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::RenderUI()
{
	if(this->window->IsOpen())
	{
		ImGui::Begin("Drawing");
			ImGui::Text("FPS: %.2f", 1/(this->dt));
			ImGui::Text("dt: %.4f ms", this->dt*1000.0f);
			ImGui::Text("Execution time: %.4f ms", this->dt2*1000.0f);
		ImGui::End();

		ImGui::Begin("Camera");
			ImGui::Text("Position: %.2f, %.2f, %.2f", this->cameraPosition[0], this->cameraPosition[1], this->cameraPosition[2]);
			ImGui::Text("Looking at: %.2f, %.2f, %.2f", this->cameraLookAt[0], this->cameraLookAt[1], this->cameraLookAt[2]);
		ImGui::End();

		ImGui::Begin("Controls");
			ImGui::Text("Rotate camera: LMB");
			ImGui::Text("Shoot ray: RMB");
			ImGui::Text("Close window: Esc / Space");
		ImGui::End();

		ImGui::Begin("Rigid Body Information");
			Vector3D pos = this->gameObjects[0]->rigidBody->GetPosition();
			Vector3D vLinear = this->gameObjects[0]->rigidBody->GetVelocity();
			Vector3D vAngular = this->gameObjects[0]->rigidBody->GetAngularVelocity();
			ImGui::Text("Mass: %.3f", this->gameObjects[0]->rigidBody->GetMass());
			ImGui::Text("Position: %.3f, %.3f, %.3f", pos[0], pos[1], pos[2]);
			ImGui::Text("Linear velocity: %.3f, %.3f, %.3f", vLinear[0], vLinear[1], vLinear[2]);
			ImGui::Text("Angular velocity: %.3f, %.3f, %.3f", vAngular[0], vAngular[1], vAngular[2]);
		ImGui::End();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::ShootRay()
{
	if (this->ray != nullptr)
		delete ray;

	this->ray = new Ray(oldX, oldY, window->GetWidth(), window->GetHeight(), this->camera, this->projection, this->cameraPosition);


	GameObject *closest = nullptr;
	float closestDist = 10000.0f;
	Vector3D finalIntersect;

	// Check collision with all objects
	for (unsigned int i = 0; i < this->gameObjects.size(); i++)
	{
		Vector3D *aabb = this->gameObjects[i]->aabb;
		Vector3D *obb = this->gameObjects[i]->graphicsNode->getMesh()->getBoundingBox();
		Matrix3D tmpPose = this->gameObjects[i]->rigidBody->GetTransform();

		// AABB collision
		float aabbDistance = ray->AABBIntersect(aabb[0], aabb[1], this->intersectionPoint);
		if (aabbDistance > 0.0f)
		{

			// OBB collision
			float obbDistance = ray->OBBIntersect(obb[0], obb[5], tmpPose, this->intersectionPoint);
			if (obbDistance > 0.0f)
			{

				// If this box is closer than the closest one
				if (aabbDistance < closestDist)
				{
					closestDist = aabbDistance;
					closest = this->gameObjects[i];
					finalIntersect = intersectionPoint;
				}
			}		
		}
	}
	


	// Affect the selected object
	if (closest != nullptr)
	{	
		closest->rigidBody->ApplyImpulseAtWorldPoint(this->intersectionPoint, this->ray->direction, 1.0f);

		this->intersectionPoint = finalIntersect;
	}
	else
		this->intersectionPoint = Vector3D(10000.0f, 0.0f, 0.0f);
	
}



} // namespace Example