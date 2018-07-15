#pragma once

#include <algorithm>
#include <fstream>
#include <string>

#include "Nvx2Structs.h"
#include "Skeleton.h"


void readNax3File(const char *filename, Skeleton *s)
{
	printf("Reading animation file \"%s\"\n", filename);

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


	// Cast start of memory to a header and then move the pointer forward
	Nax3Header *header = (Nax3Header*)ptr;
	ptr += sizeof(Nax3Header);

	//printf("magic: %u\n", header->magic);
	//printf("numClips: %u\n", header->numClips);
	//printf("numKeys: %u\n", header->numKeys);


	if (header->numClips > 0)
	{
		for (int i = 0; i < header->numClips; i++)
		{
			Nax3Clip *clip = (Nax3Clip*)ptr;
			ptr += sizeof(Nax3Clip);


			// Create animation
			AnimationGroup group;

			
			// Add animation clip to the animation
			Nax3Clip c;
			std::memcpy(&c, clip, sizeof(Nax3Clip));
			group.clip = c;


			//printf("Clip %d:\n", i);
			//printf(" numCurves %u\n", clip->numCurves);
			//printf(" startKeyIndex %u\n", clip->startKeyIndex);
			//printf(" numKeys %u\n", clip->numKeys);
			//printf(" keyStride %u\n", clip->keyStride);
			//printf(" keyDuration %u\n", clip->keyDuration);
			//printf(" numEvents %u\n", clip->numEvents);
			//printf(" name %s\n", clip->name);


			for (int j = 0; j < clip->numEvents; j++)
			{
				Nax3AnimEvent *event = (Nax3AnimEvent*)ptr;
				ptr += sizeof(Nax3AnimEvent);

				// printf(" Event:\n");
				// printf("  name %s\n", event->name);
				// printf("  category %s\n", event->category);
				// printf("  keyIndex %u\n", event->keyIndex);
			}

			for (int j = 0; j < clip->numCurves; j++)
			{
				Nax3Curve *curve = (Nax3Curve*)ptr;
				ptr += sizeof(Nax3Curve);

				// Add the curves to the animation
				Nax3Curve n3c;
				std::memcpy(&n3c, curve, sizeof(Nax3Curve));
				group.curves.push_back(n3c);

				// printf(" Curve %d:\n", j);
				// printf("  firstKeyIndex %u\n", curve->firstKeyIndex);
				// printf("  isActive %d\n", curve->isActive);
				// printf("  isStatic %d\n", curve->isStatic);
				// printf("  curveType %d\n", curve->curveType);
				// printf("  staticKeyX %f\n", curve->staticKeyX);
				// printf("  staticKeyY %f\n", curve->staticKeyY);
				// printf("  staticKeyZ %f\n", curve->staticKeyZ);
				// printf("  staticKeyW %f\n", curve->staticKeyW);
				
			}//for numCurves


			// Add the animation to the skeleton
			s->addAnimGroup(group);

		}//for numClips


		// Loop over all keys (keyframes)
		Keys *keyArray = new Keys[header->numKeys];
		for (int i = 0; i < header->numKeys; i++)
		{
			keyArray[i] = *(Keys*)ptr;
			ptr += sizeof(Keys);
		}

		// Add animation key to the skeleton
		s->addKeys(keyArray, header->numKeys);

	}//if numClip > 0


	 fprintf(stderr, "Animations successfully loaded\n");

}