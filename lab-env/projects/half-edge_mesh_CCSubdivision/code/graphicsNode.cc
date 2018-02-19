#include "graphicsNode.h"

GraphicsNode::GraphicsNode()
{
	std::shared_ptr<MeshResource> m(new MeshResource);
	std::shared_ptr<HEmesh> HEm(new HEmesh);
	std::shared_ptr<TextureResource> t(new TextureResource);
	std::shared_ptr<ShaderResource> s(new ShaderResource);

	mesh = m;
	HEMesh = HEm;
	texture = t;
	shader = s;
}

GraphicsNode::~GraphicsNode()
{}


void GraphicsNode::Draw()
{
	this->shader->UseProgram();
	this->texture->BindTexture();

	this->mesh->Render();
}
void GraphicsNode::DrawHE(int nrFaces)
{
	this->shader->UseProgram();
	this->texture->BindTexture();

	this->HEMesh->Render(nrFaces);
}

void GraphicsNode::SubdivideHEmesh(int subdivides)
{
	frameStart = std::chrono::system_clock::now();

	for (int i = 0; i < subdivides; i++)
	{
		this->HEMesh->CCSubdivide();
	}

	printf("Generating GL buffers...\n");
	start = std::chrono::system_clock::now();
	
	this->HEMesh->GenBuffers();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	printf("Done (%f s)\n\n", elapsed_seconds.count());

	
	frameEnd = std::chrono::system_clock::now();
	elapsed_seconds = frameEnd - frameStart;
	printf("Total time: %f s\n\n", elapsed_seconds.count());
}


void GraphicsNode::Transform(Matrix modelPosition, Matrix rotation)
{
	this->shader->UseProgram();

	Matrix projection(50.0f, 1.0f, 100.0f);

	Matrix view = Matrix(
						Vec3(0, 0, 5),	// Camera position
						Vec3(0, 0, 0),	// Looking at
						Vec3(0, 1, 0)	// Up
					);

	Matrix MVP = projection * view * modelPosition;


	this->shader->ModifyMatrix4vf(MVP, "MVP");
	this->shader->ModifyMatrix4vf(view, "V");
	this->shader->ModifyMatrix4vf(modelPosition, "M");
	Matrix MV = view * modelPosition;
	this->shader->ModifyMatrix4vf(MV, "MV");

}

void GraphicsNode::SetLight(PointLight light)
{
	this->shader->UseProgram();

	Vec3 cameraPos(0, 0, 5);

	this->shader->ModifyVector3fv(light.position, "LightPosition_worldspace");
	this->shader->ModifyVector3fv(light.ambientColor, "ambientColor");
	this->shader->ModifyVector3fv(light.diffuseColor, "diffuseColor");
	this->shader->ModifyVector3fv(light.specColor, "specColor");
}


std::shared_ptr<MeshResource> GraphicsNode::GetMesh()
{
	return mesh;
}
void GraphicsNode::SetMesh(std::shared_ptr<MeshResource> mesh)
{
	this->mesh = mesh;
}
void GraphicsNode::SetMesh(const char* filename)
{
	this->mesh->LoadMeshFromFile(filename);
}


std::shared_ptr<HEmesh> GraphicsNode::GetHEmesh()
{
	return HEMesh;
}
void GraphicsNode::SetHEmesh(std::shared_ptr<HEmesh> HEMesh)
{
	this->HEMesh = HEMesh;
}
void GraphicsNode::SetHEmesh(const char* filename)
{
	this->HEMesh->LoadMeshFromFile(filename);
	this->HEMesh->GenBuffers();
}


std::shared_ptr<TextureResource> GraphicsNode::GetTexture()
{
	return texture;
}
void GraphicsNode::SetTexture(std::shared_ptr<TextureResource> texture)
{
	this->texture = texture;
}
void GraphicsNode::SetTexture(const char* filename)
{
	this->texture->LoadFromFile(filename);
}
void GraphicsNode::SetTexture(unsigned char* texture, int x, int y)
{
	this->texture->ApplyTexture(texture, x, y);
}


std::shared_ptr<ShaderResource> GraphicsNode::GetShader()
{
	return shader;
}
void GraphicsNode::SetShader(std::shared_ptr<ShaderResource> shader)
{
	this->shader = shader;
}
void GraphicsNode::SetShader(const char* vertex, const char* fragment)
{
	shader->LoadVertexShader(vertex);
	shader->LoadFragmentShader(fragment);
	shader->LinkProgram();
	shader->GetHandles();
}