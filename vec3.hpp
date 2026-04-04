#pragma once

#include <math.h>
#include <iostream>

namespace __detail
{
    struct Point
    {
        double x, y, z;

        Point(double x, double y, double z)
            : x(x), y(y), z(z) { }

        void print()
        {
            std::cout << x << " " << y << " " << z << std::endl;
        }
    };

    struct Vector
    {
        Vector() :
            x(0), y(0), z(0) { }

        Vector(Point P1, Point P2)
            : x(P2.x - P1.x), y(P2.y - P1.y), z(P2.z - P1.z) { }

        Vector(double x, double y, double z)
            : x(x), y(y), z(z) { }

        Vector(const Vector &other)
            : x(other.x), y(other.y), z(other.z) { }

        Vector& operator=(const Vector &other)
        {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;

            return *this;
        }

        void print()
        {
            std::cout << x << " " << y << " " << z << std::endl; 
        }

        Vector operator+(const Vector &rhs) const
        { return Vector(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z); }

        Vector operator-(const Vector &rhs) const 
        { return Vector(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z); }

        double operator*(const Vector &rhs) const
        { return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z; }

        Vector scale(const double &alpha) const
        { return Vector(this->x * alpha, this->y * alpha, this->z * alpha); }


        static Vector cross(const Vector &lhs, const Vector &rhs)
        {
            return Vector( lhs.y * rhs.z - lhs.z * rhs.y,
                           lhs.z * rhs.x - lhs.x * rhs.z,
                           lhs.x * rhs.y - lhs.y * rhs.x);
        }

        double norm()
        { return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z); }

        void normalize()
        { 
            double n = this->norm();

            this->x = this->x / n;
            this->y = this->y / n;
            this->z = this->z / n;
        }

        double x, y, z;
    };

    struct Sph
    {
        Sph() noexcept
            : theta(0), psi(0) { }

        Sph(double theta, double psi)
            : theta(theta), psi(psi) { }

        void print()
        {
            std::cout << theta << " " << psi << std::endl;
        }

        double theta, psi;
    };

    Point sph_to_cart(Sph &sph)
    { 
        return Point(cos(sph.theta * M_PI / 180) * cos(sph.psi * M_PI / 180), 
            cos(sph.theta * M_PI / 180) * sin(sph.psi * M_PI / 180), sin(sph.theta * M_PI / 180)
        ); 
    }

    Sph cart_to_sph(Point &v)
    { 
        return Sph(
            atan2(v.z, std::sqrt(v.x * v.x + v.y * v.y)) * 180 / M_PI, atan2(v.y, v.x) * 180 / M_PI
        ); 
    }

    struct Angle
    {
        int deg, min;
        double sec;

        Angle() = default;

        Angle(int deg, int min, double sec)
            : deg(deg), min(min), sec(sec) { }
    };

    Angle convert(double alpha)
    {
        const int sign = (alpha < 0.0) ? -1 : 1;
        const double abs_alpha = std::abs(alpha);

        const int deg = static_cast<int>(std::floor(abs_alpha));
        const double min_full = (abs_alpha - deg) * 60.0;
        const int min = static_cast<int>(std::floor(min_full));
        const double sec = (min_full - min) * 60.0;

        return Angle(sign * deg, min, sec);
    }

    double revert(const Angle& ang)
    {
        const double abs_value =
            std::abs(ang.deg) + ang.min / 60.0 + ang.sec / 3600.0;

        return (ang.deg < 0) ? -abs_value : abs_value;
    }

    void print_angle(Angle ang)
    {
        std::cout << ang.deg << " deg " << std::abs(ang.min) << "\' " << std::abs(ang.sec) << "\"\n"; 
    }
    
    void orthonormal(Vector &v1, Vector &v2)
    {
        v2 = v2 - v1.scale((v1 * v2) / (v1 * v1));
        v1.normalize();
        v2.normalize();
    }
}

typedef __detail::Sph    Sph;
typedef __detail::Vector Vector;
typedef __detail::Point  Point;