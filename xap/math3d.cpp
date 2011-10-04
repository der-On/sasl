#include <math.h>
#include <stdlib.h>
#include "math3d.h"

using namespace xap;


double xap::distance(const Vector &v1, const Vector &v2)
{
    double mx = v1.x - v2.x;
    double my = v1.y - v2.y;
    double mz = v1.z - v2.z;
    return sqrt(mx*mx + my*my + mz*mz);
}


Vector xap::operator ^ (const Vector& u, const Vector& v)
{
    return Vector(u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x);
}


Matrix::Matrix(double v)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            x[i][j] = (i == j) ? v : 0.0;
    x[3][3] = 1;
}

void Matrix::invert()
{
    Matrix out(1.0);

    for (int i = 0; i < 4; i++)
    {
        double d = x[i][i];
        if (d != 1.0)
        {
            for (int j = 0; j < 4; j++)
            {
                out.x[i][j] /= d;
                x[i][j] /= d;
            }
        }
        for (int j = 0; j < 4; j++)
        {
            if (j != i)
            {
                if (x[j][i] != 0.0)
                {
                    double mulBy = x[j][i];
                    for (int k = 0; k < 4; k++)
                    {
                        x[j][k] -= mulBy * x[i][k];
                        out.x[j][k] -= mulBy * out.x[i][k];
                    }
                }
            }
        }
    }
    *this = out;
}

void Matrix::transpose()
{
    double t;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (i != j)
            {
                t = x[i][j]; x[i][j] = x[j][i]; x[j][i] = t;
            }
}
        

void Matrix::transform(Vector &v)
{
    Vector res;

    res.x = v.x*x[0][0] + v.y*x[1][0] + v.z*x[2][0] + x[3][0];
    res.y = v.x*x[0][1] + v.y*x[1][1] + v.z*x[2][1] + x[3][1];
    res.z = v.x*x[0][2] + v.y*x[1][2] + v.z*x[2][2] + x[3][2];
    double denom = v.x*x[0][3] + v.y*x[1][3] + v.z*x[2][3] + x[3][3];
    if (denom != 1.0)
        res /= denom;
    v = res;
}


Matrix& Matrix::operator += (const Matrix& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            x[i][j] += a.x[i][j];
    return *this;
}

Matrix& Matrix::operator -= (const Matrix& a)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            x[i][j] -= a.x[i][j];
    return *this;
}

Matrix& Matrix::operator *= (double v)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            x[i][j] *= v;
    return *this;
}

Matrix& Matrix::operator *= (const Matrix& a)
{
    Matrix res = *this;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < 4; k++)
                sum += res.x[i][k] * a.x[k][j];
            x[i][j] = sum;
        }
    return *this;
}


Matrix xap::operator + (const Matrix& a, const Matrix& b)
{
    Matrix res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res.x[i][j] = a.x[i][j] + b.x[i][j];
    return res;
}


Matrix xap::operator - (const Matrix& a, const Matrix& b)
{
    Matrix res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res.x[i][j] = a.x[i][j] - b.x[i][j];
    return res;
}


Matrix xap::operator * (const Matrix& a, const Matrix& b)
{
    Matrix res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < 4; k++)
                sum += a.x[i][k] * b.x[k][j];
            res.x[i][j] = sum;
        }
    return res;
}


Matrix xap::operator * (const Matrix& a, double v)
{
    Matrix res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            res.x[i][j] = a.x[i][j] * v;
    return res;
}


Vector xap::operator * (const Matrix& m, const Vector& v)
{
    Vector res;

    res.x = v.x*m.x[0][0] + v.y*m.x[1][0] + v.z*m.x[2][0] + m.x[3][0];
    res.y = v.x*m.x[0][1] + v.y*m.x[1][1] + v.z*m.x[2][1] + m.x[3][1];
    res.z = v.x*m.x[0][2] + v.y*m.x[1][2] + v.z*m.x[2][2] + m.x[3][2];
    double denom = v.x*m.x[0][3] + v.y*m.x[1][3] + v.z*m.x[2][3] + m.x[3][3];
    if (denom != 1.0)
        res /= denom;
    return res;
}


Matrix xap::translate(const Vector& loc)
{
    Matrix res(1.0);
    res.x[3][0] = loc.x;
    res.x[3][1] = loc.y;
    res.x[3][2] = loc.z;
    return res;
}


Matrix xap::scale(const Vector& loc)
{
    Matrix res(1.0);
    res.x[0][0] = loc.x;
    res.x[1][1] = loc.y;
    res.x[2][2] = loc.z;
    return res;
}


Matrix xap::rotateX(double angle)
{
    Matrix res(1.0);
    double cs = cos(angle);
    double s = sin(angle);
    res.x[1][1] = cs;
    res.x[2][1] = -s;
    res.x[1][2] = s;
    res.x[2][2] = cs;
    return res;
}


Matrix xap::rotateY(double angle)
{
    Matrix res(1.0);
    double cs = cos(angle);
    double s = sin(angle);
    res.x[0][0] = cs;
    res.x[2][0] = -s;
    res.x[0][2] = s;
    res.x[2][2] = cs;
    return res;
}


