#pragma once

#include <vector>

#include "MathLib3D.h"
#include "gameObject.h"

struct Point
{
	Vector3D v;

	Vector3D supA;
	Vector3D supB;

	Point()
	{}
	// Point(Vector3D p)
	// {
	// 	v = p;
	// }
	void operator=(const Point &p)
	{
		this->v = p.v;
		this->supA = p.supA;
		this->supB = p.supB;
	}
};

struct Edge
{
	float distance;
	// Vector3D normal;
	Point start;
	Point end;

	Edge(Point a, Point b)
	{
		start = a;
		end = b;
	}
};

struct Triangle
{
	float distance;
	Point points[3];
	Vector3D normal;

	Triangle(Point a, Point b, Point c)
	{
		points[0] = a;
		points[1] = b;
		points[2] = c;

		Vector3D u(b.v - a.v);
		Vector3D v(c.v - a.v);
		normal = u.cross(v);
		normal[3] = 0.0f;
		normal = normal.normal(); // normal
	}
};

struct CollisionInfo
{
	Vector3D worldPosition;
	Vector3D collisionNormal;
	float penetrationDepth;
};


class EPA
{
public:
	EPA();
	~EPA();


	// Get the furthest point in a given direction
	static Vector3D MaxPointAlongDirection(const Vector3D direction, const GameObject *go)
	{
		// std::vector<float> *vBuf = go->graphicsNode->getMesh()->GetVBuffer();
		float *vBuf = go->graphicsNode->getMesh()->getVertexBuffer();
		float size = go->graphicsNode->getMesh()->getVertexBufferSize();
		Matrix3D transform = go->rigidBody->GetTransform();

		// Get the point with the largest dot product
		float max = -INFINITY;
		Vector3D pos;
		

		for (unsigned int i = 0; i < size; i += 8)
		{
			Vector3D tmpPos = transform * Vector3D(vBuf[i], vBuf[i+1], vBuf[i+2]);
			float dot = tmpPos.dot3(direction);

			if (dot > max)
			{
				max = dot;
				pos = tmpPos;
			}
		}	


		return pos;
	}

	static Point Support(Vector3D direction, const GameObject *A, const GameObject *B)
	{
		direction[3] = 0;
		direction = direction.normal();

		Point p;
		p.supA = MaxPointAlongDirection( direction, A);
		p.supB = MaxPointAlongDirection(-direction, B);
		p.v = p.supA - p.supB;

		return p;
	}


	// Returns the face closest to Origo
	static int FindClosestFace(std::vector<Triangle> &faces)
	{
		int closestIndex = 0;
		float minDistance = INFINITY;

		// Loop over all faces and get the index of the face closes to Origo
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			faces[i].distance = (faces[i].points[0].v).dot3(faces[i].normal);

			if (faces[i].distance < minDistance)
			{
				minDistance = faces[i].distance;
				closestIndex = i;
			}
		}

		return closestIndex;
	}


	static void AddEdge(Point &a, Point &b, std::vector<Edge> &edges)
	{
		// If the edge is already in the list, remove it
		for (std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); ++it)
		{
			if (it->start.v == b.v && it->end.v == a.v)
			{
				edges.erase(it);
				return;
			}
		}

		// Else, add it
		edges.push_back(Edge(a, b));
	}

	static Vector3D Barycentric(const Vector3D &p, const Vector3D &a, const Vector3D &b, const Vector3D &c)
	{
		Vector3D v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = v0.dot3(v0);
		float d01 = v0.dot3(v1);
		float d11 = v1.dot3(v1);
		float d20 = v2.dot3(v0);
		float d21 = v2.dot3(v1);
		float denom = d00*d11 - d01*d01;

		float v = (d11*d20 - d01*d21) / denom;
		float w = (d00*d21 - d01*d20) / denom;
		float u = 1.0f - v - w;

		return Vector3D(u, v, w);
	}

	static CollisionInfo GetCollisionInfo(std::vector<Point> &list, const GameObject *A, const GameObject *B)
	{
		// Create the initial polytope from the tetrahedron built by GJK
		std::vector<Triangle> faces;
		faces.push_back(Triangle(list[0], list[1], list[2]));
		faces.push_back(Triangle(list[0], list[2], list[3]));
		faces.push_back(Triangle(list[0], list[3], list[1]));
		faces.push_back(Triangle(list[1], list[3], list[2]));


		// Edges that will be around the hole when faces are removed
		std::vector<Edge> edges;

		while(true)
		{
			// Get the closest face to Origo
			int closest = FindClosestFace(faces);

			// Find a new point in the closest face's normal direction
			Point newPoint = Support(faces[closest].normal, A, B);
			// newPoint[3] = 0.0f;

			float distance = newPoint.v.dot3(faces[closest].normal);



			// If the distance to newPoint is the same as the closest face we have the correct face
			if (distance - faces[closest].distance < 0.00001f)
			{
				Vector3D bary = Barycentric(faces[closest].normal * distance,
											faces[closest].points[0].v,
											faces[closest].points[1].v,
											faces[closest].points[2].v);

				Vector3D worldCollisionPoint = (bary[0] * faces[closest].points[0].supA +
												bary[1] * faces[closest].points[1].supA +
												bary[2] * faces[closest].points[2].supA);



				CollisionInfo info;
				info.worldPosition = worldCollisionPoint;
				info.collisionNormal = -faces[closest].normal;
				info.penetrationDepth = distance;

				return info;
			}


	
			// Remove all faces that can be seen from the new point and make
			//  a list of the edges that form the hole
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				// If the current face is visible from the new point
				// if (faces[i].normal.dot3(newPoint) > 0.00001f)
				if (faces[i].normal.dot3(newPoint.v - faces[i].points[0].v) > 0.00001f)
				{
					// Add all edges to the list
					AddEdge(faces[i].points[0], faces[i].points[1], edges);
					AddEdge(faces[i].points[1], faces[i].points[2], edges);
					AddEdge(faces[i].points[2], faces[i].points[0], edges);

					// Remove the face
					faces.erase(faces.begin()+i);
					i--;
				}
			}


			// Make a new face for each of the edges
			for (unsigned int i = 0; i < edges.size(); i++)
			{
				Triangle t(newPoint, edges[i].start, edges[i].end);

				faces.push_back(t);
			}


			// Remove any and all edges
			edges.clear();
		}
	}
	
};