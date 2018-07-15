#pragma once

#include <algorithm>
#include <fstream>
#include <string>

#include "DDSStructs.h"
#include "Skeleton.h"

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)	\
		((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | 	\
		((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif


DDSFormat getDDSFormat(const DDSPixelFormat &format)
{
	DDSFormat f = FORMAT_UNKNOWN;

	// Return if unkown format
	if ((format.flags & DDPF_FOURCC) == 0)
	{
		fprintf(stderr, "format.flags & DDPF_FOURCC == 0\n");
		return f;
	}

	// Determin what format the file has
	switch (format.fourCC)
	{
		case MAKEFOURCC('D', 'X', 'T', '1'):
			// fprintf(stderr, "DDSFormat DXT1\n");
			f = FORMAT_DXT1;
			break;
		case MAKEFOURCC('D', 'X', 'T', '3'):
			// fprintf(stderr, "DDSFormat DXT3\n");
			f = FORMAT_DXT3;
			break;
		case MAKEFOURCC('D', 'X', 'T', '5'):
			// fprintf(stderr, "DDSFormat DXT5\n");
			f = FORMAT_DXT5;
			break;

		case MAKEFOURCC('A', 'T', 'I', '1'):
		case MAKEFOURCC('B', 'C', '4', 'U'):
			// fprintf(stderr, "DDSFormat BC4\n");
			f = FORMAT_BC4;
			break;

		case MAKEFOURCC('B', 'C', '4', 'S'):
			// fprintf(stderr, "DDSFormat BC4s\n");
			f = FORMAT_BC4s;
			break;

		case MAKEFOURCC('A', 'T', 'I', '2'):
		case MAKEFOURCC('B', 'C', '5', 'U'):
			// fprintf(stderr, "DDSFormat BC5\n");
			f = FORMAT_BC5;
			break;

		case MAKEFOURCC('B', 'C', '5', 'S'):
			// fprintf(stderr, "DDSFormat BC5s\n");
			f = FORMAT_BC5s;
			break;

		default:
			fprintf(stderr, "DDSFormat none (switch default)\n");
			break;
	}

	return f;
}

DDSFormat getDX10Format(DXGIFormat format)
{
	DDSFormat f = FORMAT_UNKNOWN;

	switch (format)
	{
		// DXT1
		case DXGI_FORMAT_BC1_TYPELESS:case DXGI_FORMAT_BC1_UNORM:case DXGI_FORMAT_BC1_UNORM_SRGB:
			f = FORMAT_DXT1; break;

		// DXT3
		case DXGI_FORMAT_BC2_TYPELESS:case DXGI_FORMAT_BC2_UNORM:case DXGI_FORMAT_BC2_UNORM_SRGB:
			f = FORMAT_DXT3; break;

		// DXT5
		case DXGI_FORMAT_BC3_TYPELESS:case DXGI_FORMAT_BC3_UNORM:case DXGI_FORMAT_BC3_UNORM_SRGB:
			f = FORMAT_DXT5; break;

		// BC4
		case DXGI_FORMAT_BC4_TYPELESS:case DXGI_FORMAT_BC4_UNORM:
			f = FORMAT_BC4;	break;
		case DXGI_FORMAT_BC4_SNORM:
			f = FORMAT_BC4s; break;

		// BC5
		case DXGI_FORMAT_BC5_TYPELESS:case DXGI_FORMAT_BC5_UNORM:
			f = FORMAT_BC5; break;
		case DXGI_FORMAT_BC5_SNORM:
			f = FORMAT_BC5s; break;

		// BC6H
		case DXGI_FORMAT_BC6H_TYPELESS:case DXGI_FORMAT_BC6H_UF16:
			f = FORMAT_BC6H; break;
		case DXGI_FORMAT_BC6H_SF16:
			f = FORMAT_BC6Hs; break;

		// BC7
		case DXGI_FORMAT_BC7_TYPELESS:case DXGI_FORMAT_BC7_UNORM:
			f = FORMAT_BC7; break;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			f = FORMAT_BC7srgb; break;

		default:
			break;
	}

	return f;
}


long getDDSTextureSize(DDSFormat format, int width, int height)
{
	long size = 0;

	// Only deal with block-compressed formats
	switch (format)
	{
		case FORMAT_DXT1:
		case FORMAT_DXT3:
		case FORMAT_DXT5:
		case FORMAT_BC5s:
		case FORMAT_BC5:
		case FORMAT_BC7:
		case FORMAT_BC7srgb:
		{
			int blocksWide = 0;
			if (width > 0)
				blocksWide = std::max(1, (width+3)/4);

			int blocksHigh = 0;
			if (height > 0)
				blocksHigh = std::max(1, (height+3)/4);

			int bytesPerBlock = 16;
			if (format == FORMAT_DXT1)
				bytesPerBlock = 8;

			size = blocksWide * blocksHigh * bytesPerBlock;
			break;
		}

		default:
			break;

	}

	return size;
}


void readDDSFile(const char* filename, Skeleton *s, DDSType type)
{
	if (type == DDSTYPE_DIFFUSE)
		printf("Reading DDS file \"%s\" as diffuse texture\n", filename);
	else if (type == DDSTYPE_NORMAL)
		printf("Reading DDS file \"%s\" as normal map\n", filename);
	else if (type == DDSTYPE_SPECULAR)
		printf("Reading DDS file \"%s\" as specular map\n", filename);
	

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

		// Read file to a buffer
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



	// First 4 bytes are "DDS "
	if (ptr[0] != 'D' || ptr[1] != 'D' || ptr[2] != 'S' || ptr[3] != ' ')
	{
		fprintf(stderr, "File is not a DDS file (Does not start with \"DDS \")\n");
		return;
	}
	ptr += sizeof(uint32_t);

	// Cast memory to a header and then move the pointer forward
	DDSHeader *header = (DDSHeader*)ptr;
	ptr += sizeof(DDSHeader);


	// fprintf(stderr, "size: %d\n", header->size);
	// fprintf(stderr, "flags: %d\n", header->flags);
	// fprintf(stderr, "height: %d\n", header->height);
	// fprintf(stderr, "width: %d\n", header->width);
	// fprintf(stderr, "pitchOrLinearSize: %d\n", header->pitchOrLinearSize);
	// fprintf(stderr, "depth: %d\n", header->depth);
	// fprintf(stderr, "mipMapCount: %d\n", header->mipMapCount);

	// fprintf(stderr, "format.size: %d\n", header->format.size);
	// fprintf(stderr, "format.flags: %d\n", header->format.flags);
	// fprintf(stderr, "format.fourCC: %d\n", header->format.fourCC);


	DDSFormat format;

	// Check for DX10 extended header
	if ((header->format.flags & DDPF_FOURCC) && (header->format.fourCC == MAKEFOURCC('D', 'X', '1', '0')))
	{
		// fprintf(stderr, "DDS is in DX10 format\n");

		// Get the dx10 header
		DDSHeader10 *header10 = (DDSHeader10*)ptr;
		ptr += sizeof(DDSHeader10);


		// fprintf(stderr, "dx10 - dxgiFormat: %d\n", header10->dxgiFormat);
		// fprintf(stderr, "dx10 - resourceDimension: %d\n", header10->resourceDimension);
		// fprintf(stderr, "dx10 - miscFlag: %d\n", header10->miscFlag);
		// fprintf(stderr, "dx10 - arraySize: %d\n", header10->arraySize);
		// fprintf(stderr, "dx10 - miscFlag2: %d\n", header10->miscFlag2);


		// Only support for 2D textures (not 1D or 3D)
		switch (header10->resourceDimension)
		{
			case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
			case D3D10_RESOURCE_DIMENSION_UNKNOWN:
				break;

			default:
				fprintf(stderr, "Invalid DX10 resource dimension: %d\n", header10->resourceDimension);
				return;
		}


		// No support for texture arrays
		if (header10->arraySize > 1)
		{
			printf("Parser does not support textyre arrays\n");
			return;
		}


		// Check dx10 format
		format = getDX10Format(header10->dxgiFormat);
		
	}// if dx10
	else
	{
		// Check the format of the file
		format = getDDSFormat(header->format);
	}


	if (format == FORMAT_UNKNOWN)
	{
		fprintf(stderr, "Invalid DDS file format\n");
		return;
	}
	else
	{
		// fprintf(stderr, "File format: %d\n", format);
	}
	



	int w = header->width;
	int h = header->height;
	int mips = std::max((int)header->mipMapCount, 1);
	std::vector<Texture> textureData;

	// Loop over all mip-maps
	for (unsigned int i = 0; i < mips; i++)
	{
		Texture tex;
		tex.width = w;
		tex.height = h;
		tex.format = format;

		tex.dataSize = getDDSTextureSize(format, tex.width, tex.height);

		// Verify data size
		if (tex.dataSize == 0 || tex.dataSize > fileSize)
		{
			fprintf(stderr, "Invalid data size: size %ld, max size %ld\n", tex.dataSize, fileSize);
			return;
		}


		tex.data = ptr;
		ptr += tex.dataSize;

		textureData.push_back(tex);
		

		w = std::max(w/2, 1);
		h = std::max(h/2, 1);
	}


	// Send the vector with the texture data to the skeleton
	s->setTexture(textureData, type);

}