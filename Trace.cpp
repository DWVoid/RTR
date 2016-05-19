#include "Graphics.h"           /* G_pixel */
#include "Colour.h"               /* colour related */
#include "Vector.h"               /* linear algebra related */
#include "Trace.h"                 /* self definition */
#include <math.h>                           /* sqrt */
#include <stdlib.h>                         /* malloc */

int TR_rendering_type;                      /* rendering options */
Vector TR_viewer;              /* position of the viewer */
Vector TR_screen;              /* origine of the screen */
Vector TR_screen_u;            /* screen orientation vectors */
Vector TR_screen_v;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Constructing a ray from two points.                   *
 *                                                       *
 * RETURNS: Constructed ray.                             *
 * --------                                              *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    TR_ray *TRI_make_ray_point(TR_ray *r, Vector& from, Vector& to)
    {
        //设置光线矢量的起始点
        V_set(r->tr_start, from);
        //计算光线的矢量方向
        V_difference(r->tr_codirected, to, from);
        return(r);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Constructing a ray from a point and a vector.         *
     *                                                       *
     * RETURNS: Constructed ray.                             *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    TR_ray *TRI_make_ray_vector(TR_ray *r, Vector& from, Vector& vector)
    {
        V_set(r->tr_start, from);
        V_set(r->tr_codirected, vector);
        return(r);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Returns point at distance t from the origine.         *
     *                                                       *
     * RETURNS: Constructed vertex.                          *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Vector& TRI_on_ray(Vector& point, TR_ray *r, float t)
    {
        point[0] = r->tr_start[0] + r->tr_codirected[0] * t;
        point[1] = r->tr_start[1] + r->tr_codirected[1] * t;
        point[2] = r->tr_start[2] + r->tr_codirected[2] * t;
        return(point);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
     * Computing illumination of a intersected surface point.*
     *                                                       *
     * RETURNS: An RGB triple.                               *
     * --------                                              *
    \* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Vector& TRI_illuminate(Vector& light, TR_point_light *l, TR_matter *material,
        Vector& normal, Vector& where, Vector& viewer)
    {
        int i;
        Vector lightvector, viewvector, reflect;
        float diffuseratio, specularratio, specularfun;

        V_unit_vector(lightvector, where, l->tr_centre);
        V_unit_vector(viewvector, where, viewer);

        if ((diffuseratio = V_scalar_product(normal, lightvector)) > 0)
        {
            if (TR_rendering_type&(TR_DIFFUSE | TR_SPECULAR))
            {
                light[0] += l->tr_intensity[0] * material->tr_diffuse[0] * diffuseratio;
                light[1] += l->tr_intensity[1] * material->tr_diffuse[1] * diffuseratio;
                light[2] += l->tr_intensity[2] * material->tr_diffuse[2] * diffuseratio;
            }
            /* diffuse term */
            if (TR_rendering_type&TR_SPECULAR)
            {
                reflect[0] = 2 * diffuseratio*normal[0] - lightvector[0];
                reflect[1] = 2 * diffuseratio*normal[1] - lightvector[1];
                reflect[2] = 2 * diffuseratio*normal[2] - lightvector[2];

                if ((specularratio = V_scalar_product(reflect, viewvector)) > 0)
                {
                    for (specularfun = 1, i = 0; i < material->tr_exponent; i++) specularfun *= specularratio;
                    light[0] += l->tr_intensity[0] * material->tr_specular*specularfun;
                    light[1] += l->tr_intensity[1] * material->tr_specular*specularfun;
                    light[2] += l->tr_intensity[2] * material->tr_specular*specularfun;
                }                                        /* specular term */
            }
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

    bool TRI_shadow_ray(TR_world *w, TR_point_light *l, Vector& point, int cur_obj)
    {
        float t = 0.0;
        size_t size = w->tr_objects.size();
        TR_ray r;

        TRI_make_ray_point(&r, point, l->tr_centre);
        for (size_t i = 0; i < size; ++i)            /* finding intersection */
            if (i != cur_obj)
            {
                t = w->tr_objects[i]->TR_intersect(&r);
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

    Vector& TRI_direct_ray(Vector& light, TR_world *w, TR_ray *r, int cur_obj, int depth)
    {
        int min = 0, no_inter = 0;
        float objt[TR_MAX_SPHERES], t = 0.0;
        int obj[TR_MAX_SPHERES];
        TR_ray rr;
        Vector where;                 /* current intersection */
        Vector normal;                /* of the current intersection */
        Vector viewer, reflect, rlight;
        size_t osize = w->tr_objects.size(), lsize = w->tr_point_lights.size();

        if (depth != 0)
        {
            for (size_t i = 0; i < osize; ++i)           /* finding intersection */
                if (i != cur_obj)                           /* with itself, no sense */
                {
                    t = w->tr_objects[i]->TR_intersect(r);
                    if (t > 0)
                    {/* not behind the ray */
                        objt[no_inter] = t;
                        obj[no_inter++] = i;
                    }                /* a valid intersection */
                }

            if (no_inter != 0)                           /* if some objects intersected */
            {
                for (size_t i = 1; i < no_inter; ++i)
                    if (objt[min]>objt[i]) min = i;            /* finding closest intersection */

                light[0] += w->tr_objects[obj[min]]->tr_material.tr_ambient[0] * w->tr_ambient[0];
                light[1] += w->tr_objects[obj[min]]->tr_material.tr_ambient[1] * w->tr_ambient[1];
                light[2] += w->tr_objects[obj[min]]->tr_material.tr_ambient[2] * w->tr_ambient[2];

                TRI_on_ray(where, r, objt[min]);           /* intersection's coordinate */

                w->tr_objects[obj[min]]->TR_normal(normal, where);

                for (size_t i = 0; i < lsize; ++i)     /* illumination from each light */
                    if ((!TRI_shadow_ray(w, w->tr_point_lights[i], where, obj[min])) || (!(TR_rendering_type & TR_SHADOW)))
                        TRI_illuminate(light, w->tr_point_lights[i], &w->tr_objects[obj[min]]->tr_material,
                            normal, where, TR_viewer);

                if (TR_rendering_type & TR_REFLECT)
                {
                    V_unit_vector(viewer, where, TR_viewer);
                    V_multiply(reflect, normal, V_scalar_product(normal, viewer) * 2);
                    V_difference(reflect, reflect, viewer);
                    TRI_make_ray_vector(&rr, where, reflect); /* prepare recursive ray */

                    TRI_direct_ray(V_zero(rlight), w, &rr, obj[min], depth - 1);
                    light[0] += rlight[0] * w->tr_objects[obj[min]]->tr_material.tr_reflect;
                    light[1] += rlight[1] * w->tr_objects[obj[min]]->tr_material.tr_reflect;
                    light[2] += rlight[2] * w->tr_objects[obj[min]]->tr_material.tr_reflect;
                }
            }
        }
        return(light);
    }

    /**********************************************************\
     * Setting the rendering type.                            *
    \**********************************************************/

    void TR_init_rendering(int type)
    {
        TR_rendering_type = type;
    }

    /**********************************************************\
     * Setting the camera parameter, where TR_viewer stores   *
     * position of the viewer's eye, TR_screen origine of the *
     * projection plane, TR_screen_u and TR_screen_v          *
     * orientation of the projection plane in the world       *
     * space.                                                 *
    \**********************************************************/
    //摄像机参数的设置，
    //TR_viever存储遮视窗eye的位置?
    //TR_screen是投影平面??
    //TR_screen_u & TR_sreen_v 是世界空间在投影平面上的投影
    void TR_set_camera(float viewer_x, float viewer_y, float viewer_z,
        float screen_x, float screen_y, float screen_z,
        float screen_ux, float screen_uy, float screen_uz,
        float screen_vx, float screen_vy, float screen_vz
        )
    {
        V_vector_coordinates(TR_viewer, viewer_x, viewer_y, viewer_z);
        V_vector_coordinates(TR_screen, screen_x, screen_y, screen_z);
        V_vector_coordinates(TR_screen_u, screen_ux, screen_uy, screen_uz);
        V_vector_coordinates(TR_screen_v, screen_vx, screen_vy, screen_vz);
    }

    /**********************************************************\
     * Finding intersection of a ray with a sphere.           *
     *                                                        *
     * RETURNS: Distance from the origine of the ray.         *
     * --------                                               *
    \**********************************************************/

    float TR_sphere::TR_intersect(TR_ray *r)
    {
        float a, b, c, det;
        Vector d;

        a = V_scalar_product(r->tr_codirected, r->tr_codirected);
        b = 2 * V_scalar_product(r->tr_codirected, V_difference(d, r->tr_start, tr_centre));
        c = V_scalar_product(d, d) - tr_radius*tr_radius;

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

    Vector& TR_sphere::TR_normal(Vector& normal, Vector& where)
    {
        V_unit_vector(normal, tr_centre, where);  /* from the centre */
        return(normal);
    }

    /**********************************************************\
     * Computes plane equation and the equations delimiting   *
     * the edges.                                             *
    \**********************************************************/

    //计算平面方程和边的投影公式
    //多边形的初始化
    void TR_polygon::TR_init()
    {
        Vector a, b;
        //指定类型

        V_vector_points(a, tr_vertices[2], tr_vertices[1]);
        V_vector_points(b, tr_vertices[1], tr_vertices[0]);
        V_vector_product(tr_normal, a, b);        /* normal to the plane */

        V_zero(a);                                 /* making it unit length */
        V_unit_vector(tr_normal, a, tr_normal);

        for (size_t i = 0; i < tr_vertices.size() - 1; ++i)           /* finding equations for edges */
        {
            V_vector_points(a, tr_vertices[i], tr_vertices[i + 1]);
            V_vector_product(b, tr_normal, a);
            tr_edges.push_back(Plane());
            V_plane(tr_edges[i], b, tr_vertices[i]);
        }
    }

    /**********************************************************\
     * Finding intersection of a ray with a polygon.           *
     *                                                        *
     * RETURNS: Distance from the origine of the ray.         *
     * --------                                               *
    \**********************************************************/

    float TR_polygon::TR_intersect(TR_ray *r)
    {
        Vector a;
        float t, s1, s2;
        size_t size = tr_vertices.size() - 1;

        V_difference(a, tr_vertices[0], r->tr_start);
        s1 = V_scalar_product(a, tr_normal);
        s2 = V_scalar_product(r->tr_codirected, tr_normal);

        if (s2 == 0) return(-1); else t = s1 / s2;
        if (t < 0) return(-1);

        TRI_on_ray(a, r, t);

        for (size_t i = 0; i < size; i++)
            if (V_vertex_on_plane(tr_edges[i], a)>0) return(-1);

        return(t);
    }

    /**********************************************************\
     * Returns polygon's normal.                              *
     *                                                        *
     * RETURNS: The Normal vector.                            *
     * --------                                               *
    \**********************************************************/

    Vector& TR_polygon::TR_normal(Vector& normal, Vector& where)
    {
        normal[0] = tr_normal[0];
        normal[1] = tr_normal[1];
        normal[2] = tr_normal[2];
        return(normal);
    }

    /**********************************************************\
     * Initialisez all entities in the world.                 *
    \**********************************************************/

    void TR_init_world(TR_world *w)
    {
        for (TR_generic_object* o : w->tr_objects)
            o->TR_init();
    }

    /**********************************************************\
     * Ray tracing a scene for all pixels.                    *
    \**********************************************************/

    void TR_trace_world(TR_world *w, int depth)
    {
        float x, y;
        Vector l;//light
        Vector point;
        int coord[2];
        TR_ray r;

        for (coord[0] = 0; coord[0] < HW_SCREEN_X_SIZE; ++coord[0])
            for (coord[1] = 0; coord[1] < HW_SCREEN_Y_SIZE; ++coord[1])           /* for each pixel on screen */
            {
                x = coord[0] - HW_SCREEN_X_SIZE / 2;
                y = coord[1] - HW_SCREEN_Y_SIZE / 2;                  /* plane coordinates */

                point[0] = TR_screen_u[0] * x + TR_screen_v[0] * y + TR_screen[0];
                point[1] = TR_screen_u[1] * x + TR_screen_v[1] * y + TR_screen[1];
                point[2] = TR_screen_u[2] * x + TR_screen_v[2] * y + TR_screen[2];

                //从两个点(point and TR_viewer)构建ray
                TRI_make_ray_point(&r, TR_viewer, point);

                //关键中的关键，计算环境光，返回pixel的照明度
                TRI_direct_ray(V_zero(l), w, &r, -1, depth);

                //Setting a pixel
                G_pixel(coord,
                    CL_colour(l[0] * CL_COLOUR_LEVELS, l[1] * CL_COLOUR_LEVELS, l[2] * CL_COLOUR_LEVELS));
            }
    }
