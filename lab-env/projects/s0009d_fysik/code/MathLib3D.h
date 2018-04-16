#pragma once
#include <iostream>
#include <math.h>

// For projection matrix
#define CONSTANT 0.01745329251
#define PI 3.141592654


class Vector3D
{
public:
	/**
	 Initialize a vector as (0, 0, 0, 1)
	*/
	Vector3D()
	{
		this->v[0] = 0;
		this->v[1] = 0;
		this->v[2] = 0;
		this->v[3] = 1;
	}
	~Vector3D(){}
	/**
	 Initialize a vector as (x, y, z, 1)
	*/
	Vector3D(float x, float y, float z)
	{
		this->v[0] = x;
		this->v[1] = y;
		this->v[2] = z;
		this->v[3] = 1;
	}
	Vector3D(float x, float y, float z, float w)
	{
		this->v[0] = x;
		this->v[1] = y;
		this->v[2] = z;
		this->v[3] = w;
	}

	float& operator[](const int i)
	{
		return v[i];
	}

	const float& operator[](const int i) const
	{
		return v[i];
	}

	float get(int i)
	{
		return this->v[i];
	}

	float* getArray()
	{
		float* tmp = new float[4];
		tmp[0] = this->v[0];
		tmp[1] = this->v[1];
		tmp[2] = this->v[2];
		tmp[3] = this->v[3];

		return tmp;
	}

	/**
	 Add vector to vector
	*/
	Vector3D operator+(const Vector3D &v)const
	{
		Vector3D newVector;
		newVector[0] = this->v[0] + v.v[0];
		newVector[1] = this->v[1] + v.v[1];
		newVector[2] = this->v[2] + v.v[2];
		newVector[3] = this->v[3] + v.v[3];
		
		return newVector;
	}
	void operator+=(const Vector3D &v)
	{
		this->v[0] += v.v[0];
		this->v[1] += v.v[1];
		this->v[2] += v.v[2];
		this->v[3] += v.v[3];
	}
	/**
	 Add float to vector
	*/
	Vector3D operator+(const float f)const
	{
		Vector3D newVector;
		newVector[0] = this->v[0] + f;
		newVector[1] = this->v[1] + f;
		newVector[2] = this->v[2] + f;
		newVector[3] = this->v[3] + f;
		
		return newVector;
	}
	void operator+=(const float f)
	{
		this->v[0] += f;
		this->v[1] += f;
		this->v[2] += f;
		this->v[3] += f;
	}
	/**
	 Subtract vector from vector
	*/
	Vector3D operator-(const Vector3D &v)const
	{
		Vector3D newVector;
		newVector[0] = this->v[0] - v.v[0];
		newVector[1] = this->v[1] - v.v[1];
		newVector[2] = this->v[2] - v.v[2];
		newVector[3] = this->v[3] - v.v[3];
		
		return newVector;
	}
	/**
	 Subtract float from vector
	*/
	Vector3D operator-(const float f)
	{
		Vector3D newVector;
		newVector[0] = this->v[0] - f;
		newVector[1] = this->v[1] - f;
		newVector[2] = this->v[2] - f;
		newVector[3] = this->v[3] - f;

		return newVector;
	}
	friend Vector3D operator-(float f, Vector3D v)
	{
		Vector3D newVector;
		newVector[0] = v.v[0] - f;
		newVector[1] = v.v[1] - f;
		newVector[2] = v.v[2] - f;
		newVector[3] = v.v[3] - f;

		return newVector;
	}
	/**
	 Negate the vector
	*/
	Vector3D operator-()const
	{
		Vector3D newVector;
		newVector.v[0] = 0 - this->v[0];
		newVector.v[1] = 0 - this->v[1];
		newVector.v[2] = 0 - this->v[2];
		newVector.v[3] = 0 - this->v[3];

		return newVector;
	}

