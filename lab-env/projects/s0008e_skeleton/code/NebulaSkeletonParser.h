#pragma once

#include "tinyxml.h"
#include "Skeleton.h"


void readNebulaSkeletonFile(const char *filename, Skeleton *s)
{
	// Read footman xml file
	TiXmlDocument doc(filename);

	if (doc.LoadFile())
	{
		fprintf(stderr, "Reading skeleton file \"%s\"\n", filename);
		TiXmlHandle hdoc(&doc);
		TiXmlElement* model;
		TiXmlElement* characterNode;

		// Model node
		model = hdoc.FirstChildElement().FirstChildElement().Element();

		// CharacterNode node
		characterNode = model->FirstChildElement("CharacterNodes")->FirstChildElement();


		// Loop over all Joints in CharacterNode
		for(TiXmlElement* j = characterNode->FirstChildElement("Joint"); j != NULL;
			j = j->NextSiblingElement())
		{
			Joint tmpJoint;
			tmpJoint.name = j->Attribute("name");
			tmpJoint.index = atoi(j->Attribute("index"));
			tmpJoint.parentIndex = atoi(j->Attribute("parent"));

			char* str;
			char* sub;

			// Read position from xml doc
			Matrix3D position;
			str = strdup(j->Attribute("position"));
			sub = strtok(str, ",");
			position[3] = atof(sub);
			sub = strtok(NULL, ",");
			position[7] = atof(sub);
			sub = strtok(NULL, ",");
			position[11] = atof(sub);
			sub = strtok(NULL, ",");
			position[15] = 1;//atof(sub);
			free(str);

			// Read scale from xml doc
			Matrix3D scale;
			str = strdup(j->Attribute("scale"));
			sub = strtok(str, ",");
			scale[0] = atof(sub);
			sub = strtok(NULL, ",");
			scale[5] = atof(sub);
			sub = strtok(NULL, ",");
			scale[10] = atof(sub);
			sub = strtok(NULL, ",");
			scale[15] = 1;//atof(sub);
			free(str);

			// Read rotation(quaternion) from doc
			Vector3D rot;
			str = strdup(j->Attribute("rotation"));
			sub = strtok(str, ",");
			rot[0] = atof(sub);
			sub = strtok(NULL, ",");
			rot[1] = atof(sub);
			sub = strtok(NULL, ",");
			rot[2] = atof(sub);
			sub = strtok(NULL, ",");
			rot[3] = atof(sub);
			free(str);

			// Convert rotation quaternion to matrix
			Matrix3D rotation(rot);

			// Transpose rotation matrix
			rotation = rotation.trans();

			// Calculate the local position matrix of this joint
			tmpJoint.relativeToParent = position * rotation * scale;
			tmpJoint.originalRelative = tmpJoint.relativeToParent;


			// Add the joint to the skeleton
			s->addJoint(tmpJoint);
		}
	}
	else // Can't read xml doc
	{
		fprintf(stderr, "TinyXML Error: %s\n", doc.ErrorDesc());
		return;
	}


	// Populate the children list for all joints
	for(int i = 0; i < s->numJoints(); i++)
	{
		if (i >= 1)
		{
			Joint *j = s->operator[](i);
			j->parent = s->operator[](j->parentIndex);
		}

		// Add this joint to parents children
		Joint* parent = s->operator[](i)->parent;
		if(parent != nullptr)
		{
			parent->children.push_back(s->operator[](i));
		}
	}

	// fprintf(stderr, "Skeleton successfully loaded\n");
}