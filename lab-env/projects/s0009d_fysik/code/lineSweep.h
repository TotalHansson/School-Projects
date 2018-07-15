#pragma once

#include <vector>

#include "MathLib3D.h"
#include "gameObject.h"
#include "GJK.h"

class LineSweep
{
public:
	LineSweep()
	{}
	~LineSweep()
	{}

	static void FindOverlapingAABB(const std::vector<GameObject*> &gameObjects, std::vector<GameObject*> &possibleCollisions)
	{
		// fprintf(stderr, "Looking for collisions\n");

		// Nothing can collide if there are less than 2 objects
		if (gameObjects.size() < 2)
		{
			// fprintf(stderr, "Not enough objects, no collision\n");
			return;
		}

		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			for (unsigned int j = i+1; j < gameObjects.size(); j++)
			{
				if (gameObjects[i]->rigidBody->IsStatic() && gameObjects[j]->rigidBody->IsStatic())
					continue;
				
				if (gameObjects[i]->aabb[0][0] > gameObjects[j]->aabb[1][0] ||
					gameObjects[i]->aabb[1][0] < gameObjects[j]->aabb[0][0])
					continue;

				if (gameObjects[i]->aabb[0][1] > gameObjects[j]->aabb[1][1] ||
					gameObjects[i]->aabb[1][1] < gameObjects[j]->aabb[0][1])
					continue;
				

				if (gameObjects[i]->aabb[0][2] > gameObjects[j]->aabb[1][2] ||
					gameObjects[i]->aabb[1][2] < gameObjects[j]->aabb[0][2])
					continue;


				// The AABBs are overlaping, check OBB
				possibleCollisions.push_back(gameObjects[i]);
				possibleCollisions.push_back(gameObjects[j]);
			}
		}
	}
	
};