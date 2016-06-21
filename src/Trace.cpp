#include "Graphics.h"           /* G_pixel */
#include "Colour.h"             /* colour related */
#include "Trace.h"              /* self definition */
#include <math.h>               /* sqrt */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* Constructing a ray from a point and a vector.         *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Ray::Ray(const Vec3d& from, const Vec3d& vector) :
    start(from), codirected(vector)
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* Returns point at distance t from the origine.         *
*                                                       *
* RETURNS: Constructed vertex.                          *
* --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Vec3d Ray::onRay(const double f) const
{
    return Vec3d(start + codirected * f);
}

/**********************************************************\
* Finding intersection of a ray with a sphere.           *
*                                                        *
* RETURNS: Distance from the origine of the ray.         *
* --------                                               *
\**********************************************************/

double Sphere::intersect(const Ray& r) const
{
    double a, b, c, det;
    Vec3d d;

    a = r.codirected.dot(r.codirected);
    b = 2 * r.codirected.dot(d = (r.start - centre));
    c = d.dot(d) - radius * radius;

    //根的判断
    det = b * b - 4 * a * c;

    if (det < 0) return(-1);                      /* no intersection */
    if (det == 0) return(-b / (2 * a));               /* one intersection */

    return(((a > 0) ? -sqrt(det) : sqrt(det)) - b) / (2 * a);/* closest intersection */
}

/**********************************************************\
* Computes sphere's normal for a point on a shpere.      *
*                                                        *
* RETURNS: The normal vector.                            *
* --------                                               *
\**********************************************************/

Vec3d Sphere::normal(const Vec3d& where) const
{
    return ((where - centre).unit());
}

/**********************************************************\
* Computes plane equation and the equations delimiting   *
* the edges.                                             *
\**********************************************************/

void TPolygon::init()
{
    inormal = (Vec3d(vertices[2], vertices[1]) * Vec3d(vertices[1], vertices[0])).unit();
    /* normal to the plane */

    for (size_t i = 0; i < vertices.size() - 1; ++i)           /* finding equations for edges */
        edges.push_back(Plane(inormal * Vec3d(vertices[i], vertices[i + 1]), vertices[i]));
}

/**********************************************************\
* Finding intersection of a ray with a polygon.           *
*                                                        *
* RETURNS: Distance from the origine of the ray.         *
* --------                                               *
\**********************************************************/

double TPolygon::intersect(const Ray& r) const
{
    Vec3d a(vertices[0] - r.start);
    double t, s2 = r.codirected.dot(inormal);
    int size = vertices.size() - 1;

    if (s2 == 0)
        return -1.0f;
    else
    {
        t = a.dot(inormal) / s2;
        if (t < 0)
            return -1.0f;
        else
        {
            a = r.onRay(t);
            for (int i = 0; i < size; i++)
                if (edges[i].vertexOnPlane(a) > 0.0f)
                    return -1.0f;
            return t;
        }
    }
}

/**********************************************************\
* Returns polygon's normal.                              *
*                                                        *
* RETURNS: The Normal vector.                            *
* --------                                               *
\**********************************************************/

Vec3d TPolygon::normal(const Vec3d& where) const
{
    return inormal;
}

Camera::Camera(const Vec3d & _viewer, const Vec3d & _screen, const Vec3d & _screenU, const Vec3d & _screenV):
    viewer(_viewer), screen(_screen), screenU(_screenU), screenV(_screenV)
{
}

///Setting the camera parameter, where viewer stores   
///position of the viewer's eye, screen origine of the 
///projection plane, screen_u and screen_v orientation 
///of the projection plane in the world space.  

void Camera::set(const Vec3d & _viewer, const Vec3d & _screen, const Vec3d & _screenU, const Vec3d & _screenV)
{
    viewer.set(_viewer);
    screen.set(_screen);
    screenU.set(_screenU);
    screenV.set(_screenV);
}

Ray Camera::genRay(const double xpos, const double ypos) const
{
    return Ray(viewer, (screenU * xpos) + (screenV * ypos) + screen - viewer);
}

