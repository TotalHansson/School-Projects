#pragma once

#include "MathLib3D.h"
#include "physics.h"
#include "GraphicsNode.h"

class GameObject
{
// private:
public:
	GraphicsNode *graphicsNode;
	RigidBody *rigidBody;
	Vector3D aabb[2];


// public:

	GameObject()
	{
		this->graphicsNode = new GraphicsNode();

		// Set vertex and fragment shaders from files
		this->graphicsNode->setShader("../resources/shaders/standard.vertexshader",
							"../resources/shaders/standard.fragmentshader");

		// Load mesh from file
		this->graphicsNode->setMesh("../resources/models/tris/cube.obj");

		// Load texture from file
		this->graphicsNode->setTexture("../resources/textures/blue.png");


		this->rigidBody = new RigidBody();
	}
	GameObject(const char *model, const char *texture)
	{
		this->graphicsNode = new GraphicsNode();

		// Set vertex and fragment shaders from files
		this->graphicsNode->setShader("../resources/shaders/standard.vertexshader",
							"../resources/shaders/standard.fragmentshader");

		// Load mesh from file
		this->graphicsNode->setMesh(model);

		// Load texture from file
		this->graphicsNode->setTexture(texture);


		this->rigidBody = new RigidBody();
	}

	~GameObject()
	{
		delete this->graphicsNode;
		delete this->rigidBody;
	}

	void Update(double dt)
	{
		this->rigidBody->Update(dt);

		// Bounding box
		Vector3D max(-100000, -100000, -100000);
		Vector3D min( 100000,  100000,  100000);

		Vector3D *b = this->graphicsNode->getMesh()->getBoundingBox();
		Vector3D tmpVec;

		Matrix3D transform = this->rigidBody->GetTransform();

		for (unsigned int i = 0; i < 8; i++)
		{
			tmpVec = transform * b[i];

			if (tmpVec[0] < min[0])
				min[0] = tmpVec[0];
			if (tmpVec[1] < min[1])
				min[1] = tmpVec[1];
			if (tmpVec[2] < min[2])
				min[2] = tmpVec[2];

			if (tmpVec[0] > max[0])
				max[0] = tmpVec[0];
			if (tmpVec[1] > max[1])
				max[1] = tmpVec[1];
			if (tmpVec[2] > max[2])
				max[2] = tmpVec[2];
		}

		this->aabb[0] = min;
		this->aabb[1] = max;
	}
	
};