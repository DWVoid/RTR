#include <math.h>                           /* sqrt */
#include "vector.h"               /* self definition */

/**********************************************************\
 * Computes a scalar product of two vectors.              *
 *                                                        *
 * RETURNS: Scalar product.                               *
 * --------                                               *
\**********************************************************/

float V_scalar_product(Vector& a, Vector& b)
{
	return(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

/**********************************************************\
 * Computes a vector product of two vectors.              *
 *                                                        *
 * RETURNS: Vector product.                               *
 * --------                                               *
\**********************************************************/

Vector& V_vector_product(Vector& product, Vector& a, Vector& b)
{
	product[0] = a[1] * b[2] - a[2] * b[1];
	product[1] = a[2] * b[0] - a[0] * b[2];
	product[2] = a[0] * b[1] - a[1] * b[0];

	return(product);
}

/**********************************************************\
 * Computes a unit length vector base on the coordinates  *
 * of the beginning and the end.                          *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_unit_vector(Vector& vector, Vector& from, Vector& to)
{
	float lng;
	vector[0] = to[0] - from[0]; vector[1] = to[1] - from[1]; vector[2] = to[2] - from[2];
	lng = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
	vector[0] /= lng; vector[1] /= lng; vector[2] /= lng;
	//返回的是单位化过的方向矢量
	return(vector);
}

Vector::Vector()
{
}

Vector::Vector(float x, float y, float z)
{
    v[0] = x; v[1] = y; v[2] = z;
}

Vector::Vector(const Vector & from, const Vector & to)
{
    v[0] = to.v[0] - from.v[0];
    v[1] = to.v[1] - from.v[1];
    v[2] = to.v[2] - from.v[2];
}

Vector Vector::operator-(const Vector & b) const
{
    return Vector(v[0] - b.v[0], v[1] - b.v[1], v[2] - b.v[2]);
}

Vector Vector::operator+(const Vector & b) const
{
    return Vector(v[0] + b.v[0], v[1] + b.v[1], v[2] + b.v[2]);
}

Vector Vector::operator*(const float b) const
{
    return Vector(v[0] * b, v[1] * b, v[2] * b);
}

Vector & Vector::zero()
 {
    v[0] = v[1] = v[2] = 0; 
    return *this;
 }

 Vector & Vector::set(const Vector & to)
 {
     v[0] = to.v[0]; 
     v[1] = to.v[1];
     v[2] = to.v[2];
     return *this;
 }

 Vector & Vector::set(float x, float y, float z)
 {
     v[0] = x;
     v[1] = y;
     v[2] = z;
     return *this;
 }

 Plane::Plane(const Vector & normal, const Vector & origine)
 {
     v[0] = normal.v[0];
     v[1] = normal.v[1];
     v[2] = normal.v[2];
     v[3] = -(origine.v[0] * normal.v[0] + origine.v[1] * normal.v[1] + origine.v[2] * normal.v[2]);
 }

 /**********************************************************\
 * Checking if a vertex belongs to the plane.             *
 *                                                        *
 * RETURNS: 0   when the vertex belongs to the plane;     *
 * -------- "+" when in the normal pointed                *
 *              halfplane;                                *
 *          "-" otherwise.                                *
 \**********************************************************/

 float Plane::vertexOnPlane(Vector & vertex) const
 {
     return vertex[0] * v[0] + vertex[1] * v[1] +
         vertex[2] * v[2] + v[3];
 }
