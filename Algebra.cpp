#include <math.h>                           /* sqrt */
#include "Algebra.h"               /* self definition */


Vec3d::Vec3d()
{
}

Vec3d::Vec3d(float _x, float _y, float _z):
    x(_x), y(_y), z(_z)
{
}

Vec3d::Vec3d(const Vec3d & from, const Vec3d & to)
{
    x = to.x - from.x;
    y = to.y - from.y;
    z = to.z - from.z;
}

Vec3d Vec3d::operator-(const Vec3d & b) const
{
    return Vec3d(x - b.x, y - b.y, z - b.z);
}

Vec3d Vec3d::operator+(const Vec3d & b) const
{
    return Vec3d(x + b.x, y + b.y, z + b.z);
}

Vec3d Vec3d::operator*(const float b) const
{
    return Vec3d(x * b, y * b, z * b);
}

Vec3d Vec3d::operator*(const Vec3d & b) const
{
    return Vec3d(y * b.z - z * b.y, z * b.x - x * b.z,
        x * b.y - y * b.x);
}

Vec3d & Vec3d::zero()
 {
    x = y = z = 0; 
    return *this;
 }

 Vec3d & Vec3d::set(const Vec3d & to)
 {
     x = to.x; 
     y = to.y;
     z = to.z;
     return *this;
 }

 Vec3d & Vec3d::set(const float _x, const float _y, const float _z)
 {
     x = _x;
     y = _y;
     z = _z;
     return *this;
 }

 float Vec3d::length() const
 {
     return (sqrt(x * x + y * y + z * z));
 }

 Vec3d Vec3d::unit() const
 {
     const float l = length();
     return Vec3d(x / l, y / l, z / l);
 }

 float Vec3d::dot(const Vec3d & b) const
 {
    return(x * b.x + y * b.y + z * b.z);
 }

 Plane::Plane(const Vec3d & normal, const Vec3d & origine)
 {
     v[0] = normal.x;
     v[1] = normal.y;
     v[2] = normal.z;
     v[3] = -(origine.x * normal.x + origine.y * normal.y + origine.z * normal.z);
 }

 /**********************************************************\
 * Checking if a vertex belongs to the plane.             *
 *                                                        *
 * RETURNS: 0   when the vertex belongs to the plane;     *
 * -------- "+" when in the normal pointed                *
 *              halfplane;                                *
 *          "-" otherwise.                                *
 \**********************************************************/

 float Plane::vertexOnPlane(Vec3d & vertex) const
 {
     return vertex.x * v[0] + vertex.y * v[1] + vertex.z * v[2] + v[3];
 }
