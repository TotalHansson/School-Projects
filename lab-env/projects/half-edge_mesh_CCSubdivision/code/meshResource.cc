#include "meshResource.h"

MeshResource::MeshResource()
{
}

MeshResource::~MeshResource()
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &elementBuffer);
}


float* MeshResource::GetVertexBuffer()
{
	return this->softwareVertexBuffer;
}
int MeshResource::GetVertexBufferSize()
{
	return this->vertexBufferSize;
}
int* MeshResource::GetIndexBuffer()
{
	return this->softwareIndexBuffer;
}
int MeshResource::GetIndexBufferSize()
{
	return this->indexBufferSize;
}


/*
	What is this monstrosity.... 
	I should just use my own importer
*/
void MeshResource::LoadOBJ(
	const char* filePath,
	std::vector<float> &completeBuffer,
	std::vector<int> &indices)
{
	std::ifstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open()) 
	{
		std::cout << "Could not read file " << filePath << std::endl;
	}

	std::vector<float> position;
	std::vector<float> textureCoord;
	std::vector<float> normal;
	int indiceCounter = 0;

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
void MeshResource::LoadMeshFromFile(const char* filename)
{
	std::vector<float> completeBuffer;
	std::vector<int> indices;
	LoadOBJ(filename, completeBuffer, indices);


	float* vertexBuffer = completeBuffer.data();
	vertexBufferSize = completeBuffer.size();
	softwareVertexBuffer = new float[vertexBufferSize];
	memcpy(softwareVertexBuffer, (void*)vertexBuffer, sizeof(float) * vertexBufferSize);

	int* indexBuffer = indices.data();
	indexBufferSize = indices.size();
	softwareIndexBuffer = new int[indexBufferSize];
	memcpy(softwareIndexBuffer, (void*)indexBuffer, sizeof(int) * indexBufferSize);


	GenBuffers(vertexBuffer, vertexBufferSize, indexBuffer, indexBufferSize);
}

	
void MeshResource::GenBuffers(float* buf, int bufSize, int* indices, int indicesSize)
{
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufSize * sizeof(float), buf, GL_STATIC_DRAW);

	glGenBuffers(1, &this->elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(int), indices, GL_STATIC_DRAW);
	
	this->indexCount = indicesSize;
}


void MeshResource::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 5));

	glDrawElements(GL_TRIANGLES, this->indexCount, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}