#ifndef _VECTOR_H_
#define _VECTOR_H_

class Vec3d
{
public:
    double x, y, z;
    Vec3d();
    Vec3d(double _x, double _y, double _z);
    Vec3d(const Vec3d& from, const Vec3d& to);
    Vec3d operator - (const Vec3d& b) const;
    Vec3d operator + (const Vec3d& b) const;
    Vec3d operator * (const double b) const;
    Vec3d operator * (const Vec3d& b) const; //Cross Product

    Vec3d& operator -= (const Vec3d& b);
    Vec3d& operator += (const Vec3d& b);
    Vec3d& operator *= (const double b);
    Vec3d& operator *= (const Vec3d& b); //Cross Product

    Vec3d blend(const Vec3d& b) const;
    Vec3d& zero();
    Vec3d& set(const Vec3d& to);
    Vec3d& set(const double _x, const double _y, const double _z);
    double length() const;
    Vec3d unit() const;
    double dot(const Vec3d& b) const;
};

class Plane
{
private:
    double v[4];
public:
    double& operator[](const int _v)
    {
        return v[_v];
    }
    Plane(const Vec3d& normal, const Vec3d& origine);
    double vertexOnPlane(Vec3d& vertex) const;
};

#endif
