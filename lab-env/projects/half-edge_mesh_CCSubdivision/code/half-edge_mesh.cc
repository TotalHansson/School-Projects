#include "half-edge_mesh.h"


HEmesh::HEmesh()
{}


HEmesh::~HEmesh()
{
	for (int i = 0; i < this->verts.size(); i++)
		delete this->verts[i];

	for (int i = 0; i < this->edges.size(); i++)
		delete this->edges[i];
	
	for (int i = 0; i < this->faces.size(); i++)
		delete this->faces[i];
	
	for (int i = 0; i < this->normals.size(); i++)
		delete[] this->normals[i];
	
	for (int i = 0; i < this->uvs.size(); i++)
		delete[] this->uvs[i];
}


void HEmesh::Render(int nrFaces)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * (3 + 2 + 3), (GLvoid*)(sizeof(float) * 5));


	// GL_QUADS is faster than GL_TRIANGLES even though it's depricated
	// glDrawArrays(GL_TRIANGLES, 0, this->faces.size() * 6);
	glDrawArrays(GL_QUADS, 0, this->faces.size() * 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void HEmesh::GenBuffers()
{
	// float *vBuf = new float[this->faces.size() * 6 * (3 + 2 + 3)];
	float *vBuf = new float[this->faces.size() * 4 * (3 + 2 + 3)];
	int j = 0;

	for(int i = 0; i < this->faces.size(); i++)
	{
		HEedge *edge = this->faces[i]->edge;
		HEedge *next1 = edge->next;
		HEedge *next2 = next1->next;
		HEedge *next3 = next2->next;


		// Add 6 points per face
		vBuf[j++] = edge->vert->x;
		vBuf[j++] = edge->vert->y;
		vBuf[j++] = edge->vert->z;
		vBuf[j++] = edge->u;
		vBuf[j++] = edge->v;
		vBuf[j++] = edge->nx;
		vBuf[j++] = edge->ny;
		vBuf[j++] = edge->nz;

		vBuf[j++] = next1->vert->x;
		vBuf[j++] = next1->vert->y;
		vBuf[j++] = next1->vert->z;
		vBuf[j++] = next1->u;
		vBuf[j++] = next1->v;
		vBuf[j++] = next1->nx;
		vBuf[j++] = next1->ny;
		vBuf[j++] = next1->nz;

		vBuf[j++] = next2->vert->x;
		vBuf[j++] = next2->vert->y;
		vBuf[j++] = next2->vert->z;
		vBuf[j++] = next2->u;
		vBuf[j++] = next2->v;
		vBuf[j++] = next2->nx;
		vBuf[j++] = next2->ny;
		vBuf[j++] = next2->nz;


		// For GL_TRIANGLE
		// vBuf[j++] = edge->vert->x;
		// vBuf[j++] = edge->vert->y;
		// vBuf[j++] = edge->vert->z;
		// vBuf[j++] = edge->u;
		// vBuf[j++] = edge->v;
		// vBuf[j++] = edge->nx;
		// vBuf[j++] = edge->ny;
		// vBuf[j++] = edge->nz;

		// For GL_TRIANGLE
		// vBuf[j++] = next2->vert->x;
		// vBuf[j++] = next2->vert->y;
		// vBuf[j++] = next2->vert->z;
		// vBuf[j++] = next2->u;
		// vBuf[j++] = next2->v;
		// vBuf[j++] = next2->nx;
		// vBuf[j++] = next2->ny;
		// vBuf[j++] = next2->nz;

		vBuf[j++] = next3->vert->x;
		vBuf[j++] = next3->vert->y;
		vBuf[j++] = next3->vert->z;
		vBuf[j++] = next3->u;
		vBuf[j++] = next3->v;
		vBuf[j++] = next3->nx;
		vBuf[j++] = next3->ny;
		vBuf[j++] = next3->nz;
	}

	// Vertex buffer
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	// glBufferData(GL_ARRAY_BUFFER, this->faces.size() * 6 * (3 + 2 + 3) * sizeof(float), vBuf, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, this->faces.size() * 4 * (3 + 2 + 3) * sizeof(float), vBuf, GL_STATIC_DRAW);

	delete[] vBuf;
}


void HEmesh::CCSubdivide()
{
	subdivStart = std::chrono::system_clock::now();

	this->nrSubdivs++;
	fprintf(stderr, "Subdivision nr %2d:\n", this->nrSubdivs);


	// Calculate new face points ------------------------------------
	fprintf(stderr, "Generating face points...\n");
	start = std::chrono::system_clock::now();
	
		this->MakeFacePoints();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());


	// Calculate new edge points ------------------------------------
	fprintf(stderr, "Generating edge points...\n");
	start = std::chrono::system_clock::now();
	
		this->MakeEdgePoints();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());


	// Move old vertices --------------------------------------------
	fprintf(stderr, "Moving old vertices...\n");
	start = std::chrono::system_clock::now();
	
		this->UpdateVerts();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());



	// Split all the edges and link each face -----------------------
	fprintf(stderr, "Generating new faces and edges...\n");
	start = std::chrono::system_clock::now();
	
		this->SplitEdges();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());


	// Stitch the new mesh together ---------------------------------
	fprintf(stderr, "Stitching together the mesh...\n");
	start = std::chrono::system_clock::now();
	
		this->MakeNewFaces();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());


	// Reset tmp values ---------------------------------------------
	fprintf(stderr, "Resetting values...\n");
	start = std::chrono::system_clock::now();
	
		this->ResetValues();

	end = std::chrono::system_clock::now(); 
	elapsed_seconds = end-start;
	fprintf(stderr, "Done (%f s)\n", elapsed_seconds.count());


	// Subdiv done
	subdivEnd = std::chrono::system_clock::now(); 
	elapsed_seconds = subdivEnd-subdivStart;
	fprintf(stderr, "Subdivision complete (%f s)\n\n", elapsed_seconds.count());
}


