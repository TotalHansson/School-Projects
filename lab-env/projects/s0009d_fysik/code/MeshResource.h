#pragma once
#include "MathLib3D.h"
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include "TextureResource.h"

class MeshResource
{
private:
	std::vector<Vector3D> index;

	unsigned short indexCount;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	GLuint elementBuffer;
	
	Vector3D boundingBox[8];
	Vector3D centerOfMass;

	std::vector<float> completeBuffer;

	float* softwareVertexBuffer;
	int vertexBufferSize;
	int* softwareIndexBuffer;
	int indexBufferSize;

	bool hasUV;

public:
	MeshResource()
	{
	}

	~MeshResource()
	{
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteBuffers(1, &uvBuffer);
		glDeleteBuffers(1, &normalBuffer);
		glDeleteBuffers(1, &elementBuffer);
	}

	int getVertexSize()
	{
		if (this->hasUV)
			return 8;
		return 6;
	}
	Vector3D* getBoundingBox()
	{
		return this->boundingBox;
	}

	Vector3D getCenterOfMass()
	{
		return this->centerOfMass;
	}

	std::vector<float>* GetVBuffer()
	{
		return &this->completeBuffer;
	}
	float* getVertexBuffer()
	{
		return this->softwareVertexBuffer;
	}
	int getVertexBufferSize()
	{
		return this->vertexBufferSize;
	}
	int* getIndexBuffer()
	{
		return this->softwareIndexBuffer;
	}
	int getIndexBufferSize()
	{
		return this->indexBufferSize;
	}
	

	void loadOBJ(
		const char* filePath,
		std::vector<float> &completeBuffer,
		std::vector<int> &indices)
	{
		std::ifstream fileStream(filePath, std::ios::in);
		if (!fileStream.is_open()) 
		{
			fprintf(stderr, "Could not read file %s\n", filePath);
		}

		std::vector<float> position;
		std::vector<float> textureCoord;
		std::vector<float> normal;
		//std::vector<float> completeBuffer;
		int indiceCounter = 0;
		//std::vector<int> indices;


		/*
			I don't know what this parser is....
			It's a horrible monster that should be burned. There is a better one in the ray tracing project
		*/
		//Loop through obj file
		while (!fileStream.eof()) 
		{
			std::string line = "";
			std::getline(fileStream, line);
			if (line == "")
				continue;

			int i = 0;
			std::string task = "";
			for (; i < line.length(); i++)
			{
				if (line.at(i) == ' ' && task == "")
					continue;
				else if (line.at(i) == ' ')
					break;
				else
					task += line.at(i);
			}
			/////////////////////////////////////////////////////////////
			if (task == "v")
			{
				std::string item = "";
				for (; i < line.length(); i++)
				{
					if (line.at(i) == ' ' && item == "")
						continue;
					else if (line.at(i) == ' ')
					{
						position.push_back(std::stof(item));
						item = "";
					}
					else
						item += line.at(i);

					if (i == line.length() - 1 && item != "")
						position.push_back(std::stof(item));
				}
			}/////////////////////////////////////////////////////////////
			else if (task == "vt")
			{
				this->hasUV = true;
				int itemSaved = 0;
				std::string item = "";
				for (; i < line.length(); i++)
				{
					if (itemSaved == 2)
						break;
					if (line.at(i) == ' ' && item == "")
						continue;
					else if (line.at(i) == ' ')
					{
						textureCoord.push_back(std::stof(item));
						item = "";
						itemSaved++;
					}
					else
						item += line.at(i);

					if (i == line.length() - 1 && item != "")
						textureCoord.push_back(std::stof(item));
				}
			}/////////////////////////////////////////////////////////////
			else if (task == "vn")
			{
				std::string item = "";
				for (; i < line.length(); i++)
				{
					if (line.at(i) == ' ' && item == "")
						continue;
					else if (line.at(i) == ' ')
					{
						normal.push_back(std::stof(item));
						item = "";
					}
					else
						item += line.at(i);

					if (i == line.length() - 1 && item != "")
						normal.push_back(std::stof(item));
				}
			}/////////////////////////////////////////////////////////////
			else if (task == "f")
			{
				int counter = 0; //counter; 0 == vector, 1 == texture coordinate, 2 == normal
				int sides = 0;
				std::string item = "";
				for (; i < line.length(); i++)
				{
					if (line.at(i) == ' ' && item == "")
						continue;
					else if (line.at(i) == '/' && item == "")
					{
						counter++;
						continue;
					}
					else if (line.at(i) == '/')
					{
						int index = std::stoi(item) - 1;
						switch (counter)
						{
						case 0:
							completeBuffer.push_back(position.at(index * 3));
							completeBuffer.push_back(position.at(index * 3 + 1));
							completeBuffer.push_back(position.at(index * 3 + 2));
							if (sides < 3)
								indices.push_back(indiceCounter);
							else
							{
								indices.push_back(indiceCounter - 3);
								indices.push_back(indiceCounter - 1);
								indices.push_back(indiceCounter);
							}
							indiceCounter++;
							break;
						case 1:
							completeBuffer.push_back(textureCoord.at(index * 2));
							completeBuffer.push_back(textureCoord.at(index * 2 + 1));
							break;
						}
						counter++;
						item = "";
					}
					else if (line.at(i) == ' ')
					{
						int index = std::stoi(item) - 1;
						switch (counter)
						{
						case 0:
							completeBuffer.push_back(position.at(index * 3));
							completeBuffer.push_back(position.at(index * 3 + 1));
							completeBuffer.push_back(position.at(index * 3 + 2));
							if (sides < 3)
								indices.push_back(indiceCounter);
							else
							{
								indices.push_back(indiceCounter - 3);
								indices.push_back(indiceCounter - 1);
								indices.push_back(indiceCounter);
							}
							indiceCounter++;
							break;
						case 1:
							completeBuffer.push_back(textureCoord.at(index * 2));
							completeBuffer.push_back(textureCoord.at(index * 2 + 1));
							break;
						case 2:
							completeBuffer.push_back(normal.at(index * 3));
							completeBuffer.push_back(normal.at(index * 3 + 1));
							completeBuffer.push_back(normal.at(index * 3 + 2));
							break;
						}
						counter = 0;
						item = "";
						sides++;
					}
					else
						item += line.at(i);

					if (i == line.length() - 1 && item != "")
					{
						int index = std::stoi(item) - 1;
						switch (counter)
						{
						case 0:
							completeBuffer.push_back(position.at(index * 3));
							completeBuffer.push_back(position.at(index * 3 + 1));
							completeBuffer.push_back(position.at(index * 3 + 2));
							if (sides < 3)
								indices.push_back(indiceCounter);
							else
							{
								indices.push_back(indiceCounter - 3);
								indices.push_back(indiceCounter - 1);
								indices.push_back(indiceCounter);
							}
							indiceCounter++;
							break;
						case 1:
							completeBuffer.push_back(textureCoord.at(index * 2));
							completeBuffer.push_back(textureCoord.at(index * 2 + 1));
							break;
						case 2:
							completeBuffer.push_back(normal.at(index * 3));
							completeBuffer.push_back(normal.at(index * 3 + 1));
							completeBuffer.push_back(normal.at(index * 3 + 2));
							break;
						}
					}
				}
			}
		}
		//End loop
		fileStream.close();
	}
	

