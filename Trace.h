#ifndef _TRACE_H_
#define _TRACE_H_

#include <vector>
#include "Algebra.h"     
struct Material
{
	//环境反射，RGB矢量
	Vec3d tr_ambient;            /* coefs of ambient reflection */
   //漫反射，RGB矢量
	Vec3d tr_diffuse;            /* coefs of diffuse reflection */
   //镜面反射
	float tr_specular;                         /* coef of specular reflection */
   //镜面反射系数
	float tr_exponent;                         /* specular exponent */
   //递归光线
	float tr_reflect;                          /* recursive ray */
};

struct PointLight
{
	Vec3d tr_centre;             /* point light source */
	Vec3d tr_intensity;
};

struct Ray
{
	Vec3d tr_start;              /* origin of the ray */
	Vec3d tr_codirected;         /* a co-directed vector */
};

class BaseObject                    /* for storage in the world */
{
public:
    Material tr_material;              /* material it is made of */
    virtual void TR_init() {};
    virtual float TR_intersect(Ray *r) = 0;
    virtual Vec3d& TR_normal(Vec3d& normal, Vec3d& where) = 0;
};

class Sphere :public BaseObject
{
public:
    Vec3d tr_centre;             /* sphere's centre */
    float tr_radius;
    float TR_intersect(Ray *r);
    Vec3d& TR_normal(Vec3d& normal, Vec3d& where);
};

class TPolygon:public BaseObject
{
public:
    Vec3d tr_normal;
    std::vector<Plane> tr_edges;   
    std::vector<Vec3d> tr_vertices;
    void TR_init();
    float TR_intersect(Ray *r);
    Vec3d& TR_normal(Vec3d& normal, Vec3d& where);
};

#define TR_MAX_POINT_LIGHTS 10
#define TR_MAX_SPHERES      10

class Camera
{
public:

};

class PerspectiveCamera : public Camera
{

};
class Scene
{
public:
    Vec3d tr_ambient;            /* illumination of the world */
    std::vector<PointLight*> tr_point_lights;
    std::vector<BaseObject*> tr_objects;
};

#define TR_AMBIENT  0x1                     /* only ambient illumination */
#define TR_DIFFUSE  0x2                     /* only diffuse illumination */
#define TR_SPECULAR 0x4                     /* plus all of the above */
#define TR_SHADOW   0x8                     /* shoot shadow rays */
#define TR_REFLECT  0x10                    /* shoot reflection rays */

void TR_init_rendering(int type);
void TR_set_camera(float viewer_x,float viewer_y,float viewer_z,
                   float screen_x,float screen_y,float screen_z,
                   float screen_ux,float screen_uy,float screen_uz,
                   float screen_vx,float screen_vy,float screen_vz
                  );
void TR_init_world(Scene *w);
void TR_trace_world(Scene *w,int depth);

#endif
