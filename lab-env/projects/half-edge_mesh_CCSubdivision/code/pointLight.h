#pragma once

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"

class PointLight
{
public:
	Vec3 position;

	Vec3 ambientColor;
	float ambientPower;

	Vec3 diffuseColor;
	float diffusePower;

	Vec3 specColor;
	float specularPower;

	PointLight()
	{
		this->position = Vec3(0, 0, 0);
		this->ambientColor = Vec3(0, 0, 0);
		this->diffuseColor = Vec3(0, 0, 0);
		this->specColor = Vec3(0, 0, 0);
	}
};