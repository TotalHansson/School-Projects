#pragma once
#include <memory>
#include "MeshResource.h"
#include "TextureResource.h"
#include "ShaderResource.h"
#include "PointLight.h"


class GraphicsNode
{
private:
	std::shared_ptr<MeshResource> mesh;
	std::shared_ptr<TextureResource> texture;
	std::shared_ptr<ShaderResource> shader;



public:

	void *objectRef;

	GraphicsNode()
	{
		std::shared_ptr<MeshResource> m(new MeshResource);
		std::shared_ptr<TextureResource> t(new TextureResource);
		std::shared_ptr<ShaderResource> s(new ShaderResource);

		mesh = m;
		texture = t;
		shader = s;
	}

	~GraphicsNode()
	{

	}


	void draw()
	{
		this->shader->useProgram();

		this->texture->bindTexture();


		this->mesh->render();
	}

	void transform(Matrix3D modelPosition, Matrix3D view, Matrix3D projection)
	{
		this->shader->useProgram();

		Matrix3D MVP = projection * view * modelPosition;
		Matrix3D MV = view * modelPosition;
		
		//----------------------------------------

		this->shader->modifyMatrix4vf(MVP, "MVP");
		this->shader->modifyMatrix4vf(MV, "MV");
		this->shader->modifyMatrix4vf(view, "V");
		this->shader->modifyMatrix4vf(modelPosition, "M");
	}

	void setLight(PointLight light)
	{
		this->shader->useProgram();

		this->shader->modifyVector3fv(light.position, "LightPosition_worldspace");
		this->shader->modifyVector3fv(light.ambientColor, "ambientColor");
		this->shader->modifyVector3fv(light.diffuseColor, "diffuseColor");
		this->shader->modifyVector3fv(light.specularColor, "specularColor");
		this->shader->modifyFloat(light.intensity, "lightIntensity");
	}


	std::shared_ptr<MeshResource> getMesh()
	{
		return this->mesh;
	}
	void setMesh(std::shared_ptr<MeshResource> mesh)
	{
		this->mesh = mesh;
	}
	void setMesh(const char* filename)
	{
		this->mesh->loadMeshFromFile(filename);
	}

	std::shared_ptr<TextureResource> getTexture()
	{
		return texture;
	}
	void setTexture(std::shared_ptr<TextureResource> texture)
	{
		this->texture = texture;
	}
	void setTexture(const char* filename)
	{
		this->texture->loadFromFile(filename);
	}
	void setTexture(unsigned char* texture, int x, int y)
	{
		this->texture->applyTexture(texture, x, y);
	}


	std::shared_ptr<ShaderResource> getShader()
	{
		return shader;
	}
	void setShader(std::shared_ptr<ShaderResource> shader)
	{
		this->shader = shader;
	}
	void setShader(const char* vertex, const char* fragment)
	{
		shader->loadVertexShader(vertex);
		shader->loadFragmentShader(fragment);
		shader->linkProgram();
		shader->getHandles();
	}
};