HEface* HEmesh::MakeFace(HEvert *midPoint, HEedge *edge1, HEedge *edge4,
						 HEedge *uv1, HEedge *uv3, HEedge *uv4)
{
	HEface *face = new HEface();
	HEedge *e2 = new HEedge();
	HEedge *e3 = new HEedge();
	
	float nx = edge1->vert->x + edge1->next->vert->x + midPoint->x + edge4->vert->x;
	float ny = edge1->vert->y + edge1->next->vert->y + midPoint->y + edge4->vert->y;
	float nz = edge1->vert->z + edge1->next->vert->z + midPoint->z + edge4->vert->z;


	edge4->face = face;
	edge4->u = (uv1->u + edge1->u) * 0.5f;
	edge4->v = (uv1->v + edge1->v) * 0.5f;
	edge4->nx = nx;
	edge4->ny = ny;
	edge4->nz = nz;

	e3->vert = midPoint;
	e3->face = face;
	e3->next = edge4;
	e3->u = (uv1->u + edge1->u + uv3->u + uv4->u) * 0.25f;
	e3->v = (uv1->v + edge1->v + uv3->v + uv4->v) * 0.25f;
	e3->nx = nx;
	e3->ny = ny;
	e3->nz = nz;

	e2->vert = edge1->next->vert;
	e2->face = face;
	e2->next = e3;
	e2->u = (edge1->u + uv3->u) * 0.5f;
	e2->v = (edge1->v + uv3->v) * 0.5f;
	e2->nx = nx;
	e2->ny = ny;
	e2->nz = nz;

	edge1->next = e2;
	edge1->face = face;
	// edge1->u = 0;	// Already set
	// edge1->v = 0;
	edge1->nx = nx;
	edge1->ny = ny;
	edge1->nz = nz;


	// Save the two new edges
	this->edges.push_back(e2);
	this->edges.push_back(e3);

	
	face->edge = edge1;

	return face;
}

