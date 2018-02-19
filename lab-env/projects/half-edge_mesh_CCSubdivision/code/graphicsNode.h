#pragma once
#include <memory>
#include <chrono>

#include "meshResource.h"
#include "half-edge_mesh.h"
#include "textureResource.h"
#include "shaderResource.h"
#include "pointLight.h"


class GraphicsNode
{
private:
	std::shared_ptr<MeshResource> mesh;
	std::shared_ptr<HEmesh> HEMesh;
	std::shared_ptr<TextureResource> texture;
	std::shared_ptr<ShaderResource> shader;

	std::chrono::time_point<std::chrono::system_clock> frameStart, frameEnd, start, end;
	std::chrono::duration<double> elapsed_seconds;


public:

	GraphicsNode();
	~GraphicsNode();

	void Draw();
	void DrawHE(int nrFaces);
	void SubdivideHEmesh(int subdivides = 1);
	void Transform(Matrix modelPosition, Matrix rotation);
	void SetLight(PointLight light);

	// Mesh
	std::shared_ptr<MeshResource> GetMesh();
	void SetMesh(std::shared_ptr<MeshResource> mesh);
	void SetMesh(const char* filename);
	// HE-Mesh
	std::shared_ptr<HEmesh> GetHEmesh();
	void SetHEmesh(std::shared_ptr<HEmesh> HEMesh);
	void SetHEmesh(const char* filename);
	// Texture
	std::shared_ptr<TextureResource> GetTexture();
	void SetTexture(std::shared_ptr<TextureResource> texture);
	void SetTexture(const char* filename);
	void SetTexture(unsigned char* texture, int x, int y);
	// Shader
	std::shared_ptr<ShaderResource> GetShader();
	void SetShader(std::shared_ptr<ShaderResource> shader);
	void SetShader(const char* vertex, const char* fragment);
};