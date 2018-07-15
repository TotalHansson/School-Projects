#pragma once

#include <vector>

#include "MathLib3D.h"
#include "gameObject.h"
#include "EPA.h"


class GJK
{
public:
	GJK()
	{

	}
	~GJK()
	{}

	// Get the furthest point in a given direction
	static Vector3D MaxPointAlongDirection(const Vector3D direction, const GameObject *go)
	{
		// Get OBB
		Vector3D *obb = go->graphicsNode->getMesh()->getBoundingBox();
		Matrix3D transform = go->rigidBody->GetTransform();

		// Get the point with the largest dot product
		float max = -INFINITY;
		Vector3D pos;
		for (unsigned int i = 0; i < 8; i++)
		{
			Vector3D tmpPos = transform * obb[i];
			float dot = tmpPos.dot3(direction);

			if (dot > max)
			{
				max = dot;
				pos = tmpPos;
			}
		}

		return pos;
	}


	static bool TestOneFace(std::vector<Point> &list, Vector3D &direction,
		Vector3D &ab, Vector3D &ac, Vector3D &ao, Vector3D &abc)
	{
		if( abc.cross(ac).dot3(ao) >= 0)
		{
			// Keep A,C
			list[1] = list[2];
			list.pop_back();
			list.pop_back();

			direction = ac.cross(ao).cross(ac);

			return false;
		}


		return TestOneFacePart2(list, direction, ab, ac, ao, abc);
	}

	static bool TestOneFacePart2(std::vector<Point> &list, Vector3D &direction,
		Vector3D &ab, Vector3D &ac, Vector3D &ao, Vector3D &abc)
	{
		if(ab.cross(abc).dot3(ao) >= 0)
		{
			// Keep A,B
			list.pop_back();
			list.pop_back();

			direction = ab.cross(ao).cross(ab);

			return false;
		}

		// Keep A,B,C
		list.pop_back();

		direction = abc;

		return false;
	}

	static bool TestTwoFaces(std::vector<Point> &list, Vector3D &direction,
		Vector3D &ab, Vector3D &ac, Vector3D &ad, Vector3D &ao, Vector3D &abc, Vector3D &acd)
	{
		if(abc.cross(ac).dot3(ao) >= 0)
		{
			list[1] = list[2];
			list[2] = list[3];

			ab = ac;
			ac = ad;

			abc = acd;

			return TestOneFace(list, direction, ab, ac, ao, abc);
		}

		return TestOneFacePart2(list, direction, ab, ac, ao, abc);
	}

	// If the simplex contains a line
	static void DoLine(std::vector<Point> &list, Vector3D &direction)
	{
		Vector3D ao = -list[0].v;
		Vector3D ab = list[1].v - list[0].v;

		// If the line(ab) is the closest
		if (ab.dot3(ao) > 0)
		{
			// List stays the same, contains the line

			// New direction form line to Origo
			direction = ab.cross(ao).cross(ab);
		}

		// Point A is the closest
		else
		{
			// List now only contains A
			list.pop_back();

			// New direction is towards Origo
			direction = ao;
		}
	}

	// If the simplex contains a triangle
	static void DoTriangle(std::vector<Point> &list, Vector3D &direction)
	{
		Vector3D ao = -list[0].v;
		Vector3D ab = list[1].v - list[0].v;
		Vector3D ac = list[2].v - list[0].v;
		Vector3D abc = ab.cross(ac);
		Vector3D abn = ab.cross(abc);
		Vector3D acn = abc.cross(ac);

		
		if (acn.dot3(ao) > 0)
		{
			if (ac.dot3(ao) > 0)
			{
				// List contains A,C
				list[1] = list[2];
				list.pop_back();

				direction = ac.cross(ao).cross(ac);
			}
			else
			{
				if (ab.dot3(ao) > 0)
				{
					// List contains A,B
					list.pop_back();

					direction = ab.cross(ao).cross(ab);
				}
				else
				{
					// List contains A
					list.pop_back();
					list.pop_back();

					direction = ao;
				}
			}
		}

		else
		{
			if (abn.dot3(ao) > 0)
			{
				if (ab.dot3(ao) > 0)
				{
					// List contains A,B
					list.pop_back();

					direction = ab.cross(ao).cross(ab);
				}
				else
				{
					// List contains A
					list.pop_back();
					list.pop_back();

					direction = ao;
				}
			}
			else
			{
				if (abc.dot3(ao) > 0)
				{
					// List keeps A,B,C

					direction = abc;
				}
				else
				{
					// List swaps to A,C,B
					Point tmp = list[1];
					list[1] = list[2];
					list[2] = tmp;

					direction = -abc;
				}
			}
		}
	}

