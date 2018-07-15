#pragma once

#include "MathLib3D.h"

class PointLight
{
public:
	Vector3D position;

	Vector3D ambientColor;
	Vector3D diffuseColor;
	Vector3D specularColor;
	float intensity;

	PointLight()
	{
		this->position = Vector3D(0, 0, 0);
		this->ambientColor = Vector3D(0, 0, 0);
		this->diffuseColor = Vector3D(0, 0, 0);
		this->specularColor = Vector3D(0, 0, 0);
		this->intensity = 0.0f;
	}
};