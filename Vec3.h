#ifndef VEC3_H
#define VEC3_H
class Vec3
{
    public:
        float _v[3];

        Vec3() 
        {

        }

        Vec3(float x, float y, float z)
        {
            _v[0]=x;
            _v[1]=y;
            _v[2]=z;
        }

        inline bool operator == (const Vec3& v) const
        {
            return 
                _v[0] == v._v[0] 
             && _v[1] == v._v[1]
             && _v[2] == v._v[2];
        }

        inline bool operator != (const Vec3& v) const
        {
            return 
                _v[0] == v._v[0] 
             && _v[1] == v._v[1]
             && _v[2] == v._v[2];
        }

        inline bool operator < (const Vec3& v) const
        {
            if(_v[0] < v._v[0]) return true;
            else if (_v[0] > v._v[0]) return false;
            else if (_v[1] < v._v[1]) return true;
            else if (_v[1] > v._v[1]) return false;
            else return (_v[2] < v._v[2]);
        }

        inline float* ptr()
        {
            return _v;
        }

        inline const float* ptr() const
        {
            return _v;
        }

        inline void set(float x, float y, float z)
        {
            _v[0] = x;
            _v[1] = y;
            _v[2] = z;
        }

        inline float& operator [] (unsigned int i)
        {
            return _v[i];
        }

        inline float operator [] (unsigned int i) const
        {
            return _v[i];
        }

        inline float& x() { return _v[0]; }
        inline float& y() { return _v[1]; }
        inline float& z() { return _v[2]; }

        inline float x() const { return _v[0]; }
        inline float y() const { return _v[1]; }
        inline float z() const { return _v[2]; }

        inline float& r() { return _v[0]; }
        inline float& g() { return _v[1]; }
        inline float& b() { return _v[2]; }

        inline float r() const { return _v[0]; }
        inline float g() const { return _v[1]; }
        inline float b() const { return _v[2]; }

        inline bool valid() const { return !isNaN(); }
        inline bool isNaN() const { return false; }

        inline float operator * (const Vec3& rhs) const
        {
            return _v[0] * rhs._v[0] +
                   _v[1] * rhs._v[1] +
                   _v[2] * rhs._v[2];
        }

        inline Vec3 operator * (float rhs) const
        {
            return Vec3(
                _v[0] * rhs,
                _v[1] * rhs,
                _v[2] * rhs
                );
        }

        inline Vec3& operator *= (float rhs)
        {
            _v[0] *= rhs;
            _v[1] *= rhs;
            _v[2] *= rhs;

            return *this;
        }

        inline Vec3 operator / (float rhs) const
        {
            return Vec3(
                _v[0] / rhs,
                _v[1] / rhs,
                _v[2] / rhs
                );
        }

        inline Vec3& operator /= (float rhs)
        {
            _v[0] /= rhs;
            _v[1] /= rhs;
            _v[2] /= rhs;

            return *this;
        }

        inline Vec3 operator + (const Vec3& rhs) const
        {
            return Vec3(
                _v[0] + rhs._v[0],
                _v[1] + rhs._v[1],
                _v[2] + rhs._v[2]
                );
        }

        inline Vec3 operator += (const Vec3& rhs)
        {
            _v[0] += rhs._v[0];
            _v[1] += rhs._v[1];
            _v[2] += rhs._v[2];

            return *this;
        }

        inline Vec3 operator - (const Vec3& rhs) const
        {
            return Vec3(
                _v[0] - rhs._v[0],
                _v[1] - rhs._v[1],
                _v[2] - rhs._v[2]
                );
        }

        inline Vec3& operator -= (const Vec3& rhs)
        {
            _v[0] -= rhs._v[0];
            _v[1] -= rhs._v[1];
            _v[2] -= rhs._v[2];

            return *this;
        }

        inline const Vec3 operator - () const
        {
            return Vec3 (
                -_v[0],
                -_v[1],
                -_v[2]
                );
        }

        inline float length() const
        {
            return sqrtf(Vec3::length2());
        }

        inline float length2() const
        {
            return _v[0] * _v[0] +
                   _v[1] * _v[1] +
                   _v[2] * _v[2];
        }

        inline float normalize()
        {
            float norm = Vec3::length();
            if (norm > 0.0f)
            {
                float inv = 1.0f / norm;
                _v[0] *= inv;
                _v[1] *= inv;
                _v[2] *= inv;
            }

            return (norm);
        }


};

inline Vec3 interpolate (const Vec3 lhs, const Vec3 rhs, const float vallhs, const float valrhs, const float level)
{
    if(fabs(level - vallhs) < 0.00001) {
        return (lhs);
    }
    if(fabs(level - valrhs) < 0.00001) {
        return (rhs);
    }
    if(fabs(vallhs - valrhs) < 0.00001) {
        return (lhs);
    }
    
    float mu;

    Vec3 p;

    mu = (level - vallhs) / (valrhs - vallhs);

    p._v[0] = lhs._v[0] + mu * (rhs._v[0] - lhs._v[0]);
    p._v[1] = lhs._v[1] + mu * (rhs._v[1] - lhs._v[1]);
    p._v[2] = lhs._v[2] + mu * (rhs._v[2] - lhs._v[2]);

    return p;
}

inline Vec3 cross(const Vec3& lhs, const Vec3& rhs)
{
    return Vec3(
        lhs._v[1] * rhs._v[2] - lhs._v[2] * rhs._v[1],
        lhs._v[2] * rhs._v[0] - lhs._v[0] * rhs._v[2],
        lhs._v[0] * rhs._v[1] - lhs._v[1] * rhs._v[0]
    );
}

inline float dot(const Vec3& lhs, const Vec3& rhs)
{
    return lhs._v[0] * rhs._v[0] + 
           lhs._v[1] * rhs._v[1] +
           lhs._v[2] * rhs._v[2];
}

#endif