void Renderer::init()
{
    for (BaseObject* o : scene->objects)
        o->init();
}

void Renderer::capture(const int xSize, const int ySize, const int camID, const int depth)
{
    double x, y;
    Vec3d l;//light
    int coord[2];
    Ray r;

    for (coord[0] = 0; coord[0] < xSize; ++coord[0])
        for (coord[1] = 0; coord[1] < ySize; ++coord[1])           /* for each pixel on screen */
        {
            x = coord[0] - xSize / 2;
            y = coord[1] - ySize / 2;                  /* plane coordinates */

            //关键中的关键，计算环境光，返回pixel的照明度
            scene->directRay(l.zero(), cameras[camID]->genRay(x, y), cameras[camID]->viewer, nullptr, depth);

            //Setting a pixel
            G_pixel(coord,
                CL_colour(l.x * CL_COLOUR_LEVELS, l.y * CL_COLOUR_LEVELS, l.z * CL_COLOUR_LEVELS));
        }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* Computing illumination of a intersected surface point.*
*                                                       *
* RETURNS: An RGB triple.                               *
* --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Scene::illuminate(Vec3d & light, PointLight * l, const Material & material, const Vec3d & normal, const Vec3d & where, const Vec3d & viewer)
{
    double diffuseratio, specularratio;
    Vec3d lightvector((l->centre - where).unit());
    if ((diffuseratio = normal.dot(lightvector)) > 0)
    {
        light += l->intensity.blend(material.diffuse) * diffuseratio;//diffuse term 
        if ((specularratio = (normal * (diffuseratio * 2) - lightvector).dot((viewer - where).unit())) > 0)
            light += l->intensity * material.specular * pow(specularratio, (int)material.exponent);  //specular term
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* Casting a ray towards a lightsource to find out if    *
* it is hidden by other objects or not.                 *
*                                                       *
* RETURNS: 1 light source visible; 0 otherwise.         *
* --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool Scene::shadowRay(PointLight * l, const Vec3d & point, BaseObject * cur_obj) const
{
    double t = 0.0;
    Ray r(point, l->centre - point);
    for (BaseObject* i : objects)            /* finding intersection */
        if (i != cur_obj)
        {
            t = i->intersect(r);
            if ((t > 0) && (t <= 1)) return true;             /* first intersection is enough */
        }
    return false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
* Casting a ray into the world, recursing to compute    *
* environmental reflections.                            *
*                                                       *
* RETURNS: Illumination for the pixel.                  *
* --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Scene::directRay(Vec3d & light, const Ray & r, const Vec3d& viewer, BaseObject * cur_obj, const int depth)
{
    BaseObject* obj = nullptr;
    double minInterDist = 1E5f, t = 0.0f;
    Vec3d where;                 /* current intersection */
    Vec3d normal;                /* of the current intersection */
    Vec3d _viewer, reflect, rlight;
    int lsize = point_lights.size();

    if (depth != 0)
    {
        for (BaseObject* i : objects)           /* finding intersection */
            if (i != cur_obj)                           /* with itself, no sense */
            {
                t = i->intersect(r);
                if ((t > 0) && (t < minInterDist))/* not behind the ray */
                {
                    minInterDist = t;/* finding closest intersection */
                    obj = i;
                }                /* a valid intersection */
            }
        if (obj)                           //got intersection
        {
            light += obj->material.ambient.blend(ambient);

            where = r.onRay(minInterDist);           /* intersection's coordinate */

            normal = obj->normal(where);

            for (int i = 0; i < lsize; ++i)     /* illumination from each light */
                if (!shadowRay(point_lights[i], where, obj))
                    illuminate(light, point_lights[i], obj->material, normal, where, viewer);

            _viewer = (viewer - where).unit();
            reflect = normal * normal.dot(_viewer) * 2 - _viewer;
            Ray rr(where, reflect); /* prepare recursive ray */

            directRay(rlight.zero(), rr, viewer, obj, depth - 1);
            light += rlight * obj->material.reflect;
        }
    }
}