Matrix xap::rotateZ(double angle)
{
    Matrix res(1.0);
    double cs = cos(angle);
    double s = sin(angle);
    res.x[0][0] = cs;
    res.x[1][0] = -s;
    res.x[0][1] = s;
    res.x[1][1] = cs;
    return res;
}



#define EPSILON 1e-10



Quat::Quat(const Vector& axis, double angle)
{
    double omega,s;
    double l;

    l = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    if (l < EPSILON) 
    {
        v[0] = v[1] = v[2] = 0.0;
        v[3] = 1.0;
        return;
    }
    
    omega = -0.5 * angle;
    s = sin(omega) / l;
    v[0] = s * axis.x;
    v[1] = s * axis.y;
    v[2] = s * axis.z;
    v[3] = cos(omega);
}

Quat::Quat(double ax, double ay, double az)
{
    double cr, cp, cy, sr, sp, sy, cpcy, spsy;
    double degToRad2 = (M_PI / 180.0) / 2.0;

    cr = (double)cos(az*degToRad2);
    cp = (double)cos(ay*degToRad2);
    cy = (double)cos(ax*degToRad2);

    sr = (double)sin(az*degToRad2);
    sp = (double)sin(ay*degToRad2);
    sy = (double)sin(ax*degToRad2);

    cpcy = cp * cy;
    spsy = sp * sy;

    v[3] = cr * cpcy + sr * spsy;
    v[0] = sr * cpcy - cr * spsy;
    v[1] = cr * sp * cy + sr * cp * sy;
    v[2] = cr * cp * sy - sr * sp * cy;
}

Quat& Quat::operator += (const Quat& q)
{
    v[0] += q.v[0];
    v[1] += q.v[1];
    v[2] += q.v[2];
    v[3] += q.v[3];
    return *this;
}


Quat& Quat::operator *= (const Quat& q)
{
    double px,py,pz,pw;

    px=v[0];
    py=v[1];
    pz=v[2];
    pw=v[3];
    
    v[0]=pw*q.v[0] + px*q.v[3] + py*q.v[2] - pz*q.v[1];
    v[1]=pw*q.v[1] + py*q.v[3] + pz*q.v[0] - px*q.v[2];
    v[2]=pw*q.v[2] + pz*q.v[3] + px*q.v[1] - py*q.v[0];
    v[3]=pw*q.v[3] - px*q.v[0] - py*q.v[1] - pz*q.v[2];

    return *this;
}


Quat xap::operator + (const Quat& p, const Quat& q)
{
    double x = p.v[0] + q.v[0];
    double y = p.v[1] + q.v[1];
    double z = p.v[2] + q.v[2];
    double w = p.v[3] + q.v[3];
    return Quat(x, y, z, w);
}

Quat xap::operator * (const Quat& p, const Quat& q)
{
/*    double x, y, z, w;
    x = p.v[3]*q.v[0] + p.v[0]*q.v[3] + p.v[1]*q.v[2] - p.v[2]*q.v[1];
    y = p.v[3]*q.v[1] + p.v[1]*q.v[3] + p.v[2]*q.v[0] - p.v[0]*q.v[2];
    z = p.v[3]*q.v[2] + p.v[2]*q.v[3] + p.v[0]*q.v[1] - p.v[1]*q.v[0];
    w = p.v[3]*q.v[3] - p.v[0]*q.v[0] - p.v[1]*q.v[1] - p.v[2]*q.v[2];
    return Quat(x, y, z, w);*/

/*    float A, B, C, D, E, F, G, H;

    A = (p.v[3] + p.v[0]) * (q.v[3] + q.v[0]);
    B = (p.v[2] - p.v[1]) * (q.v[1] - q.v[2]);
    C = (p.v[3] - p.v[0]) * (q.v[1] + q.v[2]);
    D = (p.v[1] + p.v[2]) * (q.v[3] - q.v[0]);
    E = (p.v[0] + p.v[2]) * (q.v[0] + q.v[1]);
    F = (p.v[0] - p.v[2]) * (q.v[0] - q.v[1]);
    G = (p.v[3] + p.v[1]) * (q.v[3] - q.v[2]);
    H = (p.v[3] - p.v[1]) * (q.v[3] + q.v[2]);

    return Quat(A - (E + F + G + H) * 0.5, C + (E - F + G - H) * 0.5,
            D + (E - F - G + H) * 0.5, B + (-E - F + G + H) * 0.5);*/
  
    double t[8];

    t[0] = (p.v[3] + p.v[0]) * (q.v[3] + q.v[0]);
    t[1] = (p.v[2] - p.v[1]) * (q.v[1] - q.v[2]);
    t[2] = (p.v[0] - p.v[3]) * (q.v[1] + q.v[2]);
    t[3] = (p.v[1] + p.v[2]) * (q.v[0] - q.v[3]);
    t[4] = (p.v[0] + p.v[2]) * (q.v[0] + q.v[1]);
    t[5] = (p.v[0] - p.v[2]) * (q.v[0] - q.v[1]);
    t[6] = (p.v[3] + p.v[1]) * (q.v[3] - q.v[2]);
    t[7] = (p.v[3] - p.v[1]) * (q.v[3] + q.v[2]);

    return Quat(t[0] - (( t[4] + t[5] + t[6] + t[7]) * 0.5),
                -t[2] + (( t[4] - t[5] + t[6] - t[7]) * 0.5),
                -t[3] + (( t[4] - t[5] - t[6] + t[7]) * 0.5),
                t[1] + ((-t[4] - t[5] + t[6] + t[7]) * 0.5));
}