	// buf = x, y, z, u, v, xn, yn, zn
	void loadMeshFromFile(const char* filename)
	{
		
		std::vector<int> indices;
		loadOBJ(filename, completeBuffer, indices);


		float* vertexBuffer = completeBuffer.data();
		vertexBufferSize = completeBuffer.size();
		softwareVertexBuffer = new float[vertexBufferSize];
		memcpy(softwareVertexBuffer, (void*)vertexBuffer, sizeof(float) * vertexBufferSize);

		int* indexBuffer = indices.data();
		indexBufferSize = indices.size();
		softwareIndexBuffer = new int[indexBufferSize];
		memcpy(softwareIndexBuffer, (void*)indexBuffer, sizeof(int) * indexBufferSize);



		// Bounding box
		Vector3D max(-100000, -100000, -100000);
		Vector3D min( 100000,  100000,  100000);

		// Center of mass
		Vector3D CoM(0.0f, 0.0f, 0.0f);

		int vertSize = (hasUV)? 8 : 6;
		for (unsigned int i = 0; i < completeBuffer.size(); i += vertSize)
		{
			// Bounding box
			if (completeBuffer[i+0] < min[0])
				min[0] = completeBuffer[i+0];
			if (completeBuffer[i+1] < min[1])
				min[1] = completeBuffer[i+1];
			if (completeBuffer[i+2] < min[2])
				min[2] = completeBuffer[i+2];

			if (completeBuffer[i+0] > max[0])
				max[0] = completeBuffer[i+0];
			if (completeBuffer[i+1] > max[1])
				max[1] = completeBuffer[i+1];
			if (completeBuffer[i+2] > max[2])
				max[2] = completeBuffer[i+2];


			// Center of mass
			CoM = CoM + Vector3D(completeBuffer[i+0], completeBuffer[i+1], completeBuffer[i+2]);
		}

		fprintf(stderr, "filename: %s\n", filename);
		fprintf(stderr, "completeBuffer.size / 8: %f\n\n", completeBuffer.size() / 8.0f);

		this->boundingBox[0] = Vector3D(min[0], min[1], min[2]); // left down back
		this->boundingBox[1] = Vector3D(min[0], max[1], min[2]); // left up back
		this->boundingBox[2] = Vector3D(max[0], min[1], min[2]); // right down back
		this->boundingBox[3] = Vector3D(max[0], max[1], min[2]); // right up back
		this->boundingBox[4] = Vector3D(max[0], min[1], max[2]); // right down front
		this->boundingBox[5] = Vector3D(max[0], max[1], max[2]); // right up front
		this->boundingBox[6] = Vector3D(min[0], min[1], max[2]); // left down front
		this->boundingBox[7] = Vector3D(min[0], max[1], max[2]); // left up front

		this->centerOfMass = CoM / (completeBuffer.size() / vertSize);


		genBuffers(vertexBuffer, vertexBufferSize, indexBuffer, indexBufferSize);
	}

	
	void genBuffers(float* buf, int bufSize, int* indices, int indicesSize)
	{
		glGenBuffers(1, &this->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufSize * sizeof(float), buf, GL_STATIC_DRAW);

		glGenBuffers(1, &this->elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(int), indices, GL_STATIC_DRAW);
		
		this->indexCount = indicesSize;
	}


	void render()
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);


		if (this->hasUV)
		{
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 3));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 5));
		}
		else
		{
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 3), (GLvoid*)0);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 3), (GLvoid*)(sizeof(float) * 3));
		}

		

		glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}

	template <typename T, size_t N>
	inline
		size_t SizeOfArray(const T(&)[N])
	{
		return N;
	}
};