void HEmesh::MakeNewFaces()
{
	int faceSize = this->faces.size();
	for (int i = 0; i < faceSize; i++)
	{
		HEface *face = this->faces[i];
		HEedge *edge = face->edge;
		HEedge *next1 = edge->next;
		HEedge *next2 = next1->next;
		HEedge *next3 = next2->next;
		HEedge *next4 = next3->next;
		HEedge *next5 = next4->next;
		HEedge *next6 = next5->next;
		HEedge *next7 = next6->next;
		HEvert *midPoint = face->tmpPoint;


		// Save the edge and face points
		if (!next1->vert->inserted)
		{
			next1->vert->inserted = true;
			this->verts.push_back(next1->vert);
		}
		if (!next3->vert->inserted)
		{
			next3->vert->inserted = true;
			this->verts.push_back(next3->vert);
		}
		if (!next5->vert->inserted)
		{
			next5->vert->inserted = true;
			this->verts.push_back(next5->vert);
		}
		if (!next7->vert->inserted)
		{
			next7->vert->inserted = true;
			this->verts.push_back(next7->vert);
		}
		this->verts.push_back(midPoint);


		// Create (soon to be) new faces
		HEface *f2, *f3, *f4;


		// Update the original face first
		HEedge *e2 = new HEedge();
		HEedge *e3 = new HEedge();

		float nx = edge->vert->x + next1->vert->x + midPoint->x + next7->vert->x;
		float ny = edge->vert->y + next1->vert->y + midPoint->y + next7->vert->y;
		float nz = edge->vert->z + next1->vert->z + midPoint->z + next7->vert->z;


		next7->face = face;
		next7->u = (edge->u + next6->u) * 0.5f;
		next7->v = (edge->v + next6->v) * 0.5f;
		next7->nx = nx;
		next7->ny = ny;
		next7->nz = nz;

		e3->vert = midPoint;
		e3->face = face;
		e3->next = next7;
		e3->u = (edge->u + next2->u + next4->u + next6->u) * 0.25f;
		e3->v = (edge->v + next2->v + next4->v + next6->v) * 0.25f;
		e3->nx = nx;
		e3->ny = ny;
		e3->nz = nz;

		e2->vert = edge->next->vert;
		e2->face = face;
		e2->next = e3;
		e2->u = (edge->u + next2->u) * 0.5f;
		e2->v = (edge->v + next2->v) * 0.5f;
		e2->nx = nx;
		e2->ny = ny;
		e2->nz = nz;

		edge->next = e2;
		// edge->face = face; 	// 
		// edge->u = edge->uf;	// Already true
		// edge->v = edge->vf;	//
		edge->nx = nx;
		edge->ny = ny;
		edge->nz = nz;


		midPoint->edge = e3;

		// Save the two new edges
		this->edges.push_back(e2);
		this->edges.push_back(e3);


		// Create the three new faces
		f2 = this->MakeFace(midPoint, next2, next1, edge, next4, next6);
		f3 = this->MakeFace(midPoint, next4, next3, next2, next6, edge);
		f4 = this->MakeFace(midPoint, next6, next5, next4, edge, next2);


		// Set the internal pairs for each face
		edge->next->pair = next2->next->next;
		next2->next->next->pair = edge->next;

		next2->next->pair = next4->next->next;
		next4->next->next->pair = next2->next;

		next4->next->pair = next6->next->next;
		next6->next->next->pair = next4->next;

		next6->next->pair = edge->next->next;
		edge->next->next->pair = next6->next;


		// Save the new faces
		this->faces.push_back(f2);
		this->faces.push_back(f3);
		this->faces.push_back(f4);
	}
}


void HEmesh::SplitEdges()
{
	// Split the current edges and pair the faces
	int edgeSize = this->edges.size();
	for (int i = 0; i < edgeSize; i++)
	{
		HEedge *edge = this->edges[i];

		// Skip if already split
		if(edge->alreadySplit)
			continue;

		// Mark this edge and it's pair as already split
		edge->alreadySplit = true;
		edge->pair->alreadySplit = true;

		
		// Create a new edge from edge point to next edge
		HEedge *newEdge = new HEedge();
		newEdge->alreadySplit = true;
		newEdge->vert = edge->tmpPoint;
		newEdge->next = edge->next;

		// Point the old edge to the new edge
		edge->next = newEdge;
		edge->tmpPoint->edge = newEdge;


		// Do the same for it's pair
		HEedge *newEdge2 = new HEedge();
		newEdge2->alreadySplit = true;
		newEdge2->vert = edge->tmpPoint;
		newEdge2->next = edge->pair->next;

		edge->pair->next = newEdge2;


		// Pair them up
		edge->next->pair = edge->pair;
		edge->pair->pair = edge->next;

		edge->pair->next->pair = edge;
		edge->pair = edge->pair->next;


		// Add them to the list
		this->edges.push_back(newEdge);
		this->edges.push_back(newEdge2);
	}
}


