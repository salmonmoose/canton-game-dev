#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vec3.h"

class Triangle
{
    public:
        Vector::Vec3 _vec[3];

        Triangle() 
        {
            _vec[0] = Vector::Vec3();
            _vec[1] = Vector::Vec3();
            _vec[2] = Vector::Vec3();
        }

        Triangle(Vector::Vec3 p0, Vector::Vec3 p1, Vector::Vec3 p2) 
        {
            _vec[0] = p0;
            _vec[1] = p1;
            _vec[2] = p2;
        }

        inline Vector::Vec3* ptr()
        {
            return _vec;
        }

        inline const Vector::Vec3* prt() const
        {
            return _vec;
        }

        inline void set(Vector::Vec3 p0, Vector::Vec3 p1, Vector::Vec3 p2)
        {
            _vec[0] = p0;
            _vec[1] = p1;
            _vec[2] = p2;
        }

        inline Vector::Vec3& operator [] (unsigned int i)
        {
            return _vec[i];
        }

        inline Vector::Vec3 operator [] (unsigned int i) const
        {
            return _vec[i];
        }

        inline Vector::Vec3 normal ()
        {
            Vector::Vec3 n;

            n = Vector::cross((_vec[1] - _vec[0]), (_vec[0] - _vec[2]));

            n.normalize();

            return n;
        }
};
#endif