	/**
	Multiply vector and float
	*/
	Vector3D operator*(const float f)const
	{
		Vector3D newVector;
		newVector[0] = this->v[0] * f;
		newVector[1] = this->v[1] * f;
		newVector[2] = this->v[2] * f;
		newVector[3] = this->v[3] * f;

		return newVector;
	}
	friend Vector3D operator*(float f, Vector3D v)
	{
		Vector3D newVector;
		newVector[0] = v.v[0] * f;
		newVector[1] = v.v[1] * f;
		newVector[2] = v.v[2] * f;
		newVector[3] = v.v[3] * f;

		return newVector;
	}
	/**
	Multiply each value in vectors
	*/
	Vector3D operator*(Vector3D v)
	{
		Vector3D newVector;
		newVector[0] = this->v[0] * v.v[0];
		newVector[1] = this->v[1] * v.v[1];
		newVector[2] = this->v[2] * v.v[2];
		newVector[3] = this->v[3] * v.v[3];

		return newVector;
	}
	Vector3D quaternionMult(const Vector3D &q)const
	{
		Vector3D quat;
		quat[0] =  this->v[0] * q.v[3] + this->v[1] * q.v[2] - this->v[2] * q.v[1] + this->v[3] * q.v[0];
		quat[1] = -this->v[0] * q.v[2] + this->v[1] * q.v[3] + this->v[2] * q.v[0] + this->v[3] * q.v[1];
		quat[2] =  this->v[0] * q.v[1] - this->v[1] * q.v[0] + this->v[2] * q.v[3] + this->v[3] * q.v[2];
		quat[3] = -this->v[0] * q.v[0] - this->v[1] * q.v[1] - this->v[2] * q.v[2] + this->v[3] * q.v[3];

		return quat;
	}

	Vector3D operator/(float f)
	{
		Vector3D newVector;
		newVector.v[0] = this->v[0] / f;
		newVector.v[1] = this->v[1] / f;
		newVector.v[2] = this->v[2] / f;
		newVector.v[3] = this->v[3] / f;

		return newVector;
	}

	Vector3D operator/(const Vector3D &v)
	{
		Vector3D newVector;
		newVector[0] = this->v[0] / v.v[0];
		newVector[1] = this->v[1] / v.v[1];
		newVector[2] = this->v[2] / v.v[2];
		return newVector;
	}

	/**
	 Dot product between vectors
	*/
	float dot(const Vector3D &v)
	{
		float x = this->v[0] * v.v[0];
		float y = this->v[1] * v.v[1];
		float z = this->v[2] * v.v[2];
		float w = this->v[3] * v.v[3];
		float dotProd = x + y + z + w;
		
		return dotProd;
	}
	float dot3(const Vector3D &v)
	{
		float x = this->v[0] * v.v[0];
		float y = this->v[1] * v.v[1];
		float z = this->v[2] * v.v[2];
		float dotProd = x + y + z;
		
		return dotProd;
	}
	/**
	 Cross product between vectors
	*/
	Vector3D cross(const Vector3D &v)const
	{
		Vector3D newVector;
		newVector[0] = this->v[1] * v.v[2] - this->v[2] * v.v[1];
		newVector[1] = this->v[2] * v.v[0] - this->v[0] * v.v[2];
		newVector[2] = this->v[0] * v.v[1] - this->v[1] * v.v[0];

		return newVector;
	}

	/**
	 Set vector equal to vector
	*/
	void operator=(const Vector3D &v)
	{
		this->v[0] = v.v[0];
		this->v[1] = v.v[1];
		this->v[2] = v.v[2];
		this->v[3] = v.v[3];
	}

	/**
	 Check if vector is equal to vector
	*/
	bool operator==(const Vector3D &v)
	{
		bool isEqual = false;
		if (this->v[0] == v.v[0] &&
			this->v[1] == v.v[1] &&
			this->v[2] == v.v[2] &&
			this->v[3] == v.v[3])
		{
			isEqual = true;
		}

		return isEqual;
	}
	/**
	 Check if vector is not equal to vector
	*/
	bool operator!=(const Vector3D &v)
	{
		bool isEqual = false;
		if (this->v[0] == v.v[0] &&
			this->v[1] == v.v[1] &&
			this->v[2] == v.v[2] && 
			this->v[3] == v.v[3])
		{
			isEqual = true;
		}

		return !isEqual;
	}

	/**
	 Get the length of a vector
	*/
	float length()
	{
		float x = this->v[0];
		float y = this->v[1];
		float z = this->v[2];
		float w = this->v[3];
		float length = sqrt(x*x + y*y + z*z + w*w);

		return length;
	}
	/**
	 Get the square length of a vector (no sqrt)
	*/
	float squareLength()
	{
		float x = this->v[0];
		float y = this->v[1];
		float z = this->v[2];
		float w = this->v[3];
		float length = (x*x) + (y*y) + (z*z);// + (w*w);

		return length;
	}

