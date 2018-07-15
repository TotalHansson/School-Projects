#pragma once

#include <vector>

#include "MathLib3D.h"

#define uint unsigned int
#define ushort unsigned short
#define uchar unsigned char

struct Nax3Header
{
	uint magic;
	uint numClips;
	uint numKeys;
};

struct Nax3Clip
{
	ushort numCurves;
	ushort startKeyIndex;
	ushort numKeys;
	ushort keyStride;
	ushort keyDuration; 
	uchar preInfinityType;
	uchar postInfinityType;
	ushort numEvents;
	char name[50];
};

struct Nax3AnimEvent
{
	char name[47];
	char category[15];
	ushort keyIndex;
};

struct Nax3Curve
{
	uint firstKeyIndex;
	uchar isActive;			// 0 or 1
	uchar isStatic;			// 0 or 1
	uchar curveType;
	uchar _padding;			// padding
	float staticKeyX;
	float staticKeyY;
	float staticKeyZ;
	float staticKeyW;
};

struct Keys
{
	Vector3D v;
};

struct AnimationGroup
{
	Nax3Clip clip;
	std::vector<Nax3AnimEvent> animEvents;
	std::vector<Nax3Curve> curves;
};