#include "Graphics.h"           /* G_pixel */
#include "Colour.h"               /* colour related */
#include "Trace.h"                 /* self definition */
#include <math.h>                           /* sqrt */

int TR_rendering_type;                      /* rendering options */
Vec3d TR_viewer;              /* position of the viewer */
Vec3d TR_screen;              /* origine of the screen */
Vec3d TR_screen_u;            /* screen orientation vectors */
Vec3d TR_screen_v;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Constructing a ray from two points.                   *
 *                                                       *
 * RETURNS: Constructed ray.                             *
 * --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    Ray *TRI_make_ray_point(Ray *r, Vec3d& from, Vec3d& to)
    {
        //设置光线矢量的起始点
        r->tr_start = from;
        //计算光线的矢量方向
        r->tr_codirected = to - from;
        return r;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Constructing a ray from a point and a vector.         *
     *                                                       *
     * RETURNS: Constructed ray.                             *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Ray *TRI_make_ray_vector(Ray *r, Vec3d& from, Vec3d& vector)
    {
        r->tr_start = from;
        r->tr_codirected = vector;
        return r;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Returns point at distance t from the origine.         *
     *                                                       *
     * RETURNS: Constructed vertex.                          *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Vec3d& TRI_on_ray(Vec3d& point, Ray *r, float t)
    {
        return point = r->tr_start + r->tr_codirected * t;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Computing illumination of a intersected surface point.*
     *                                                       *
     * RETURNS: An RGB triple.                               *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Vec3d& TRI_illuminate(Vec3d& light, PointLight *l, Material *material,
        Vec3d& normal, Vec3d& where, Vec3d& viewer)
    {
        Vec3d lightvector;
        float diffuseratio, specularratio;

        lightvector = (l->tr_centre - where).unit();

		if ((diffuseratio = normal.dot(lightvector)) > 0)
		{
			light += l->tr_intensity.blend(material->tr_diffuse) * diffuseratio;//diffuse term 

			if ((specularratio = (normal * (diffuseratio * 2) - lightvector).dot((viewer - where).unit())) > 0)
				light += l->tr_intensity * material->tr_specular * pow(specularratio, (int)material->tr_exponent);
			//specular term
		}
        return(light);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Casting a ray towards a lightsource to find out if    *
     * it is hidden by other objects or not.                 *
     *                                                       *
     * RETURNS: 1 light source visible; 0 otherwise.         *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    bool TRI_shadow_ray(Scene *w, PointLight *l, Vec3d& point, BaseObject* cur_obj)
    {
        float t = 0.0;
        Ray r;

        TRI_make_ray_point(&r, point, l->tr_centre);
        for (BaseObject* i : w->tr_objects)            /* finding intersection */
            if (i != cur_obj)
            {
                t = i->TR_intersect(&r);
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

    Vec3d& TRI_direct_ray(Vec3d& light, Scene *w, Ray *r, BaseObject* cur_obj, int depth)
    {
        BaseObject* obj = nullptr;
        float minInterDist = 1E5f, t = 0.0f;
        Ray rr;
        Vec3d where;                 /* current intersection */
        Vec3d normal;                /* of the current intersection */
        Vec3d viewer, reflect, rlight;
        size_t lsize = w->tr_point_lights.size();

        if (depth != 0)
        {
            for (BaseObject* i : w->tr_objects)           /* finding intersection */
                if (i != cur_obj)                           /* with itself, no sense */
                {
                    t = i->TR_intersect(r);
                    if ((t > 0) && (t < minInterDist))/* not behind the ray */ 
                    {
                        minInterDist = t;/* finding closest intersection */
                        obj = i;
                    }                /* a valid intersection */
                }
			if (obj)                           /* if some objects intersected */
			{
				light += obj->tr_material.tr_ambient.blend(w->tr_ambient);

				TRI_on_ray(where, r, minInterDist);           /* intersection's coordinate */

				obj->TR_normal(normal, where);

				for (size_t i = 0; i < lsize; ++i)     /* illumination from each light */
					if (!TRI_shadow_ray(w, w->tr_point_lights[i], where, obj))
						TRI_illuminate(light, w->tr_point_lights[i], &obj->tr_material,
							normal, where, TR_viewer);

				viewer = (TR_viewer - where).unit();
				reflect = normal * normal.dot(viewer) * 2 - viewer;
				TRI_make_ray_vector(&rr, where, reflect); /* prepare recursive ray */

				TRI_direct_ray(rlight.zero(), w, &rr, obj, depth - 1);
				light += rlight * obj->tr_material.tr_reflect;
			}
        }
        return(light);
    }

    /**********************************************************\
     * Setting the camera parameter, where TR_viewer stores   *
     * position of the viewer's eye, TR_screen origine of the *
     * projection plane, TR_screen_u and TR_screen_v          *
     * orientation of the projection plane in the world       *
     * space.                                                 *
    \**********************************************************/
 
    void TR_set_camera(float viewer_x, float viewer_y, float viewer_z,
        float screen_x, float screen_y, float screen_z,
        float screen_ux, float screen_uy, float screen_uz,
        float screen_vx, float screen_vy, float screen_vz
        )
    {
        TR_viewer.set(viewer_x, viewer_y, viewer_z);
        TR_screen.set(screen_x, screen_y, screen_z);
        TR_screen_u.set(screen_ux, screen_uy, screen_uz);
        TR_screen_v.set(screen_vx, screen_vy, screen_vz);
    }

    /**********************************************************\
     * Finding intersection of a ray with a sphere.           *
     *                                                        *
     * RETURNS: Distance from the origine of the ray.         *
     * --------                                               *
    \**********************************************************/

    float Sphere::TR_intersect(Ray *r)
    {
        float a, b, c, det;
        Vec3d d;

        a = r->tr_codirected.dot(r->tr_codirected);
        b = 2 * r->tr_codirected.dot(d = (r->tr_start - tr_centre));
        c = d.dot(d) - tr_radius * tr_radius;

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

    Vec3d& Sphere::TR_normal(Vec3d& normal, Vec3d& where)
    {
        return normal=(where - tr_centre).unit();
    }

    /**********************************************************\
     * Computes plane equation and the equations delimiting   *
     * the edges.                                             *
    \**********************************************************/

    //计算平面方程和边的投影公式
    //多边形的初始化
    void TPolygon::TR_init()
    {
        tr_normal = (Vec3d(tr_vertices[2], tr_vertices[1]) * Vec3d(tr_vertices[1], tr_vertices[0])).unit();        
		/* normal to the plane */

        for (size_t i = 0; i < tr_vertices.size() - 1; ++i)           /* finding equations for edges */
            tr_edges.push_back(Plane(tr_normal * Vec3d(tr_vertices[i], tr_vertices[i + 1]), tr_vertices[i]));
    }

    /**********************************************************\
     * Finding intersection of a ray with a polygon.           *
     *                                                        *
     * RETURNS: Distance from the origine of the ray.         *
     * --------                                               *
    \**********************************************************/

    float TPolygon::TR_intersect(Ray *r)
    {
        Vec3d a(tr_vertices[0] - r->tr_start);
        float t, s2 = r->tr_codirected.dot(tr_normal);
        size_t size = tr_vertices.size() - 1;

        if (s2 == 0) 
            return -1.0f; 
        else
        {
            t = a.dot(tr_normal) / s2;
            if (t < 0) 
                return -1.0f;
            else
            {
                TRI_on_ray(a, r, t);
                for (size_t i = 0; i < size; i++)
                    if (tr_edges[i].vertexOnPlane(a) > 0.0f)\
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

    Vec3d& TPolygon::TR_normal(Vec3d& normal, Vec3d& where)
    {
        normal.set(tr_normal);
        return normal;
    }

    /**********************************************************\
     * Initialisez all entities in the world.                 *
    \**********************************************************/

    void TR_init_world(Scene *w)
    {
        for (BaseObject* o : w->tr_objects)
            o->TR_init();
    }

    /**********************************************************\
     * Ray tracing a scene for all pixels.                    *
    \**********************************************************/

    void TR_trace_world(Scene *w, int depth)
    {
        float x, y;
        Vec3d l;//light
        Vec3d point;
        int coord[2];
        Ray r;

        for (coord[0] = 0; coord[0] < HW_SCREEN_X_SIZE; ++coord[0])
            for (coord[1] = 0; coord[1] < HW_SCREEN_Y_SIZE; ++coord[1])           /* for each pixel on screen */
            {
                x = coord[0] - HW_SCREEN_X_SIZE / 2;
                y = coord[1] - HW_SCREEN_Y_SIZE / 2;                  /* plane coordinates */

                point = (TR_screen_u*x) + (TR_screen_v * y) + TR_screen;

                //从两个点(point and TR_viewer)构建ray
                TRI_make_ray_point(&r, TR_viewer, point);

                //关键中的关键，计算环境光，返回pixel的照明度
                TRI_direct_ray(l.zero(), w, &r, nullptr, depth);

                //Setting a pixel
                G_pixel(coord,
                    CL_colour(l.x * CL_COLOUR_LEVELS, l.y * CL_COLOUR_LEVELS, l.z * CL_COLOUR_LEVELS));
            }
    }
