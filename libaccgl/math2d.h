#ifndef __MATH_2D_H__
#define __MATH_2D_H__


#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>


class Matrix;


// 2D vector
class Vector
{
    friend class Matrix;

    private:
        // x, y and 1
        float m[3];

    public:
        // create entity vector
        Vector() { m[0] = m[1] = 0; m[2] = 1; };

        // create 2d vector
        Vector(float x, float y) { m[0] = x;  m[1] = y; m[2] = 1; };

    public:
        // Return x component
        float getX() const { return m[0]; };

        // Return y component
        float getY() const { return m[1]; };

    public:
        // multipy matrix and vector
        friend Vector operator * (const Matrix &m, const Vector &v);
};


// 2D transformation matrix
class Matrix
{
    private:
        // values
        float m[3][3];

    public:
        // create rotation matrix
        static Matrix identity();

        // create rotation matrix
        static Matrix rotate(float angle);
        
        // create scale matrix
        static Matrix scale(float x, float y);
        
        // create translation matrix
        static Matrix translate(float x, float y);

    public:
        // multipy 2 matrices
        friend Matrix operator * (const Matrix &m1, const Matrix &m2);
        
        // multipy matrix and vector
        friend Vector operator * (const Matrix &m, const Vector &v);
};



inline Matrix Matrix::identity()
{
    Matrix m;
    m.m[0][0] = 1.0;  m.m[1][0] = 0.0;  m.m[2][0] = 0.0;
    m.m[0][1] = 0.0;  m.m[1][1] = 1.0;  m.m[2][1] = 0.0;
    m.m[0][2] = 0.0;  m.m[1][2] = 0.0;  m.m[2][2] = 1.0;
    return m;
}


inline Matrix Matrix::rotate(float angle)
{
    Matrix m;
    angle = angle * M_PI / 180.0;
    float cf = cos(angle);
    float sf = sin(angle);
    m.m[0][0] = cf;   m.m[1][0] = -sf;  m.m[2][0] = 0.0;
    m.m[0][1] = sf;   m.m[1][1] = cf;   m.m[2][1] = 0.0;
    m.m[0][2] = 0.0;  m.m[1][2] = 0.0;  m.m[2][2] = 1.0;
    return m;
}


inline Matrix Matrix::scale(float x, float y)
{
    Matrix m;
    m.m[0][0] = x;    m.m[1][0] = 0.0;  m.m[2][0] = 0.0;
    m.m[0][1] = 0.0;  m.m[1][1] = y;    m.m[2][1] = 0.0;
    m.m[0][2] = 0.0;  m.m[1][2] = 0.0;  m.m[2][2] = 1.0;
    return m;
}


inline Matrix Matrix::translate(float x, float y)
{
    Matrix m;
    m.m[0][0] = 1.0;  m.m[1][0] = 0.0;  m.m[2][0] = x;
    m.m[0][1] = 0.0;  m.m[1][1] = 1.0;  m.m[2][1] = y;
    m.m[0][2] = 0.0;  m.m[1][2] = 0.0;  m.m[2][2] = 1.0;
    return m;
}


inline Matrix operator * (const Matrix &m1, const Matrix &m2)
{
    Matrix r;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            float sum = 0.0;
            for (int k = 0; k < 3; k++)
                sum += m1.m[i][k] * m2.m[k][j];
            r.m[i][j] = sum;
        }
    return r;
}

inline Vector operator * (const Matrix &m1, const Vector &v1)
{
    float x = m1.m[0][0] * v1.m[0] + m1.m[1][0] * v1.m[1] + m1.m[2][0];
    float y = m1.m[0][1] * v1.m[0] + m1.m[1][1] * v1.m[1] + m1.m[2][1];
    return Vector(x, y);
}


#endif

