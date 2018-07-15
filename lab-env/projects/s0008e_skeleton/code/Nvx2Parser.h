#pragma once

#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

#include "Nvx2Structs.h"
#include "Skeleton.h"


void readNvx2File(const char *filename, Skeleton *s)
{
	fprintf(stderr, "Reading mesh file \"%s\"\n", filename);
	
	size_t fileSize = 0;
	std::string contents;

	// Read file
	std::ifstream in(filename, std::ios::binary);
	if (in.is_open())
	{
		// Get file size
		in.seekg(0, std::ios::end);
		fileSize = in.tellg();
		in.seekg(0, std::ios::beg);

		// Read file into buffer
		contents.resize(fileSize);
		in.read(&contents[0], fileSize);
		in.close();
	}
	else
	{
		printf("ERROR: Could not read file\n");
		return;
	}

	// Create a pointer that will be moved along the file as it is parsed
	uchar *ptr = (uchar*)contents.data();

	// Cast start of memory to a header and then move the pointer forward
	Nvx2Header *header = (Nvx2Header*)ptr;
	ptr += sizeof(Nvx2Header);

	// printf("magic: %u\n", header->magic);
	// printf("numGroups: %u\n", header->numGroups);
	// printf("numVertices: %u\n", header->numVertices);
	// printf("vertexWidth: %u\n", header->vertexWidth);
	// printf("numIndices: %u\n", header->numIndices);
	// printf("numEdges: %u\n", header->numEdges);
	// printf("vertexCompnentMask: %u\n", header->vertexComponentMask);


	int groupDataSize = 6 * sizeof(uint) * header->numGroups;
	int vertexDataSize = header->numVertices * header->vertexWidth * sizeof(float);
	// numIndices *= 3 Because the Nebula source said so. ("header holds number of tris, not indices")
	int indexDataSize = header->numIndices * 3 * sizeof(int);

	uchar *groupDataPointer = ptr;
	uchar *vertexDataPointer = ((uchar*)groupDataPointer) + groupDataSize;
	uchar *indexDataPointer = ((uchar*)vertexDataPointer) + vertexDataSize;

	std::vector<VertexComponent> vertexComponents;

	
	for (unsigned int i = 0; i < header->numGroups; i++)
	{
		Nvx2Group *group = (Nvx2Group*)groupDataPointer;

		// Set the type and format of the vertex
		// https://github.com/gscept/nebula-trifid/blob/master/code/render/coregraphics/legacy/nvx2streamreader.cc
		for (unsigned int j = 0; j < N2NumVertexComponents; j++)
		{
			int sem;
			int fmt;
			uint index = 0;

			if (header->vertexComponentMask & (1<<j))
			{
				switch (1<<j)
				{
					case N2Coord: 			sem = NVX2SemanticName::Position; 		fmt = NVX2Format::Float3; 	break;
					case N2Normal: 			sem = NVX2SemanticName::Normal; 		fmt = NVX2Format::Float3; 	break;
					case N2NormalB4N: 		sem = NVX2SemanticName::Normal; 		fmt = NVX2Format::Byte4N; 	break;
					case N2Uv0: 			sem = NVX2SemanticName::TexCoord1; 		fmt = NVX2Format::Float2; 	index = 0; 	break;
					case N2Uv0S2: 			sem = NVX2SemanticName::TexCoord1; 		fmt = NVX2Format::Short2; 	index = 0; 	break;
					case N2Uv1: 			sem = NVX2SemanticName::TexCoord2; 		fmt = NVX2Format::Float2; 	index = 1; 	break;
					case N2Uv1S2: 			sem = NVX2SemanticName::TexCoord2; 		fmt = NVX2Format::Short2; 	index = 1; 	break;
					case N2Color: 			sem = NVX2SemanticName::Color; 			fmt = NVX2Format::Float4; 	break;
					case N2ColorUB4N: 		sem = NVX2SemanticName::Color; 			fmt = NVX2Format::UByte4N; 	break;
					case N2Tangent: 		sem = NVX2SemanticName::Tangent; 		fmt = NVX2Format::Float3; 	break;
					case N2TangentB4N: 		sem = NVX2SemanticName::Tangent; 		fmt = NVX2Format::Byte4N; 	break;
					case N2Binormal: 		sem = NVX2SemanticName::Binormal; 		fmt = NVX2Format::Float3; 	break;
					case N2BinormalB4N: 	sem = NVX2SemanticName::Binormal; 		fmt = NVX2Format::Byte4N; 	break;
					case N2Weights: 		sem = NVX2SemanticName::SkinWeights; 	fmt = NVX2Format::Float4; 	break;
					case N2WeightsUB4N: 	sem = NVX2SemanticName::SkinWeights; 	fmt = NVX2Format::UByte4N; 	break;
					case N2JIndices: 		sem = NVX2SemanticName::SkinJIndices; 	fmt = NVX2Format::Float4; 	break;
					case N2JIndicesUB4: 	sem = NVX2SemanticName::SkinJIndices; 	fmt = NVX2Format::UByte4; 	break;

					default:
						fprintf(stderr, "Invalid vertex component\n");
						sem = NVX2SemanticName::Position;
						fmt = NVX2Format::Float3;
					break;
				}

				vertexComponents.push_back(VertexComponent(sem, fmt, index));
			}	
		}

	}
	


	// Add all vertices to the skeletons vertex buffer
	for (unsigned int i = 0; i < header->numVertices; i++)
	{
		for (unsigned int j = 0; j < vertexComponents.size(); j++)
		{
			float *f = (float*)vertexDataPointer;
			char *c = (char*)vertexDataPointer;
			uchar *d = (uchar*)vertexDataPointer;

			switch(vertexComponents[j].fmt)
			{
				// UV
				case NVX2Format::Float2:
					vertexDataPointer += sizeof(float) * 2;

					if (vertexComponents[j].sem == NVX2SemanticName::TexCoord1)
					{
						s->uvs.push_back(f[0]);
						s->uvs.push_back(f[1]);
					}
					else
						fprintf(stderr, "Unused value. Semantic name: %d\n", vertexComponents[j].sem);
				break;

				// Position
				case NVX2Format::Float3:
					vertexDataPointer += sizeof(float) * 3;

					if (vertexComponents[j].sem == NVX2SemanticName::Position)
					{
						s->positions.push_back(f[0]);
						s->positions.push_back(f[1]);
						s->positions.push_back(f[2]);
					}
				break;

				// Normal
				case NVX2Format::Byte4N:
					vertexDataPointer += sizeof(char) * 4;

					if (vertexComponents[j].sem == NVX2SemanticName::Normal)
					{
						s->normals.push_back((float)c[0]);
						s->normals.push_back((float)c[1]);
						s->normals.push_back((float)c[2]);
						s->normals.push_back((float)c[3]);
					}
					else if(vertexComponents[j].sem == NVX2SemanticName::Tangent)
					{
						s->tangents.push_back((float)c[0]);
						s->tangents.push_back((float)c[1]);
						s->tangents.push_back((float)c[2]);
						s->tangents.push_back((float)c[3]);
					}
					else if(vertexComponents[j].sem == NVX2SemanticName::Binormal)
					{
						s->binormals.push_back((float)c[0]);
						s->binormals.push_back((float)c[1]);
						s->binormals.push_back((float)c[2]);
						s->binormals.push_back((float)c[3]);
					}
					else
						fprintf(stderr, "Unused value. Semantic name: %d\n", vertexComponents[j].sem);

				break;

				// Skin weight
				case NVX2Format::UByte4N:
					vertexDataPointer += sizeof(uchar) * 4;

					if (vertexComponents[j].sem == NVX2SemanticName::SkinWeights)
					{
						s->skinWeights.push_back((float)d[0]);
						s->skinWeights.push_back((float)d[1]);
						s->skinWeights.push_back((float)d[2]);
						s->skinWeights.push_back((float)d[3]);
					}
					else
						fprintf(stderr, "Unused value. Semantic name: %d\n", vertexComponents[j].sem);
					
				break;

				// Skin joint indices
				case NVX2Format::UByte4:
					vertexDataPointer += sizeof(uchar) * 4;

					if (vertexComponents[j].sem == NVX2SemanticName::SkinJIndices)
					{
						s->skinJIndices.push_back(s->indexArray[d[0]]);
						s->skinJIndices.push_back(s->indexArray[d[1]]);
						s->skinJIndices.push_back(s->indexArray[d[2]]);
						s->skinJIndices.push_back(s->indexArray[d[3]]);
					}
					else
						fprintf(stderr, "Unused value. Semantic name: %d\n", vertexComponents[j].sem);
				break;

				// case NVX2Format::Float4: vertexDataPointer += sizeof(float) * 4; break;
				// case NVX2Format::Short2: vertexDataPointer += sizeof(short) * 2; break;

				default:
					fprintf(stderr, "Got non-suported format %d\n", vertexComponents[j].fmt);
				break;
			}// switch
		}// for vertexComponent size
	}// for numVertices


	 fprintf(stderr, "Mesh successfully loaded\n");
}