Matrix Quat::toMatrix() const
{
/*    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    Matrix m;
    
    x2 = v[0] + v[0];
    y2 = v[1] + v[1];
    z2 = v[2] + v[2];
    xx = v[0] * x2;   xy = v[0] * y2;   xz = v[0] * z2;
    yy = v[1] * y2;   yz = v[1] * z2;   zz = v[2] * z2;
    wx = v[3] * x2;   wy = v[3] * y2;   wz = v[3] * z2;

    m.x[0][0] = 1.0-(yy+zz);  m.x[0][1] = xy-wz;        m.x[0][2] = xz+wy;        m.x[0][3] = 0.0;
    m.x[1][0] = xy+wz;        m.x[1][1] = 1.0-(xx+zz);  m.x[1][2] = yz-wx;        m.x[1][3] = 0.0;
    m.x[2][0] = xz-wy;        m.x[2][1] = yz+wx;        m.x[2][2] = 1.0-(xx+yy);  m.x[2][3] = 0.0;
    m.x[3][0] = 0.0;          m.x[3][1] = 0.0;          m.x[3][2] = 0.0;          m.x[3][3] = 1.0;

    return m;*/

    Matrix m;
    double xx, xy, xz, xw, yy, yz, yw, zz, zw;

    xx = v[0] * v[0];
    xy = v[0] * v[1];
    xz = v[0] * v[2];
    xw = v[0] * v[3];

    yy = v[1] * v[1];
    yz = v[1] * v[2];
    yw = v[1] * v[3];

    zz = v[2] * v[2];
    zw = v[2] * v[3];
    
    m.x[0][0] = 1.0 - 2.0 * (yy + zz);
    m.x[0][1] =       2.0 * (xy - zw);
    m.x[0][2] =       2.0 * (xz + yw);
    m.x[0][3] = 0.0;

    m.x[1][0] =       2.0 * (xy + zw);
    m.x[1][1] = 1.0 - 2.0 * (xx + zz);
    m.x[1][2] =       2.0 * (yz - xw);
    m.x[1][3] = 0.0;

    m.x[2][0] =       2.0 * (xz - yw);
    m.x[2][1] =       2.0 * (yz + xw);
    m.x[2][2] = 1.0 - 2.0 * (xx + yy);
    m.x[2][3] = 0.0;

    m.x[3][0] = m.x[3][1] = m.x[3][2] = 0.0;
    m.x[3][3] = 1.0;

    return m;
}

void Quat::fromSphere(double latitude, double longitude, double angle)
{
    double sin_a = sin(angle / 2.0);
    double cos_a = cos(angle / 2.0);

    double sin_lat = sin(latitude);
    double cos_lat = cos(latitude);

    double sin_long = sin(longitude);
    double cos_long = cos(longitude);

    v[0] = sin_a * cos_lat * sin_long;
    v[1] = sin_a * sin_lat;
    v[2] = sin_a * sin_lat * cos_long;
    v[3] = cos_a;
}

double Quat::norm() const
{
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

Quat Quat::qStar()
{
    return Quat(-v[0], -v[1], -v[2], v[3]);
}


double Quat::getRotation(Vector& axis) const
{
    double omega = acos(v[3]);
    double d_angle = -2.0 * omega;
    double s = sin(omega);
    if (fabs(s)>EPSILON) {
        axis.x = v[0] / s;
        axis.y = v[1] / s;
        axis.z = v[2] / s;
    } else {
        axis.x = 0.0;
        axis.y = 0.0;
        axis.z = 0.0;
    }
    return d_angle;
    
    /*double cos_angle = v[3];
    double angle = acos(cos_angle);
    double sin_angle = sqrt(1.0 - cos_angle * cos_angle);
    if (fabs(sin_angle M 0.0005)*/
    
/*    double scale = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    if (scale != 0.0) {
        axis.x = v[0] / scale;
        axis.y = v[1] / scale;
        axis.z = v[2] / scale;
    } else {
        axis.x = 1.0;
        axis.y = 0.0;
        axis.z = 0.0;
    }
    
    double ac;
    if (v[3] > 1.0)
       ac = acos(1.0);
    else if (v[3] < -1.0)
        ac = acos(-1.0);
    else
        ac = acos(v[3]);
    return 2.0 * ac; */
}

const Quat& Quat::operator /= (const double f)
{
    v[0] /= f;
    v[1] /= f;
    v[2] /= f;
    v[3] /= f;
    return *this;
}

