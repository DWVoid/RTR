#include <math.h>                           /* sqrt */
#include "vector.h"               /* self definition */

/**********************************************************\
 * Initialize a vector to zero.                           *
 *                                                        *
 * RETURNS: Initializeed vector.                          *
 * --------                                               *
\**********************************************************/

Vector& V_zero(Vector& vec)
{
    //一个float型的指针和一个矢量有什么关系?
    vec[0] = 0; vec[1] = 0; vec[2] = 0; return(vec);
}

/**********************************************************\
 * Sets the coordinates of a vector.                      *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_vector_coordinates(Vector& vector, float x, float y, float z)
{
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
	return(vector);
}

/**********************************************************\
 * Computes a vector base on the coordinates of the       *
 * beginning and the end.                                 *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_vector_points(Vector& vector, Vector& from, Vector& to)
{
	vector[0] = to[0] - from[0];
	vector[1] = to[1] - from[1];
	vector[2] = to[2] - from[2];
	return(vector);
}

/**********************************************************\
 * Copy a vector from another vector.                     *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_set(Vector& what, Vector& to)
{
	what[0] = to[0]; what[1] = to[1]; what[2] = to[2];
	return(what);
}

/**********************************************************\
 * Multiply a vector by a scalar number.                  *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_multiply(Vector& result, Vector& vector, float m)
{
	result[0] = vector[0] * m; result[1] = vector[1] * m; result[2] = vector[2] * m;
	return(result);
}

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
 * Computes a sum of two vectors.                         *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_sum(Vector& sm, Vector& a, Vector& b)
{
	sm[0] = a[0] + b[0]; sm[1] = a[1] + b[1]; sm[2] = a[2] + b[2];
	return(sm);
}

/**********************************************************\
 * Computes difference of two vectors.                    *
 *                                                        *
 * RETURNS: Resulting vector.                             *
 * --------                                               *
\**********************************************************/

Vector& V_difference(Vector& differ, Vector& a, Vector& b)
{
	differ[0] = a[0] - b[0]; differ[1] = a[1] - b[1]; differ[2] = a[2] - b[2];
	return(differ);
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

/**********************************************************\
 * Computes coefficients of a plane equation.             *
 *                                                        *
 * RETURNS: Pointer to the coefficients.                  *
 * --------                                               *
\**********************************************************/

Plane& V_plane(Plane& plane, Vector& normal, Vector& origine)
{
	plane[0] = normal[0];
	plane[1] = normal[1];
	plane[2] = normal[2];
	plane[3] = -(origine[0] * normal[0] + origine[1] * normal[1] + origine[2] * normal[2]);

	return(plane);
}

/**********************************************************\
 * Checking if a vertex belongs to the plane.             *
 *                                                        *
 * RETURNS: 0   when the vertex belongs to the plane;     *
 * -------- "+" when in the normal pointed                *
 *              halfplane;                                *
 *          "-" otherwise.                                *
\**********************************************************/

float V_vertex_on_plane(Plane& plane, Vector& vertex)
{
	return(vertex[0] * plane[0] +
		vertex[1] * plane[1] +
		vertex[2] * plane[2] +
		plane[3]
		);
}

/**********************************************************/