void HEmesh::UpdateVerts()
{
	std::vector<HEvert> newPos;
	HEvert v;

	for (int i = 0; i < this->verts.size(); i++)
	{
		HEvert *oldPos = this->verts[i];
		HEedge *start = oldPos->edge;
		HEedge *e = start;
		int n = 0;

		HEvert avgFacePoint;
		HEvert avgEdgePoint;


		// Add all face- and edge-points for the current vert
		do
		{
			avgFacePoint.x = avgFacePoint.x + e->face->tmpPoint->x;
			avgFacePoint.y = avgFacePoint.y + e->face->tmpPoint->y;
			avgFacePoint.z = avgFacePoint.z + e->face->tmpPoint->z;

			avgEdgePoint.x = avgEdgePoint.x + (oldPos->x + e->next->vert->x)*0.5f;
			avgEdgePoint.y = avgEdgePoint.y + (oldPos->y + e->next->vert->y)*0.5f;
			avgEdgePoint.z = avgEdgePoint.z + (oldPos->z + e->next->vert->z)*0.5f;

			n++;
			e = e->pair->next;
		} while (e != start);


		// Calc the average of the face- and edge-points
		avgFacePoint.x = avgFacePoint.x / n;
		avgFacePoint.y = avgFacePoint.y / n;
		avgFacePoint.z = avgFacePoint.z / n;
		avgEdgePoint.x = avgEdgePoint.x / n;
		avgEdgePoint.y = avgEdgePoint.y / n;
		avgEdgePoint.z = avgEdgePoint.z / n;

		// Move to the new position
		v.x = (avgFacePoint.x + avgEdgePoint.x*2.0f + oldPos->x * (n-3.0f)) / n;
		v.y = (avgFacePoint.y + avgEdgePoint.y*2.0f + oldPos->y * (n-3.0f)) / n;
		v.z = (avgFacePoint.z + avgEdgePoint.z*2.0f + oldPos->z * (n-3.0f)) / n;

		newPos.push_back(v);
	}

	for (int i = 0; i < this->verts.size(); i++)
	{
		this->verts[i]->x = newPos[i].x;
		this->verts[i]->y = newPos[i].y;
		this->verts[i]->z = newPos[i].z;
	}
}


void HEmesh::MakeEdgePoints()
{
	HEedge *edge;

	for (int i = 0; i < this->edges.size(); i++)
	{
		edge = this->edges[i];

		// If my pair has a point, use the same point
		if (edge->pair->tmpPoint != nullptr)
		{
			edge->tmpPoint = edge->pair->tmpPoint;
			continue;
		}


		HEvert *v = new HEvert();

		v->x = (edge->vert->x + edge->next->vert->x + edge->face->tmpPoint->x +
					edge->pair->face->tmpPoint->x) * 0.25f;
		v->y = (edge->vert->y + edge->next->vert->y + edge->face->tmpPoint->y +
					edge->pair->face->tmpPoint->y) * 0.25f;
		v->z = (edge->vert->z + edge->next->vert->z + edge->face->tmpPoint->z +
					edge->pair->face->tmpPoint->z) * 0.25f;

		v->edge = edge;


		// Save the edge point
		this->edges[i]->tmpPoint = v;
	}
}


void HEmesh::MakeFacePoints()
{
	HEedge *edge;

	for (int i = 0; i < this->faces.size(); i++)
	{
		edge = this->faces[i]->edge;
		HEvert *v = new HEvert();

		v->x = (edge->vert->x + edge->next->vert->x + edge->next->next->vert->x +
					edge->next->next->next->vert->x) * 0.25f;
		v->y = (edge->vert->y + edge->next->vert->y + edge->next->next->vert->y +
					edge->next->next->next->vert->y) * 0.25f;
		v->z = (edge->vert->z + edge->next->vert->z + edge->next->next->vert->z +
					edge->next->next->next->vert->z) * 0.25f;

		this->faces[i]->tmpPoint = v;
	}
}


void HEmesh::ResetValues()
{
	for (int i = 0; i < this->edges.size(); i++)
	{
		this->edges[i]->tmpPoint = nullptr;
		this->edges[i]->alreadySplit = false;
	}

	for (int i = 0; i < this->faces.size(); i++)
		this->faces[i]->tmpPoint = nullptr;
}


