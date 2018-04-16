#pragma once

#include <algorithm>

#include "MathLib3D.h"
#include "GraphicsNode.h"

struct State
{
	// Primary
	Vector3D position;
	Vector3D momentum;

	Vector3D orientation;		// Quaternion
	Vector3D angularMomentum;


	// Secondary
	Vector3D velocity;

	Vector3D spin;				// Quaternion
	Vector3D angularVelocity;

	Matrix3D inertia;
	Matrix3D inverseInertia;


	// Constat
	float mass;
	float inverseMass;

	Matrix3D inverseBodyInertia;



	void Reclalculate()
	{
		position[3] = 0;
		momentum[3] = 0;
		angularMomentum[3] = 0;
		velocity[3] = 0;
		angularVelocity[3] = 0;


		Matrix3D rotation(orientation);
		this->inverseInertia = rotation * inverseBodyInertia * rotation.trans();


		// Movement
		velocity = momentum * inverseMass;

		const float maxAngularMomentum = 10.0f;
		float x =  fminf(fmaxf(angularMomentum[0], -maxAngularMomentum), maxAngularMomentum);
		float y =  fminf(fmaxf(angularMomentum[1], -maxAngularMomentum), maxAngularMomentum);
		float z =  fminf(fmaxf(angularMomentum[2], -maxAngularMomentum), maxAngularMomentum);
		
		angularMomentum = Vector3D(x, y, z);

		angularVelocity = inverseInertia * angularMomentum;

		// Normalize the orientation "quaternion" to avoid drifting
		orientation = orientation.normal();
		
		spin = 0.5f * angularVelocity.quaternionMult(orientation);
	}
};

struct Derivative
{
	// Movements
	Vector3D velocity;
	Vector3D force;

	// Rotation
	Vector3D spin;		// Quaternion
	Vector3D torque;
};


class RigidBody
{
private:

	Matrix3D transform;
	bool isStatic;

	State state;


public:
	RigidBody()
	{
		this->isStatic = false;

		// Still really need vec3 so I don't have to set w to 0
		this->state.position[3] = 0;
		this->state.momentum[3] = 0;
		this->state.angularMomentum[3] = 0;

		this->state.velocity[3] = 0;
		this->state.angularVelocity[3] = 0;	

		// Default 1m^3 at 100 KG
		this->state.mass = 100.0f;

		// this->state.mass = 0.015f;
		this->state.inverseMass = 1.0f / this->state.mass;

		// Cube
		this->state.inverseBodyInertia[0]  = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
		this->state.inverseBodyInertia[5]  = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
		this->state.inverseBodyInertia[10] = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
	}

	~RigidBody()
	{
	}


	// Get information

	Vector3D GetPosition()
	{
		return this->state.position;
	}
	Vector3D GetMomentum()
	{
		return this->state.momentum;
	}
	Vector3D GetVelocity()
	{
		return this->state.velocity;
	}
	Vector3D GetAngularMomentum()
	{
		return this->state.angularMomentum;
	}
	Vector3D GetAngularVelocity()
	{
		return this->state.angularVelocity;
	}
	float GetMass()
	{
		if (this->isStatic)
			return 0.0f;
		return this->state.mass;
	}
	float GetInverseMass()
	{
		if (this->isStatic)
			return 0.0f;
		return this->state.inverseMass;
	}
	bool IsStatic()
	{
		return this->isStatic;
	}
	Matrix3D GetInverseInertia()
	{
		return this->state.inverseInertia;
	}
	Matrix3D GetTransform()
	{
		return this->transform;
	}



	// Instantaneous teleport to the target position
	void SetPosition(const Vector3D &v)
	{
		this->state.position = v;
		transform[3] = v[0];
		transform[7] = v[1];
		transform[11]= v[2];
	}

	// Instantly adds force to the objecs momentum
	void AddForce(Vector3D f)
	{
		f[3] = 0;
		this->state.momentum += f;

		this->state.Reclalculate();
	}
	void AddAngularMomentum(Vector3D f)
	{
		f[3] = 0;
		this->state.angularMomentum += f;

		this->state.Reclalculate();
	}

	// Adds rotation around the objects center of mass
	void AddTorque(Vector3D point, Vector3D force)
	{	
		// Get point relative to objects center of mass
		Vector3D localPos = point - this->state.position;

		// Get perpendicular axis to rotate about
		Vector3D rotationAxis = localPos.cross(force);
		this->state.angularMomentum += rotationAxis;

		this->state.Reclalculate();
	}

	// Apply a force to the center of mass (no rotation)
	void ApplyImpulse(Vector3D dir, float f)
	{
		// Make sure direction is normalized and then add force
		dir[3] = 0;
		dir = dir.normal() * f;

		// Impulse is added to center of mass point, so no torque is applied
		AddForce(dir);
	}

	// Apply a force a some point on the object causing movement and rotation
	void ApplyImpulseAtWorldPoint(Vector3D point, Vector3D dir, float f)
	{
		// Make sure direction is normalized and then add force
		dir[3] = 0;
		dir = dir.normal() * f;

		// Add the force for linear movement
		AddForce(dir);

		// Add torque at a specific point for rotation
		AddTorque(point, dir);
	}