	// If the simple contains a tetrahedron
	static bool DoTetrahedron(std::vector<Point> &list, Vector3D &direction)
	{
		// fprintf(stderr, "DoTetrahedron---\n");

		Vector3D ao = -list[0].v;
		Vector3D ab = list[1].v - list[0].v;
		Vector3D ac = list[2].v - list[0].v;
		Vector3D ad = list[3].v - list[0].v;

		Vector3D abc = ab.cross(ac);
		Vector3D acd = ac.cross(ad);
		Vector3D adb = ad.cross(ab);

		Point tmp;

		const int overAbc = 0x1;
		const int overAcd = 0x2;
		const int overAdb = 0x4;

		/*
			Bit test.
			If Origo is "above" 0 planes 	-> Inside!
			If Origo is "above" 1 plane 	-> Do the same check as the triangle
			If Origo is "above" 2 planes	-> Check one, if it's there run the triangle case, otherwise run it on the other plane
		*/
		int planeTest =
			(abc.dot3(ao) > 0.0001f ? overAbc : 0) |
			(acd.dot3(ao) > 0.0001f ? overAcd : 0) |
			(adb.dot3(ao) > 0.0001f ? overAdb : 0);


		switch(planeTest)
		{	
			// Behind all 3 planes -> Inside the tetrahedron = collicion
			case 0:
				return true;


			case overAbc:
				return TestOneFace(list, direction, ab, ac, ao, abc);


			case overAcd:
				list[1] = list[2];	// b = c
				list[2] = list[3];	// c = d

				return TestOneFace(list, direction, ac, ad, ao, acd);


			case overAdb:
				list[2] = list[1];	// c = b
				list[1] = list[3];	// b = d

				return TestOneFace(list, direction, ad, ab, ao, adb);
				

			case overAbc | overAcd:
				return TestTwoFaces(list, direction, ab, ac, ad, ao, abc, acd);
				

			case overAcd | overAdb:
				tmp = list[1];
				list[1] = list[2];	// b = c
				list[2] = list[3];	// c = d
				list[3] = tmp;		// d = b

				return TestTwoFaces(list, direction, ac, ad, ab, ao, acd, adb);
				

			case overAdb | overAbc:
				tmp = list[2];
				list[2] = list[1];	// c = b
				list[1] = list[3];	// b = d
				list[3] = tmp;		// d = c

				return TestTwoFaces(list, direction, ad, ab, ac, ao, adb, abc);


			default:
				fprintf(stderr, "[GJK - DoTetrahedron]: Default case. This should hopefully not run\n");
				return true;

		}
	}


	static bool DoSimplex(std::vector<Point> &list, Vector3D &direction)
	{
		int size = list.size();

		if (size == 2)
			DoLine(list, direction);

		else if(size == 3)
			DoTriangle(list, direction);

		else if(size == 4)
			return DoTetrahedron(list, direction);

		return false;
	}


	// Implementing GJK - 2006
	// Casey Muratori
	// https://www.youtube.com/watch?v=Qupqu1xe7Io

	static bool GJKIntersect(const GameObject *A, const GameObject *B, CollisionInfo &info)
	{	
		int nrLine = 0;
		int nrTriangle = 0;
		int nrTetrahedron = 0;

		// Start by looking in some direction
		Point start = EPA::Support(Vector3D(0.0f, 1.0f, 0.0f), A, B);

		// Create a list and add the start value
		std::vector<Point> list;
		list.push_back(start);

		// Look from the starting point toward (0,0,0)
		Vector3D direction = -start.v;

		while(true)
		{	
			// Get the point opposite to the start 
			Point newPoint = EPA::Support(direction, A, B);

			// If we did not pass Origo there is no overlap
			if (newPoint.v.dot3(direction) < 0)
				return false;

			// Add the newest point as the first element
			list.insert(list.begin(), newPoint);


			if(list.size() == 2)
				nrLine++;
			else if(list.size() == 3)
				nrTriangle++;
			else if(list.size() == 4)
				nrTetrahedron++;

			
			if (DoSimplex(list, direction))
			{
				// fprintf(stderr, "Exiting GJK after %i, %i, %i\n", nrLine, nrTriangle, nrTetrahedron);


				info = EPA::GetCollisionInfo(list, A, B);

				return true;
			}
		}
	}
	
};