void HEmesh::LoadMeshFromFile(const char* filePath)
{
	fprintf(stderr, "Loading file: %s\n", filePath);

	// Open file
	std::ifstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open()) 
	{
		fprintf(stderr, "Could not open mesh file %s\n", filePath);
		return;
	}


	//Loop through obj file
	while (!fileStream.eof()) 
	{
		std::string line = "";
		std::getline(fileStream, line);


		// Save all vertices
		if(line[0] == 'v' && line[1] == ' ')
		{
			HEvert *v = new HEvert();
			sscanf(line.c_str(), "v %f %f %f", &v->x, &v->y, &v->z);

			this->verts.push_back(v);
		}

		// Save all normals
		else if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
		{
			float *normal = new float[3];
			sscanf(line.c_str(), "vn %f %f %f", &normal[0], &normal[1], &normal[2]);

			this->normals.push_back(normal);
		}

		// Save all texture coordinates
		else if(line[0] == 'v' && line[1] == 't' && line[2] == ' ')
		{
			float *uv = new float[2];
			sscanf(line.c_str(), "vt %f %f", &uv[0], &uv[1]);

			this->uvs.push_back(uv);
		}

		// Connect all vertices with edges and faces
		else if(line[0] == 'f' && line[1] == ' ')
		{
			HEface *face = new HEface();
			int v[4] = {0}, t[4] = {0}, n[4] = {0};


			// Check if the format is d/d/d or d//d
			sscanf(line.c_str(), "f %d/%d/%d", &v[0], &t[0], &n[0]);

			// Read without uv (d//d)
			if(t[0] == 0 && n[0] == 0)
			{
				sscanf(line.c_str(), "f %d//%d %d//%d %d//%d %d//%d",
				 &v[0], &n[0], &v[1], &n[1], &v[2], &n[2], &v[3], &n[3]);
			}

			// Read with uv (d/d/d)
			else
			{
				sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				 &v[0], &t[0], &n[0], &v[1], &t[1], &n[1], &v[2], &t[2], &n[2], &v[3], &t[3], &n[3]);
			}



			HEedge *edge1 = new HEedge();
			HEedge *edge2 = new HEedge();
			HEedge *edge3 = new HEedge();
			HEedge *edge4 = new HEedge();

			// Obj index start at 1, therefore -1
			edge1->vert = this->verts[v[0]-1];
			edge1->face = face;
			edge1->next = edge2;
			edge1->nx = this->normals[n[0]-1][0];
			edge1->ny = this->normals[n[0]-1][1];
			edge1->nz = this->normals[n[0]-1][2];

			edge2->vert = this->verts[v[1]-1];
			edge2->face = face;
			edge2->next = edge3;
			edge2->nx = this->normals[n[0]-1][0];
			edge2->ny = this->normals[n[0]-1][1];
			edge2->nz = this->normals[n[0]-1][2];

			edge3->vert = this->verts[v[2]-1];
			edge3->face = face;
			edge3->next = edge4;
			edge3->nx = this->normals[n[0]-1][0];
			edge3->ny = this->normals[n[0]-1][1];
			edge3->nz = this->normals[n[0]-1][2];

			edge4->vert = this->verts[v[3]-1];
			edge4->face = face;
			edge4->next = edge1;
			edge4->nx = this->normals[n[0]-1][0];
			edge4->ny = this->normals[n[0]-1][1];
			edge4->nz = this->normals[n[0]-1][2];


			// Save the four edges of the quad
			this->edges.push_back(edge1);
			this->edges.push_back(edge2);
			this->edges.push_back(edge3);
			this->edges.push_back(edge4);


			// Set the edge pointer of the vertices
			this->verts[v[0]-1]->edge = edge1;
			this->verts[v[1]-1]->edge = edge2;
			this->verts[v[2]-1]->edge = edge3;
			this->verts[v[3]-1]->edge = edge4;


			// Set uv and normals for each vertex
			if(t[0] != 0 && n[0] != 0)
			{
				for(int i = 0; i < 4; i++)
				{
					this->verts[v[i]-1]->edge->u = this->uvs[t[i]-1][0];
					this->verts[v[i]-1]->edge->v = this->uvs[t[i]-1][1];
				}
			}


			// Save the face between the four edges
			face->edge = edge1;
			this->faces.push_back(face);
		}
	}// while


	// Find all edge pairs
	int edgeSize = this->edges.size();
	for (int i = 0; i < edgeSize; i++)
	{
		for (int j = 0; j < edgeSize; j++)
		{
			if (this->edges[i]->vert == this->edges[j]->next->vert &&
				this->edges[i]->next->vert == this->edges[j]->vert)
			{
				// Add both pairs at the same time and then check for pairs before starting the inner loop
				this->edges[i]->pair = this->edges[j];
				break;
			}
		}
	}


	// Close file
	fileStream.close();

	fprintf(stderr, "Done reading file\n");
}