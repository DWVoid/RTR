#ifndef _TRACE_H_
#define _TRACE_H_

#include <vector>
#include "Vector.h"               /* V_LNG_VECTOR */
struct TR_matter
{
	//环境反射，RGB矢量
	Vector tr_ambient;            /* coefs of ambient reflection */
   //漫反射，RGB矢量
	Vector tr_diffuse;            /* coefs of diffuse reflection */
   //镜面反射
	float tr_specular;                         /* coef of specular reflection */
   //镜面反射系数
	float tr_exponent;                         /* specular exponent */
   //递归光线
	float tr_reflect;                          /* recursive ray */
};

struct TR_point_light
{
	Vector tr_centre;             /* point light source */
	Vector tr_intensity;
};

struct TR_ray
{
	Vector tr_start;              /* origin of the ray */
	Vector tr_codirected;         /* a co-directed vector */
};

class TR_generic_object                    /* for storage in the world */
{
public:
    TR_matter tr_material;              /* material it is made of */
    virtual void TR_init() {};
    virtual float TR_intersect(TR_ray *r) = 0;
    virtual Vector& TR_normal(Vector& normal, Vector& where) = 0;
};

class TR_sphere :public TR_generic_object
{
public:
    Vector tr_centre;             /* sphere's centre */
    float tr_radius;
    float TR_intersect(TR_ray *r);
    Vector& TR_normal(Vector& normal, Vector& where);
};

class TR_polygon:public TR_generic_object
{
public:
    Vector tr_normal;
    std::vector<Plane> tr_edges;   
    std::vector<Vector> tr_vertices;
    void TR_init();
    float TR_intersect(TR_ray *r);
    Vector& TR_normal(Vector& normal, Vector& where);
};

#define TR_MAX_POINT_LIGHTS 10
#define TR_MAX_SPHERES      10

class TR_world
{
public:
    Vector tr_ambient;            /* illumination of the world */
    std::vector<TR_point_light*> tr_point_lights;
    std::vector<TR_generic_object*> tr_objects;
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
void TR_init_world(TR_world *w);
void TR_trace_world(TR_world *w,int depth);

#endif