	// Set the mass of the object and recalculate the inertia tensor
	void SetMass(float m)
	{
		this->state.mass = m;
		this->state.inverseMass = 1.0f / m;

		// Ellipsoid  1.0f * 1.0f  0.25f * 0.25f
		// a
		// this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));
		// this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		// this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		// b
		this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));
		this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.25f * 0.25f));
		// c
		// this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.5f * 0.5f));
		// this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.5f * 0.5f));
		// this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));

		this->state.Reclalculate();
	}

	// Set whether the object should move and react or not
	void SetStatic(bool b)
	{
		this->isStatic = b;
	}
	

	// Returns the velocity of the specified point
	Vector3D VelocityAtWorldPoint(const Vector3D &worldPoint)const
	{
		// Point in local space
		Vector3D localPoint = worldPoint - this->state.position;

		// Angular velocity based on where the point is relative to the rotation axis
		Vector3D vAngular = this->state.angularVelocity.cross(localPoint);

		// Total velocity is linear velocity + angular velocity
		Vector3D pointVelocity = this->state.velocity + vAngular;

		return pointVelocity;
	}



	// Here is where movement from forces is calculated
	Vector3D Force(const State &state, float dt)const
	{
		Vector3D gravity(0, -9.82f, 0);

		return gravity;
	}

	// Here is where rotation from torque is calculated
	Vector3D Torque(const State &state, float dt)const
	{
		Vector3D simpleSpin;// = Vector3D(1, 0, 0) - state.angularVelocity * 0.1f;

		return simpleSpin;
	}

	// Evaluates each step during the integration
	Derivative Evaluate(State current, float dt, const Derivative &d)
	{
		// Only changes for MidPoint and RK4
		current.position += d.velocity * dt;
		current.momentum += d.force * dt;

		current.orientation += d.spin * dt;
		current.angularMomentum += d.torque * dt;

		current.Reclalculate();


		// Get the added force and return the derivated values
		Derivative output;
		output.velocity = current.velocity;
		output.force = Force(current, dt);

		output.spin = current.spin;
		output.torque = Torque(current, dt);

		return output;
	}



	// Single step integration
	void Euler(State &state, float dt)
	{	
		// Integrate once
		Derivative a = Evaluate(state, dt, Derivative());

		// Explicit Euler
		state.position += a.velocity * dt;
		state.momentum += a.force * dt;

		state.orientation += a.spin * dt;
		state.angularMomentum += a.torque * dt;


		// Change to this for higher accuracy Semi-Implicit Euler
		// state.velocity += a.force * dt;
		// state.position += a.velocity * dt;

		// Recalculate derived state variables
		state.Reclalculate();
	}

	// Two step integration
	void MidPoint(State &state, float dt)
	{
		Derivative a, b;

		// Take the 4 steps
		a = Evaluate(state, 0.0f, Derivative());
		b = Evaluate(state, dt, a);

		state.position += b.velocity * dt;
		state.momentum += b.force * dt;

		state.orientation += a.spin * dt;
		state.angularMomentum += a.torque * dt;

		state.Reclalculate();
	}

	// Four step integration
	void RK4(State &state, float dt)
	{
		Derivative a, b, c, d;

		// Take the 4 steps
		a = Evaluate(state, 0.0f, Derivative());
		b = Evaluate(state, dt*0.5f, a);
		c = Evaluate(state, dt*0.5f, b);
		d = Evaluate(state, dt, c);

		// Apply with weight of the steps
		Vector3D dPosition = 1.0f / 6.0f * (a.velocity + 2.0f * (b.velocity + c.velocity) + d.velocity);
		Vector3D dMomentum = 1.0f / 6.0f * (a.force + 2.0f * (b.force + c.force) + d.force);

		Vector3D dOrientation = 1.0f / 6.0f * (a.spin + 2.0f * (b.spin + c.spin) + d.spin);
		Vector3D dAngularMomentum = 1.0f / 6.0f * (a.torque + 2.0f * (b.torque + c.torque) + d.torque);


		// Update the current state
		state.position += dPosition * dt;
		state.momentum += dMomentum * dt;

		state.orientation += dOrientation * dt;
		state.angularMomentum += dAngularMomentum * dt;

		// Recalculate derived state variables
		state.Reclalculate();
	}



	void Update(double dt)
	{
		if (this->isStatic)
		{
			this->state.momentum = Vector3D();
			this->state.velocity = Vector3D();
			this->state.angularMomentum = Vector3D();
			this->state.angularVelocity = Vector3D();
			return;
		}

		// Drag to reduce speed
		this->state.velocity = this->state.velocity * 0.995f;
		this->state.angularVelocity = this->state.angularVelocity * 0.995f;


		// Euler(this->state, dt);
		// MidPoint(this->state, dt);
		RK4(this->state, dt);


		// Set the final transform of the object
		this->transform = Matrix3D(this->state.orientation);
		this->transform.translateTo(this->state.position);
	}

};



