#ifndef _VECTOR_H_
#define _VECTOR_H_

class Vec3d
{
public:
    float x, y, z;
    Vec3d();
    Vec3d(float _x, float _y, float _z);
    Vec3d(const Vec3d& from, const Vec3d& to);
    Vec3d operator - (const Vec3d& b) const;
    Vec3d operator + (const Vec3d& b) const;
    Vec3d operator * (const float b) const;
    Vec3d operator * (const Vec3d& b) const; //Cross Product

	Vec3d& operator -= (const Vec3d& b);
	Vec3d& operator += (const Vec3d& b);
	Vec3d& operator *= (const float b);
	Vec3d& operator *= (const Vec3d& b); //Cross Product

	Vec3d blend(const Vec3d& b) const;
    Vec3d& zero();
    Vec3d& set(const Vec3d& to);
    Vec3d& set(const float _x, const float _y, const float _z);
    float length() const;
    Vec3d unit() const;
    float dot(const Vec3d& b) const;
};

class Plane
{
private:
    float v[4];
public:
    float& operator[](const int _v)
    {
        return v[_v];
    }
    Plane(const Vec3d& normal, const Vec3d& origine);
    float vertexOnPlane(Vec3d& vertex) const;
};

#endif
