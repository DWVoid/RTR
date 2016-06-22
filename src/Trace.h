#ifndef _TRACE_H_
#define _TRACE_H_

#include <vector>
#include <memory>
#include "Algebra.h"

struct Material
{
    Vec3d ambient;            // 环境反射 - coefs of ambient reflection
    Vec3d diffuse;            // 漫反射 - coefs of diffuse reflection
    double specular;           // 镜面反射 - coef of specular reflection
    double exponent;           // 镜面反射系数 - specular exponent
    double reflect;            // 递归光线 - recursive ray
};

struct PointLight
{
    Vec3d centre;             /* point light source */
    Vec3d intensity;
};

class Ray
{
public:
    Vec3d start;              // origin of the ray 
    Vec3d codirected;         // a co-directed vector 
    Ray() = default;
    Ray(const Vec3d& from, const Vec3d& vector);
    Vec3d onRay(const double f) const;
};

class BaseObject                 
{
public:
    Material material;              // material it is made of 
    virtual void init() {};
    virtual double intersect(const Ray& r) const = 0;
    virtual Vec3d normal(const Vec3d& where) const = 0;
    virtual ~BaseObject() = default;
};

class Sphere :public BaseObject
{
public:
    Vec3d centre;
    double radius;
    double intersect(const Ray& r) const;
    Vec3d normal(const Vec3d& where) const;
};

class TPolygon:public BaseObject
{
public:
    Vec3d inormal;
    std::vector<Plane> edges;   
    std::vector<Vec3d> vertices;
    void init();
    double intersect(const Ray& r) const;
    Vec3d normal(const Vec3d& where) const;
};

class Camera
{
public:
    Vec3d viewer;              /* position of the viewer */
    Vec3d screen;              /* origine of the screen */
    Vec3d screenU;            /* screen orientation vectors */
    Vec3d screenV;
    Camera(
        const Vec3d& _viewer, const Vec3d& _screen,
        const Vec3d& _screenU, const Vec3d& _screenV);
    void set(
        const Vec3d& _viewer, const Vec3d& _screen,
        const Vec3d& _screenU, const Vec3d& _screenV);
    Ray genRay(const double xpos, const double ypos) const;
};

class Scene
{
public:
    Vec3d ambient;            /* illumination of the world */
    std::vector<PointLight*> point_lights;
    std::vector<BaseObject*> objects;
    void illuminate(
        Vec3d& light, PointLight *l, const Material& material,
        const Vec3d& normal, const Vec3d& where, const Vec3d& viewer);
    bool shadowRay(PointLight *l, const Vec3d& point, BaseObject* cur_obj) const;
    void directRay(Vec3d& light, const Ray& r, const Vec3d& viewer, BaseObject* cur_obj, const size_t depth);
};

class Renderer
{
public:
    void init();
    std::shared_ptr<Scene> scene;
    std::vector<std::shared_ptr<Camera> > cameras;
    void capture(const int xSize, const int ySize, const size_t camID, const size_t depth = 10);
};

#endif
