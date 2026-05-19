#pragma once

#include <math.h>

#define M_PI 3.14159265359

namespace math
{
    struct Point
    {
        float x, y, z;
    };

    struct Vector
    {
        float x, y, z;

        Vector(float x_ = 0, float y_ = 0, float z_ = 0) noexcept
            : x(x_), y(y_), z(z_) { }

        Vector(const Point &P1, const Point &P2) noexcept
            : x(P2.x - P1.x), y(P2.y - P1.y), z(P2.z - P1.z) { }

        Vector operator+(const Vector &other) const noexcept
        {
            return Vector(this->x + other.x,
                    this->y + other.y,
                    this->z + other.z
                );
        }

        Vector operator-(const Vector &other) const noexcept
        {
            return Vector(this->x - other.x,
                    this->y - other.y,
                    this->z - other.z
                );
        }

        float operator*(const Vector &other) const noexcept
        { return this->x * other.x + this->y * other.y + this->z * other.z; }

        Vector scale(const float &alpha) const noexcept
        {
            return Vector(this->x * alpha,
                    this->y * alpha,
                    this->z * alpha
                );
        }

        float norm(void) const noexcept
        { return sqrt(this->x * this->x + this->y * this->y + this->z * this->z); }

        void normalize(void) noexcept
        {
            float n = this->norm();

            this->x = this->x / n;
            this->y = this->y / n;
            this->z = this->z / n;
        }
    };

    Vector cross(const Vector &lhs, const Vector &rhs)
    {
        return Vector( lhs.y * rhs.z - lhs.z * rhs.y,
                    lhs.z * rhs.x - lhs.x * rhs.z,
                    lhs.x * rhs.y - lhs.y * rhs.x
                );
    }

    struct Sph
    {
        float theta, psi;

        Sph(float theta_ = 0, float psi_ = 0) noexcept
            : theta(theta_), psi(psi_) { }
    };

    Point sph_to_cart(Sph &sph)
    {
        return {static_cast<float>(cos(sph.theta * M_PI / 180.0f)) * static_cast<float>(cos(sph.psi * M_PI / 180.0f)), 
            static_cast<float>(cos(sph.theta * M_PI / 180.0f)) * static_cast<float>(sin(sph.psi * M_PI / 180.0f)), 
            static_cast<float>(sin(sph.theta * M_PI / 180.0f))
        }; 
    }

    Sph cart_to_sph(Point &P)
    {
        return {
            static_cast<float>(atan2(P.z, sqrt(P.x * P.x + P.y * P.y)) * 180.0f / M_PI), 
            static_cast<float>(atan2(P.y, P.x) * 180.0f / M_PI)
        }; 
    }

    struct Angle
    {
        int deg, min;
        float sec;
    };

    Angle convert(float alpha)
    {
        int sign = (alpha < 0) ? -1 : 1;
        float a_alpha = fabs(alpha);

        int deg = static_cast<int>(floor(a_alpha));
        float min_full = (a_alpha - deg) * 60.0;

        int min = static_cast<int>(floor(min_full));
        
        float sec = (min_full - min) * 60.0;

        return {sign * deg, min, sec};
    }

    float revert(const Angle &angle)
    {
        const float a_value = fabs(angle.deg) + angle.min / 60.0f + angle.sec / 3600.0f;
        
        return (angle.deg < 0) ? -a_value : a_value;
    }

    void orthonormal(Vector &v1, Vector &v2)
    {
        v2 = v2 - v1.scale((v1 * v2) / (v1 * v1));
        v1.normalize();
        v2.normalize();
    }
}