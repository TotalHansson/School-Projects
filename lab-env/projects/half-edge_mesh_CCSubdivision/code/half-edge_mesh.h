#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <cmath>
#include <string>

#ifndef GL_INCLUDE
#define GL_INCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

struct HEvert;
struct HEedge;
struct HEface;

struct HEvert
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	bool inserted = false;
	HEedge *edge = nullptr;
};
struct HEedge
{
	HEvert *vert = nullptr;
	HEedge *pair = nullptr;
	HEface *face = nullptr;
	HEedge *next = nullptr;

	float u = 0.0f;
	float v = 0.0f;

	float nx = 0.0f;
	float ny = 0.0f;
	float nz = 0.0f;

	bool alreadySplit = false;
	HEvert *tmpPoint = nullptr;
};
struct HEface
{
	HEedge *edge = nullptr;

	HEvert *tmpPoint = nullptr;
};


class HEmesh
{
private:
	std::vector<HEvert *> verts;
	std::vector<HEedge *> edges;
	std::vector<HEface *> faces;

	std::vector<float *> normals;
	std::vector<float *> uvs;

	std::chrono::time_point<std::chrono::system_clock> subdivStart, subdivEnd, start, end;
	std::chrono::duration<double> elapsed_seconds;

	GLuint elementBuffer;
	GLuint vertexBuffer;

	HEface* MakeFace(HEvert *midPoint, HEedge *edge1, HEedge *edge4, HEedge *uv1, HEedge *uv2, HEedge *uv3);
	void MakeNewFaces();
	void SplitEdges();
	void UpdateVerts();
	void MakeEdgePoints();
	void MakeFacePoints();
	void ResetValues();


public:
	HEmesh();
	~HEmesh();

	int nrSubdivs = 0;

	void Render(int nrFaces);
	void GenBuffers();

	int NrVerts()
	{return this->verts.size();}
	int NrFaces()
	{return this->faces.size();}
	int NrEdges()
	{return this->edges.size();}
	float LastTime()
	{return this->elapsed_seconds.count();}

	void CCSubdivide();
	void LoadMeshFromFile(const char *filename);
};