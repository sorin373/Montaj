#include "vec3.hpp"
#include <fstream>

using namespace __detail;

const long double factor_de_corectie = 0.01;

Vector normal(Vector v1, Vector v2) { return Vector::cross(v2, v1); }

void corectie_ref_atm(Sph &s)
{
    s.m_theta = s.m_theta - (1 / std::tan((s.m_theta + 7.31 / (s.m_theta + 4.4)) * M_PI / 180)) / 60;
}

long double corectie_ref_atm(double theta)
{
    return theta - (1 / std::tan((theta + 7.31 / (theta + 4.4)) * M_PI / 180)) / 60;
}

long double corectie_inv(double theta)
{
    long double h_ap = theta;

    while (corectie_ref_atm(h_ap) < theta)
    {
        h_ap = h_ap + factor_de_corectie;
    }

    return h_ap;
}

int main()
{
    Point O(0, 0, 0);

    // se primeste dt1 si dt2
    Sph S1(revert({14, 57, 23}), revert({48, 11, 56})), 
        S2(revert({18, 53, 19}), revert({52, 43, 36})), 
        S3(revert({23, 3, 57}),  revert({57, 7, 2}));

    corectie_ref_atm(S1);
    corectie_ref_atm(S2);
    corectie_ref_atm(S3);

    Point P1 = sph_to_cart(S1), P2 = sph_to_cart(S2), P3 = sph_to_cart(S3);

    Vector v1(P1, P2);
    Vector v2(P1, P3);

    orthonormal(v1, v2);

    Vector n = normal(v1, v2);
    n.normalize();

    Vector OP1(O, P1);
    long double k1 = (n * OP1) / (n * n);

    Vector OP2(O, P2);
    long double k2 = (n * OP2) / (n * n);

    Vector OP3(O, P3);
    long double k3 = (n * OP3) / (n * n);

    long double k = (k1 + k2 + k3) / 3;

    Point C(k * n.x, k * n.y, k * n.z);
    Vector OC(O, C);

    const long double R = std::sqrt(1 - (C.x * C.x + C.y * C.y + C.z * C.z));

    long double rhs = (P3.y - C.y) / R - (P3.x - C.x) * v1.y / (R * v1.x);

    long double lhs = v2.y - v1.y * v2.x / v1.x;

    long double b = rhs / lhs;

    long double a = (P3.x - C.x) / (R * v1.x) - b * v2.x / v1.x;

    long double t3 = atan2(b, a) * 180.00 / M_PI;

    rhs = (P2.y - C.y) / R - (P2.x - C.x) * v1.y / (R * v1.x);

    lhs = v2.y - v1.y * v2.x / v1.x;

    b = rhs / lhs;

    a = (P2.x - C.x) / (R * v1.x) - b * v2.x / v1.x;

    long double t2 = atan2(b, a) * 180.00 / M_PI;

    Vector CP1(C, P1), CP2(C, P2);
    double Ctheta1 = (CP1 * CP2) / (CP1.norm() * CP2.norm());
    double theta1 = acos(Ctheta1) * 180 / M_PI;

    // double omega1 = theta1 / dt1;

    Vector CP3(C, P3);
    double Ctheta2 = (CP2 * CP3) / (CP2.norm() * CP3.norm());
    double theta2 = acos(Ctheta2) * 180 / M_PI;

    std::cout << theta2 << " .... " << t2 - t3 << std::endl; 

    // double omega2 = theta2 / dt2;

    // omega = (dt1 * omega1 + dt2 * omega2) / (dt1 + dt2)

    long double omega = 360.00 / (24.00 * 3600.00);

    int index = 1;

    while (true)
    {
        long double dt = index * 3600, t = t3;
        index++;

        t = t + dt * omega;

        t = t * M_PI / 180.0;

        Vector r_t(C.x + R * std::cos(t) * v1.x + R * std::sin(t) * v2.x,
               C.y + R * std::cos(t) * v1.y + R * std::sin(t) * v2.y,
               C.z + R * std::cos(t) * v1.z + R * std::sin(t) * v2.z
        );
        
        Point Pr(r_t.x, r_t.y, r_t.z);

        Sph Pr_s = cart_to_sph(Pr);

        Angle theta = convert(corectie_inv(Pr_s.m_theta)), 
            psi = convert(Pr_s.get_psi());

        print_angle(theta);
        print_angle(psi);   

        system("pause");
    }
    return 0;
}