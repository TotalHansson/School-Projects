#pragma once

#include <vector>

#include "MathLib3D.h"
#include "GraphicsNode.h"
#include "Nax3Structs.h"
#include "DDSStructs.h"
#include "ShaderResource.h"



class Joint
{
public:
	const char* name;
	int index;

	Matrix3D originalRelative;
	Matrix3D inverseBindPose;
	Matrix3D relativeToParent;
	Matrix3D absolutePosition;

	int parentIndex;
	Joint* parent = nullptr;
	std::vector<Joint*> children;


	// Sets the absolute world position to the parents absolute position * this relative offset
	void setAbsolutePosition(Matrix3D parentPosition)
	{
		this->absolutePosition = parentPosition * this->relativeToParent;

		for(unsigned int i = 0; i < children.size(); i++)
		{
			children[i]->setAbsolutePosition(this->absolutePosition);
		}
	}

	// Saves the inverted starting position of the joints
	void setInverseBindPose()
	{
		this->inverseBindPose = this->absolutePosition.invert();

		for (unsigned int i = 0; i < children.size(); i++)
		{
			children[i]->setInverseBindPose();
		}
	}

	void reset()
	{
		this->relativeToParent = this->originalRelative;
	}
};


class Skeleton
{
public:
	Skeleton()
	{}
	~Skeleton()
	{
		// delete[] this->keys;
	}


	Keys keyLerp(Keys a, Keys b, float t)
	{
		// float tmp = (1-t)*a + t*b;
		Keys tmp;
		tmp.v[0] = (1-t)*a.v[0] + t*b.v[0];
		tmp.v[1] = (1-t)*a.v[1] + t*b.v[1];
		tmp.v[2] = (1-t)*a.v[2] + t*b.v[2];
		tmp.v[3] = (1-t)*a.v[3] + t*b.v[3];

		return tmp;
	}


	Keys keySlerp(Keys a, Keys b, float t)
	{
		Keys key;

		// Normalize input
		a.v = a.v.normal();
		b.v = b.v.normal();


		float dot = a.v.dot(b.v);

		// Check if dot product is too close to 1
		const float dotThreshold = 0.999995f;
		if (fabs(dot) > dotThreshold)
		{
			// key.v = a.v + (b.v - a.v) * t;

			key.v = b.v - a.v;
			key.v = key.v * t;
			key.v = key.v + a.v;

			key.v = key.v.normal();
			return key;
		}

		// If dot is negative, flip one quaternion
		if (dot < 0.0f)
		{
			b.v = -b.v;
			dot = -dot;
		}

		// Clamp dot product
		if (dot > 1.0f)
			dot = 1.0f;
		else if (dot < -1.0f)
			dot = -1.0f;


		float theta = acos(dot) * t;

		key.v = b.v - a.v * dot;
		key.v = key.v.normal();

		key.v = a.v * cos(theta) + key.v * sin(theta);
		return key;
	}


