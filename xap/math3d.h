#ifndef __MATH3D_H__
#define __MATH3D_H__


#include <math.h>
#include <string.h>

namespace xap {

/// 3D Vector
class Vector
{
    public:
        double x, y, z;

    public:
        /// Create vector (0, 0, 0)
        Vector() { x = y = z = 0.0; };

        /// Create vector
        /// \param v value of x, y and z
        Vector(double v) { x = y = z = v; };

        /// Create vector
        /// \param ax x value
        /// \param ay y value
        /// \param az z value
        Vector(double ax, double ay, double az) { x = ax;  y = ay;  z = az; };

        /// Copy constructor
        /// \param v vector to copy
        Vector(const Vector& v) { x = v.x;  y = v.y;  z = v.z; };

    public:
        Vector& operator = (const Vector& v) { x = v.x;  y = v.y;  z = v.z;  return *this; };
        Vector operator - () const;
        Vector& operator += (const Vector& v);
        Vector& operator -= (const Vector& v);
        Vector& operator *= (const Vector& v);
        Vector& operator *= (double v);
        Vector& operator /= (double v);

        friend Vector operator + (const Vector& v1, const Vector& v2);
        friend Vector operator - (const Vector& v1, const Vector& v2);
        friend Vector operator * (const Vector& v1, const Vector& v2);
        friend Vector operator * (double v1, const Vector& v2);
        friend Vector operator * (const Vector& v1, double v2);
        friend Vector operator / (const Vector& v1, const Vector& v2);
        friend Vector operator / (const Vector& v1, double v2);
        friend double operator & (const Vector& u, const Vector& v) { return u.x*v.x + u.y*v.y + u.z*v.z; };
        friend Vector operator ^ (const Vector& v1, const Vector& v2);

        double operator ! () const { return (double)sqrt(x*x + y*y + z*z); };
        double operator [] (int n) { return *(&x + n); };
        int operator < (double v) { return x < v && y < v && z < v; };
        int operator > (double v) { return x > v && y > v && z > v; };
        
        void normalize() { *this /= !*this; };
};

double distance(const Vector &v1, const Vector &v2);
        
Vector operator ^ (const Vector& v1, const Vector& v2);

inline Vector Vector::operator - () const
{
    return Vector(-x, -y, -z);
}

inline Vector operator + (const Vector& u, const Vector& v)
{
    return Vector(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vector operator - (const Vector& u, const Vector& v)
{
    return Vector(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vector operator * (const Vector& u, const Vector& v)
{
    return Vector(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vector operator * (const Vector& u, double f)
{
    return Vector(u.x *f, u.y * f, u.z * f);
}

inline Vector operator * (double f, const Vector& v)
{
    return Vector(f * v.x, f* v.y, f * v.z);
}

inline Vector operator / (const Vector& u, double f)
{
    return Vector(u.x / f, u.y / f, u.z / f);
}

inline Vector operator / (const Vector& u, const Vector& v)
{
    return Vector(u.x / v.x, u.y / v.y, u.z / v.z);
}

inline Vector& Vector::operator += (const Vector& v)
{
    x += v.x;  y += v.y;  z += v.z;
    return *this;
}

inline Vector& Vector::operator -= (const Vector& v)
{
    x -= v.x;  y -= v.y;  z -= v.z;
    return *this;
}

inline Vector& Vector::operator *= (double f)
{
    x *= f;  y *= f;  z *= f;
    return *this;
}

inline Vector& Vector::operator *= (const Vector& v)
{
    x *= v.x;  y *= v.y;  z *= v.z;
    return *this;
}

inline Vector& Vector::operator /= (double f)
{
    x /= f;  y /= f;  z /= f;
    return *this;
}


inline Vector normalize(const Vector& v) { return v / !v; };


/// 4x4 Matrix
class Matrix
{
    public:
        double x[4][4];

    public:
        Matrix() { };
        Matrix(double f);
        Matrix(const Matrix& m) { memcpy(x, m.x, sizeof(x)); };

    public:
        Matrix& operator += (const Matrix& m);
        Matrix& operator -= (const Matrix& m);
        Matrix& operator *= (const Matrix& m);
        Matrix& operator *= (double f);
        Matrix& operator /= (double f);
        void invert();
        void transpose();
        void transform(Vector &v);

        friend Matrix operator + (const Matrix& m1, const Matrix& m2);
        friend Matrix operator - (const Matrix& m1, const Matrix& m2);
        friend Matrix operator * (const Matrix& m1, double f);
        friend Matrix operator * (const Matrix& m1, const Matrix& m2);
        friend Vector operator * (const Matrix& m, const Vector& v);
};


Matrix translate(const Vector& v);
Matrix scale(const Vector& v);
Matrix rotateX(double f);
Matrix rotateY(double f);
Matrix rotateZ(double f);
Matrix rotate(const Vector& v, double f);
Matrix mirrorX(double f);
Matrix mirrorY(double f);
Matrix mirrorZ(double f);


/// Quaternion for rotations
class Quat
{
    public:
        double v[4];

    public:
        Quat() { v[0] = v[1] = v[2] = 0.0; v[3] = 1.0; };
        Quat(double ax, double ay, double az, double aw) { v[0] = ax; v[1] = ay; v[2] = az; v[3] = aw; };
        Quat(const Quat& q) { memcpy(v, q.v, sizeof(v)); };
        Quat(const Vector& axis, double angle);
        Quat(double ax, double ay, double az);
        
    public:
        Quat& operator = (const Quat& q) { memcpy(v, q.v, sizeof(v));  return *this; };
        Quat& operator += (const Quat& q);
        Quat& operator -= (const Quat& q);
        Quat& operator *= (const Quat& q);
        const Quat& operator /= (const double v);

        double norm() const;
        double x() const { return(v[0]); }
        double y() const { return(v[1]); }
        double z() const { return(v[2]); }
        double w() const { return(v[3]); }
        Matrix toMatrix() const;
        void fromSphere(double latitude, double longitude, double angle);
        Quat qStar();
        double getRotation(Vector& axis) const;
        void normalize() { *this /= norm(); };
        
        friend Quat operator + (const Quat& q1, const Quat& q2);
        friend Quat operator * (const Quat& q1, const Quat& q2);
};

#ifndef M_PI
#define M_PI 3.1415926535
#endif

typedef std::vector<Vertex> VertexBuffer;

};

#endif