	/**
	 Get a normalized vector
	*/
	Vector3D normal()
	{
		Vector3D newVector;
		float length = this->length();
		newVector[0] = this->v[0] / length;
		newVector[1] = this->v[1] / length;
		newVector[2] = this->v[2] / length;
		newVector[3] = this->v[3] / length;

		return newVector;
	}

	/**
	 Print out the vector
	*/
	void print()
	{
		std::cout << v[0] << ", " << v[1] << ", "
				  << v[2] << ", " << v[3] << std::endl;
	}

private:
	float v[4];
};




class Matrix3D
{
public:
	/**
	 Initialize matrix as identity matrix
	*/
	Matrix3D()
	{
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				this->m[row][col] = 0;
			}
		}
		this->m[0][0] = 1;
		this->m[1][1] = 1;
		this->m[2][2] = 1;
		this->m[3][3] = 1;
	}
	// Projection matrix
	Matrix3D(float FoV, float near, float far)
	{
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				this->m[row][col] = 0;
			}
		}


		float s = 1.0f / (tan((FoV / 2) * (PI/180)));

		float a = -(far / (far - near));
		float b = -((far*near) / (far - near));
		this->m[0][0] = s;
		this->m[1][1] = s;
		this->m[2][2] = a;
		this->m[3][3] = 0;
		this->m[2][3] = -1;
		this->m[3][2] = b;
	}
	// View matrix
	Matrix3D(Vector3D eye, Vector3D target, Vector3D up)
	{
		Vector3D zaxis = (eye - target).normal();	// The "forward" vector.
		Vector3D xaxis = (up.cross(zaxis)).normal();// The "right" vector.
		Vector3D yaxis = zaxis.cross(xaxis);		// The "up" vector.


		this->m[0][0] = xaxis[0];
		this->m[0][1] = xaxis[1];
		this->m[0][2] = xaxis[2];
		this->m[0][3] = -(xaxis.dot3(eye));

		this->m[1][0] = yaxis[0];
		this->m[1][1] = yaxis[1];
		this->m[1][2] = yaxis[2];
		this->m[1][3] = -(yaxis.dot3(eye));

		this->m[2][0] = zaxis[0];
		this->m[2][1] = zaxis[1];
		this->m[2][2] = zaxis[2];
		this->m[2][3] = -(zaxis.dot3(eye));

		this->m[3][0] = 0;
		this->m[3][1] = 0;
		this->m[3][2] = 0;
		this->m[3][3] = 1;
	}

	/**
	 Initialize matrix as rotation matrix around x, y, or z axis (Degrees)
	*/
	Matrix3D(const char axis, const float degrees)
	{
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				this->m[row][col] = 0;
			}
		}
		this->m[0][0] = 1;
		this->m[1][1] = 1;
		this->m[2][2] = 1;
		this->m[3][3] = 1;

		float radians = degrees * PI / 180.0f;

		float c = cos(radians);
		float s = sin(radians);

		switch (axis)
		{
		case 'x':
			this->m[1][1] = c;
			this->m[1][2] = -s;
			this->m[2][1] = s;
			this->m[2][2] = c;
			break;

		case 'y':
			this->m[0][0] = c;
			this->m[0][2] = s;
			this->m[2][0] = -s;
			this->m[2][2] = c;
			break;

		case 'z':
			this->m[0][0] = c;
			this->m[0][1] = -s;
			this->m[1][0] = s;
			this->m[1][1] = c;
			break;

		default:
			break;
		}
	}
	/**
	 Initialize matrix as rotation matrix around vector
	*/
	Matrix3D(Vector3D &v, const float degrees)
	{
		// Make sure v is unit vector-----

		float radians = degrees * PI / 180.0f;

		float c = cos(radians);
		float s = sin(radians);

		this->m[0][0] = c + v[0] * v[0] * (1 - c);
		this->m[0][1] = v[0] * v[1] * (1 - c) - v[2] * s;
		this->m[0][2] = v[0] * v[2] * (1 - c) + v[1] * s;
		this->m[0][3] = 0;

		this->m[1][0] = v[1] * v[0] * (1 - c) + v[2] * s;
		this->m[1][1] = c + v[1] * v[1] * (1 - c);
		this->m[1][2] = v[1] * v[2] * (1 - c) - v[0] * s;
		this->m[1][3] = 0;

		this->m[2][0] = v[2] * v[0] * (1 - c) - v[1] * s;
		this->m[2][1] = v[2] * v[1] * (1 - c) + v[0] * s;
		this->m[2][2] = c + v[2] * v[2] * (1 - c);
		this->m[2][3] = 0;

		this->m[3][0] = 0;
		this->m[3][1] = 0;
		this->m[3][2] = 0;
		this->m[3][3] = 1;
	}

	/**
	 Returns a rotation matrix from given quaternion
	*/
	Matrix3D(Vector3D q)
	{
		this->m[0][3] = 0;
		this->m[1][3] = 0;
		this->m[2][3] = 0;
		this->m[3][3] = 1;
		this->m[3][2] = 0;
		this->m[3][1] = 0;
		this->m[3][0] = 0;

		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		this->m[0][0] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];
		this->m[0][1] = 	2 * q[0] * q[1] - 2 * q[2] * q[3];
		this->m[0][2] = 	2 * q[0] * q[2] + 2 * q[1] * q[3];

		this->m[1][0] = 	2 * q[0] * q[1] + 2 * q[2] * q[3];
		this->m[1][1] = 1 - 2 * q[0] * q[0] - 2 * q[2] * q[2];
		this->m[1][2] = 	2 * q[1] * q[2] - 2 * q[0] * q[3];

		this->m[2][0] = 	2 * q[0] * q[2] - 2 * q[1] * q[3];
		this->m[2][1] = 	2 * q[1] * q[2] + 2 * q[0] * q[3];
		this->m[2][2] = 1 - 2 * q[0] * q[0] - 2 * q[1] * q[1];
	}

	static Matrix3D skewMatrix(const Vector3D &v)
	{
		Matrix3D newMatrix;

		newMatrix[0] = 0;
		newMatrix[1] = -v[2];
		newMatrix[2] = v[1];

		newMatrix[4] = v[2];
		newMatrix[5] = 0;
		newMatrix[6] = -v[0];

		newMatrix[8] = -v[1];
		newMatrix[9] = v[0];
		newMatrix[10] = 0;

		return newMatrix;
	}

	/**
	 Get the value at position [row][col]
	*/
	float get(const int row, const int col)
	{
		return m[row][col];
	}
	float& operator[](const int i)
	{
		int row = i / 4;
		int col = i % 4;

		return this->m[row][col];
	}
	const float& operator[](const int i)const
	{
		int row = i / 4;
		int col = i % 4;

		return this->m[row][col];
	}

	Vector3D col(const int i)const
	{
		Vector3D c(m[0][i], m[1][i], m[2][i], m[3][i]);

		return c;
	}
	void setCol(const int i, Vector3D c)
	{
		m[0][i] = c[0];
		m[1][i] = c[1];
		m[2][i] = c[2];
		m[3][i] = c[3];
	}

	Vector3D row(const int i)const
	{
		Vector3D r(m[i][0], m[i][1], m[i][2], m[i][3]);

		return r;
	}
	void setRow(const int i, Vector3D r)
	{
		m[i][0] = r[0];
		m[i][1] = r[1];
		m[i][2] = r[2];
		m[i][3] = r[3];
	}


	/**
	 Set the [value] at position [row][col]
	*/
	void set(const int row, const int col, const float value)
	{
		m[row][col] = value;
	}

	float* getArray()
	{
		float* tmp = new float[16];
		for (int i = 0; i < 16; i++)
		{
			int row = i / 4;
			int col = i % 4;
			tmp[i] = this->m[row][col];
			
		}
		return tmp;
	}
	void translateTo(float x, float y, float z)
	{
		this->m[0][3] = x;
		this->m[1][3] = y;
		this->m[2][3] = z;
	}
	void translateTo(Vector3D v)
	{
		this->m[0][3] = v[0];
		this->m[1][3] = v[1];
		this->m[2][3] = v[2];
	}
	Vector3D getPosition()
	{
		Vector3D newVector(m[0][3], m[1][3], m[2][3]);
		return newVector;
	}
	void setScale(float x, float y, float z)
	{
		this->m[0][0] = x;
		this->m[1][1] = y;
		this->m[2][2] = z;
	}
	void scale(float scale)
	{
		this->m[0][0] *= scale;
		this->m[1][1] *= scale;
		this->m[2][2] *= scale;
	}
	/**
	 Multiply matrix with a float
	*/
	Matrix3D operator*(const float f)
	{
		Matrix3D newMatrix;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				newMatrix.set(row, col, m[row][col] * f);
			}
		}

		return newMatrix;
	}
	/**
	 Multiply matrix with a vector
	*/
	Vector3D operator*(Vector3D v)
	{
		Vector3D newVector;

		newVector[0] =	this->m[0][0] * v[0] +
						this->m[0][1] * v[1] +
						this->m[0][2] * v[2] +
						this->m[0][3] * v[3];

		newVector[1] =	this->m[1][0] * v[0] +
						this->m[1][1] * v[1] +
						this->m[1][2] * v[2] +
						this->m[1][3] * v[3];

		newVector[2] =	this->m[2][0] * v[0] +
						this->m[2][1] * v[1] +
						this->m[2][2] * v[2] +
						this->m[2][3] * v[3];

		newVector[3] =	this->m[3][0] * v[0] +
						this->m[3][1] * v[1] +
						this->m[3][2] * v[2] +
						this->m[3][3] * v[3];

		return newVector;
	}	
	/**
	 Multiply matrix with a matrix
	*/
	Matrix3D operator*(Matrix3D m)
	{
		Matrix3D newMatrix;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				float sum = 0;
				for (int inner = 0; inner < 4; inner++)
				{
					sum += this->m[row][inner] * m.get(inner, col);
				}
				newMatrix.set(row, col, sum);
			}
		}

		return newMatrix;
	}

	Matrix3D operator+(Matrix3D &m)
	{
		Matrix3D newMatrix;
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				newMatrix.set(row, col, this->m[row][col] + m.get(row, col));
			}
		}
		return newMatrix;
	}

	/**
	 Set matrix equal to matrix
	*/
	void operator=(Matrix3D m)
	{
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				this->m[row][col] = m.get(row, col);
			}
		}
	}

	/**
		Compare two matrices
	*/
	bool operator==(Matrix3D m)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				if (this->m[i][j] != m.m[i][j])
				{
					return false;
				}
			}
		}

		return true;
	}


	/**
	 Transpose the matrix, swap rows and cols
	*/
	Matrix3D trans()const
	{
		Matrix3D newMatrix;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				newMatrix.set(row, col, this->m[col][row]);
			}
		}

		return newMatrix;
	}

	Matrix3D GSOrtho()
	{
		Matrix3D newMatrix;

		Vector3D col0 = this->col(0).normal();
		newMatrix.setCol(0, col0);

		Vector3D col1 = (this->col(1) - this->col(1).dot3(col0) * col0).normal();
		newMatrix.setCol(1, col1);

		Vector3D col2 = (this->col(2) - this->col(2).dot3(col0) * col0);
		col2 = col2 - (col2.dot3(col1) * col1);
		col2 = col2.normal();
		newMatrix.setCol(2, col2);


		return newMatrix;
	}

	/**
	 Calculate the inverted matrix
	*/
	Matrix3D invert()const
	{
		double Result[4][4];
		double tmp[12]; /* temp array for pairs */
		Matrix3D src; /* array of transpose source matrix */
		double det; /* determinant */
					/* transpose matrix */
		
		src = this->trans();

		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		/* calculate first 8 elements (cofactors) */
		Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];

		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];
		/* calculate second 8 elements (cofactors) */
		Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
		/* calculate determinant */
		det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];


		Matrix3D FloatResult;

		try
		{
			/* calculate matrix inverse */
			det = 1.0f / det;
		}

		catch(std::exception e)
		{}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				FloatResult.set(i, j, float(Result[i][j] * det));
			}
		}
		
		
		return FloatResult;
	}

	void print()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				std::cout << m[i][j] << "\t";
			}
			std::cout << "\n";
		}
	}


private:
	float m[4][4];

};