	void animate(float dt, bool runAnim)
	{
		// Don't animate in bind pose
		if (this->bindPose)
			return;


		// Get current animation
		AnimationGroup *group = &this->animGroups[this->currentAnimClip];
		Nax3Clip *clip = &group->clip;


		// Only advance the anim if it's auto running
		if (runAnim)
		{
			// Advance time
			this->timeInFrame += dt * this->animSpeed;

			// Check how much time has passed in this frame
			if (this->timeInFrame >= 40)
			{
				this->timeInFrame -= 40;
				currentAnimFrame++;
				nextAnimFrame++;

				if (currentAnimFrame >= clip->numKeys)
					currentAnimFrame = 0;
				if (nextAnimFrame >= clip->numKeys)
					nextAnimFrame = 0;
			}
		}

		// Reset pose before applying next frame
		this->resetPose();


		// Update the joints relative position to an interpolated value between anim frames
		for (int i = 0; i < this->joints.size(); i++)
		{
			Matrix3D tmpMat;

			// Get current and next frame pose
			Keys *prevPos   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * currentAnimFrame) + 0/*Pos*/   + i*4/*Next joint*/];
			Keys *prevRot   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * currentAnimFrame) + 1/*Rot*/   + i*4/*Next joint*/];
			Keys *prevScale = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * currentAnimFrame) + 2/*Scale*/ + i*4/*Next joint*/];
			Keys *prevVel   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * currentAnimFrame) + 3/*Vel*/   + i*4/*Next joint*/];

			Keys *nextPos   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * nextAnimFrame) + 0/*Pos*/   + i*4/*Next joint*/];
			Keys *nextRot   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * nextAnimFrame) + 1/*Rot*/   + i*4/*Next joint*/];
			Keys *nextScale = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * nextAnimFrame) + 2/*Scale*/ + i*4/*Next joint*/];
			Keys *nextVel   = &keys[group->curves[0].firstKeyIndex + (clip->keyStride * nextAnimFrame) + 3/*Vel*/   + i*4/*Next joint*/];



			// Interpolate between the two frames based on time
			float frameWeight = this->timeInFrame / 40;
			Keys pos 	= keyLerp( *prevPos, 	*nextPos, 	frameWeight);
			Keys rot 	= keySlerp(*prevRot, 	*nextRot, 	frameWeight);
			Keys scale 	= keyLerp( *prevScale, 	*nextScale, frameWeight);
			Keys vel 	= keyLerp( *prevVel, 	*nextVel, 	frameWeight);
			

			// Apply the scale
			tmpMat.setScale(scale.v[0], scale.v[1], scale.v[2]);

			// Apply the rotation (not affected by scale)
			Vector3D quaternion;
			quaternion[0] = rot.v[0];
			quaternion[1] = rot.v[1];
			quaternion[2] = rot.v[2];
			quaternion[3] = rot.v[3];
			Matrix3D rotMat(quaternion);
			rotMat = rotMat.trans();
			tmpMat = tmpMat * rotMat;


			// SET the position
			tmpMat.translateTo(pos.v[0], pos.v[1], pos.v[2]);


			// Set the new relative position
			// this->joints[i].localMatrix = tmpMat;
			this->joints[i].relativeToParent = tmpMat;
		}
	}

	// Revert the skeleton to the bind pose
	void resetPose()
	{
		for (int i = 1; i < this->joints.size(); i++)
		{
			this->joints[i].reset();
		}
	}

	int getNrClips()
	{
		return this->animGroups.size();
	}

	const char* currentAnimName()
	{
		return this->animGroups[this->currentAnimClip].clip.name;
	}


	void nextFrame()
	{
		AnimationGroup *group = &this->animGroups[this->currentAnimClip];
		Nax3Clip *clip = &group->clip;

		this->timeInFrame = 0;
		currentAnimFrame++;
		nextAnimFrame++;

		if (currentAnimFrame >= clip->numKeys)
			currentAnimFrame = 0;
		if (nextAnimFrame >= clip->numKeys)
			nextAnimFrame = 0;
	}
	void prevFrame()
	{
		AnimationGroup *group = &this->animGroups[this->currentAnimClip];
		Nax3Clip *clip = &group->clip;

		this->timeInFrame = 0;
		currentAnimFrame--;
		nextAnimFrame--;

		if (currentAnimFrame <= 0)
			currentAnimFrame = clip->numKeys-1;
		if (nextAnimFrame <= 0)
			nextAnimFrame = clip->numKeys-1;
	}


	void nextAnimClip()
	{
		this->currentAnimClip++;
		if (this->currentAnimClip >= this->animGroups.size())
			this->currentAnimClip = 0;

		this->currentAnimFrame = 0;
		this->nextAnimFrame = 1;
		this->timeInFrame = 0;
	}
	void prevAnimClip()
	{
		this->currentAnimClip--;
		if (this->currentAnimClip < 0)
			this->currentAnimClip = this->animGroups.size()-1;

		this->currentAnimFrame = 0;
		this->nextAnimFrame = 1;
		this->timeInFrame = 0;
	}
	void changeAnimSpeed(float animSpeed)
	{
		this->animSpeed = animSpeed;
	}
	void setTPose(bool b)
	{
		// Toggle bind pose
		if (b)
			this->resetPose();

		this->bindPose = b;
	}


	void addKeys(Keys* keyArray, int nrKeys)
	{
		keys = keyArray;
	}
	void addAnimGroup(AnimationGroup group)
	{
		this->animGroups.push_back(group);
	}
	void addJoint(Joint j)
	{
		joints.push_back(j);
	}

	int numJoints()
	{
		return joints.size();
	}

	Joint* operator[](const int i)
	{
		if(i <= joints.size())
			return &joints[i];
		else
			return nullptr;
	}


	void generateBuffers()
	{
		glGenBuffers(1, &this->glBuffer);

		int p = 0;
		int n = 0;
		int u = 0;
		int t = 0;
		int bn = 0;
		int sw = 0;
		int si = 0;
		for (unsigned int i = 0; i < positions.size(); i+=3)
		{
			combinedBuffer.push_back(positions[p++]);
			combinedBuffer.push_back(positions[p++]);
			combinedBuffer.push_back(positions[p++]);

			combinedBuffer.push_back(normals[n++]);
			combinedBuffer.push_back(normals[n++]);
			combinedBuffer.push_back(normals[n++]);
			combinedBuffer.push_back(normals[n++]);

			combinedBuffer.push_back(uvs[u++]);
			combinedBuffer.push_back(uvs[u++]);

			combinedBuffer.push_back(tangents[t++]);
			combinedBuffer.push_back(tangents[t++]);
			combinedBuffer.push_back(tangents[t++]);

			combinedBuffer.push_back(binormals[bn++]);
			combinedBuffer.push_back(binormals[bn++]);
			combinedBuffer.push_back(binormals[bn++]);

			combinedBuffer.push_back(skinWeights[sw++]);
			combinedBuffer.push_back(skinWeights[sw++]);
			combinedBuffer.push_back(skinWeights[sw++]);
			combinedBuffer.push_back(skinWeights[sw++]);

			combinedBuffer.push_back(skinJIndices[si++]);
			combinedBuffer.push_back(skinJIndices[si++]);
			combinedBuffer.push_back(skinJIndices[si++]);
			combinedBuffer.push_back(skinJIndices[si++]);
		}

		glBindBuffer(GL_ARRAY_BUFFER, this->glBuffer);
		glBufferData(GL_ARRAY_BUFFER, combinedBuffer.size() * sizeof(float), combinedBuffer.data(), GL_STATIC_DRAW);
	}


	void updateMVPMatrix(Matrix3D modelPosition, Matrix3D view)
	{
		this->shader.useProgram();

		// Create projection matrix
		Matrix3D projection(50.0f, 1.0f, 100.0f);

		// Calculate MVP (and MV) matrix
		Matrix3D MVP = projection * view * modelPosition;
		Matrix3D MV = view * modelPosition;
		
		// Modify the uniforms for the shader
		this->shader.modifyMatrix4vf(MVP, "MVP");
		this->shader.modifyMatrix4vf(view, "V");
		this->shader.modifyMatrix4vf(modelPosition, "M");
		this->shader.modifyMatrix4vf(MV, "MV");
	}


	void sendJointsToShader()
	{
		this->shader.useProgram();

		std::vector<float> jointData;

		for (unsigned int i = 0; i < this->numJoints(); i++)
		{
			Matrix3D multipliedPose = joints[i].absolutePosition * joints[i].inverseBindPose;
			float *arr = multipliedPose.trans().getArray();

			for (unsigned int j = 0; j < 16; j++)
			{
				jointData.push_back(arr[j]);
			}

			delete[] arr;
		}

		this->shader.addMatrixArray(jointData.data(), this->numJoints(), "multipliedJoints");
	}


	void render()
	{
		this->shader.useProgram();

		// Bind the vertex array
		glBindBuffer(GL_ARRAY_BUFFER, this->glBuffer);

		// Bind the textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);
		this->shader.modifyInt(0, "diffuseTextureSampler");

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->normalTexture);
		this->shader.modifyInt(1, "normalTextureSampler");

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->specularTexture);
		this->shader.modifyInt(2, "specularTextureSampler");


		// Enable and set the vertex offset
		glEnableVertexAttribArray(0); 	// Position
		glEnableVertexAttribArray(1); 	// Normal
		glEnableVertexAttribArray(2); 	// UV
		glEnableVertexAttribArray(3); 	// Tangent
		glEnableVertexAttribArray(4); 	// Bitangent
		glEnableVertexAttribArray(5); 	// SkinWeights
		glEnableVertexAttribArray(6); 	// SkinJIndices

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 7));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 9));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 12));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 15));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 23, (GLvoid*)(sizeof(float) * 19));


		// Draw the mesh
		glDrawArrays(GL_TRIANGLES, 0, this->combinedBuffer.size()/23);



		// Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Disable vertex attribs
		glDisableVertexAttribArray(0); 	// Position
		glDisableVertexAttribArray(1); 	// Normal
		glDisableVertexAttribArray(2); 	// UV
		glDisableVertexAttribArray(3); 	// Tangent
		glDisableVertexAttribArray(4); 	// Bitangent
		glDisableVertexAttribArray(5); 	// SkinWeights
		glDisableVertexAttribArray(6); 	// SkinJIndices
	}

	void setShaders(const char* vertex, const char* fragment)
	{
		this->shader.loadVertexShader(vertex);
		this->shader.loadFragmentShader(fragment);
		this->shader.linkProgram();
		this->shader.getHandles();
	}

	void updateLight(PointLight light)
	{
		this->shader.useProgram();

		this->shader.modifyVector3fv(light.position, "lightPosition_worldspace");
		this->shader.modifyVector3fv(light.ambientColor, "ambientColor");
		this->shader.modifyVector3fv(light.diffuseColor, "diffuseColor");
		this->shader.modifyVector3fv(light.specularColor, "specularColor");
		this->shader.modifyFloat(light.intensity, "lightIntensity");
	}

	void setTexture(std::vector<Texture> textures, DDSType type)
	{
		this->shader.useProgram();

		// Generate and bind a new texture handle
		if (type == DDSTYPE_DIFFUSE)
		{
			glGenTextures(1, &this->diffuseTexture);
			glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);
		}
		else if (type == DDSTYPE_NORMAL)
		{
			glGenTextures(1, &this->normalTexture);
			glBindTexture(GL_TEXTURE_2D, this->normalTexture);
		}
		else if (type == DDSTYPE_SPECULAR)
		{
			glGenTextures(1, &this->specularTexture);
			glBindTexture(GL_TEXTURE_2D, this->specularTexture);
		}
		

		// Get the format of the texture
		GLenum format;
		switch (textures[0].format)
		{
			case FORMAT_DXT1:
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			case FORMAT_DXT3:
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			case FORMAT_DXT5:
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
		}

		// Send the texture data to GPU
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, i, format, textures[i].width, textures[i].height,
				0, textures[i].dataSize, textures[i].data);
		}

		// Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	


	// Graphics
	std::vector<float> combinedBuffer;
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> tangents;
	std::vector<float> binormals;
	std::vector<float> skinWeights;
	std::vector<float> skinJIndices;
	ShaderResource shader;

	// Hard coded indices are bad. Should read these from the skeleton file
	int indexArray[21] = {19, 18, 17, 0, 20, 15, 13, 1, 12, 9, 2, 10, 3, 11, 14, 16, 4, 5, 8, 6, 7};

private:
	GLuint glBuffer;
	GLuint diffuseTexture;
	GLuint normalTexture;
	GLuint specularTexture;

	std::vector<Joint> joints;
	bool bindPose = true;

	// Animation
	Keys *keys;
	std::vector<AnimationGroup> animGroups;

	int currentAnimClip = 0;

	float animSpeed = 1.0f;
	double timeInFrame = 0;
	int currentAnimFrame = 0;
	int nextAnimFrame = 1;
};