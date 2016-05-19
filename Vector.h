#ifndef _VECTOR_H_
#define _VECTOR_H_

//三维矢量，呵呵
#define V_LNG_VECTOR 3                      /* X Y Z */

class Vector
{
private:
    float v[3];
public:
    Vector() {}
    Vector(float x, float y, float z)
    {
        v[0] = x; v[1] = y; v[2] = z;
    }
    float& operator[](const int _v)
    {
        return v[_v];
    }
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
};

//零矢量
Vector& V_zero(Vector& vec);

//矢量的赋值
Vector& V_vector_coordinates(Vector& vector,float x,float y,float z);

//矢量的计算(from & to)
Vector& V_vector_points(Vector& vector,Vector& from,Vector& to);

//to what?设置矢量V Set
Vector& V_set(Vector& what,Vector& to);

//矢量倍增
Vector& V_multiply(Vector& result,Vector& vector,float m);

//两个向量的积
float V_scalar_product(Vector& a,Vector& b);

//这个是什么计算???不记得了
Vector& V_vector_product(Vector& product,Vector& a,Vector& b);

//矢量和
Vector& V_sum(Vector& sm,Vector& a,Vector& b);

//矢量差
Vector& V_difference(Vector& differ,Vector& a,Vector& b);

//矢量单位化
Vector& V_unit_vector(Vector& vector,Vector& from,Vector& to);

//计算一个平面等式的系数
Plane& V_plane(Plane& plane,Vector& normal,Vector& origine);

//检查点是否在平面上
float V_vertex_on_plane(Plane& plane,Vector& vertex);

#endif