class Ray
{
public:
	Vector3D start;
	Vector3D direction;
	Vector3D end;

	Ray(){}
	~Ray(){}

	Ray(double mouseX, double mouseY, int screenWidth, int screenHeight, Matrix3D view, Matrix3D projection, Vector3D position)
	{
		// Move (0,0) from top-left to bottom-left
		mouseY = screenHeight - mouseY;


		// Convert mouse coords to clip space [-1, 1]
		float x = ((float)mouseX/(float)screenWidth - 0.5f) * 2.0f;
		float y = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;


		// Clip space, -1 is the near clipping plane
		Vector3D start = Vector3D(x, y, -1.0f);
		Vector3D end = Vector3D(x, y, 0.0f);


		// Move start and end from clip to world space
		Matrix3D inverseM = (projection * view).invert();
		start = inverseM * start;
		start = start / start[3];
		end = inverseM * end;
		end = end / end[3];


		// Save the values in this object
		this->start = start;
		this->end = end;
		this->direction = (end - start).normal();
	}


	// Intersect box
	float AABBIntersect(const Vector3D &min, const Vector3D &max, Vector3D &intersectionPoint)
	{
		Vector3D tMin = (min - this->start) / this->direction;
		Vector3D tMax = (max - this->start) / this->direction;

		Vector3D t1, t2;

		t1[0] = fminf(tMin[0], tMax[0]);
		t1[2] = fminf(tMin[1], tMax[1]);
		t1[1] = fminf(tMin[2], tMax[2]);

		t2[0] = fmaxf(tMin[0], tMax[0]);
		t2[2] = fmaxf(tMin[1], tMax[1]);
		t2[1] = fmaxf(tMin[2], tMax[2]);


		float tNear = fmaxf(fmaxf(t1[0], t1[1]), t1[2]);
		float tFar = fminf(fminf(t2[0], t2[1]), t2[2]);


		if (tNear > 0.0f && tNear <= tFar)
		{
			intersectionPoint = this->start + this->direction * tNear;
			return tNear;
		}

		return -1.0f;
	}

	
	float OBBIntersect(const Vector3D &min, const Vector3D &max, Matrix3D transform, Vector3D &intersectionPoint)
	{
		float tNear = 0.0f;
		float tFar = 100000.0f;

		Vector3D obbPosition(transform[3], transform[7], transform[11]);
		Vector3D delta(obbPosition - this->start);

		// Test intersection with x axis
		{
			Vector3D xAxis(transform[0], transform[4], transform[8]);
			float e = xAxis.dot3(delta);
			float f = this->direction.dot3(xAxis);
			
			// If the line intersects the plane
			if (fabsf(f) > 0.001f)
			{
				float t1 = (e + min[0]) / f; // Intersection with the left plane
				float t2 = (e + max[0]) / f; // Intersection with the right plane


				// Make sure t1 is the closest point
				if (t1 > t2)
				{
					// Swap
					float tmp = t1;
					t1 = t2;
					t2 = tmp;
				}

				// tMax is the nearest "far" intersection
				if (t2 < tFar)
					tFar = t2;
				// tMin is the farthest "near" intersection
				if (t1 > tNear)
					tNear = t1;
				

				// If far is closer than near there is no collision (object is behind the camera)
				if (tFar < tNear)
					return -1.0f;
				
			}
			else // The ray is parallel to the plane, no intersections
			{
				if ((-e + min[0] > 0.0f) || (-e + max[0] < 0.0f))
					return -1.0f;
			}
		}

		// Test intersection with y axis. Same as above
		{
			Vector3D yAxis(transform[1], transform[5], transform[9]);
			float e = yAxis.dot3(delta);
			float f = this->direction.dot3(yAxis);

			if (fabsf(f) > 0.001f)
			{
				float t1 = (e + min[1]) / f;
				float t2 = (e + max[1]) / f;

				if (t1 > t2)
				{
					float w = t1; t1 = t2; t2 = w;
				}

				if (t2 < tFar)
					tFar = t2;
				if (t1 > tNear)
					tNear = t1;
				
				if (tFar < tNear)
					return -1.0f;
			}
			else
			{
				if ((-e + min[1] > 0.0f) || (-e + max[1] < 0.0f))
					return -1.0f;
			}
		}

		// Test intersection with z axis. Same as above
		{
			Vector3D zAxis(transform[2], transform[6], transform[10]);
			float e = zAxis.dot3(delta);
			float f = this->direction.dot3(zAxis);

			if (fabsf(f) > 0.001f)
			{
				float t1 = (e + min[2]) / f;
				float t2 = (e + max[2]) / f;

				if (t1 > t2)
				{
					float w = t1; t1 = t2; t2 = w;
				}

				if (t2 < tFar)
					tFar = t2;
				if (t1 > tNear)
					tNear = t1;
				
				if (tFar < tNear)
					return -1.0f;
			}
			else
			{
				if ((-e + min[2] > 0.0f) || (-e + max[2] < 0.0f))
					return -1.0f;
			}
		}


		intersectionPoint = this->start + this->direction * tNear;
		return tNear;
	}
};
