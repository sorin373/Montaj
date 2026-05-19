#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>

#include "math_model.hpp"

using namespace math;

std::ofstream fout("data.csv");

const double factor_de_corectie = 0.01;
const double omega = 360.0 / (24.0 * 3600.0); // deg/sec

Point O{0.0f, 0.0f, 0.0f};
Point C{0.0f, 0.0f, 0.0f};

Vector V1, V2, N;

double R = 0.0;
double t3 = 0.0;

float corectie_ref_atm(double theta)
{
    return theta - (1.0 / std::tan((theta + 7.31 / (theta + 4.4)) * M_PI / 180.0)) / 60.0;
}

float corectie_inv(double theta)
{
    float h_ap = theta;

    while (corectie_ref_atm(h_ap) < theta)
        h_ap += factor_de_corectie;

    return h_ap;
}

void corectie_ref_atm(Sph &s)
{
    s.theta = corectie_ref_atm(s.theta);
}

void model_init()
{
    Sph S1(revert({14, 57, 23}), revert({48, 11, 56}));
    Sph S2(revert({18, 53, 19}), revert({52, 43, 36}));
    Sph S3(revert({23, 3, 57}),  revert({57, 7, 2}));

    corectie_ref_atm(S1);
    corectie_ref_atm(S2);
    corectie_ref_atm(S3);

    Point P1 = sph_to_cart(S1);
    Point P2 = sph_to_cart(S2);
    Point P3 = sph_to_cart(S3);

    V1 = Vector(P1, P2);
    V2 = Vector(P1, P3);

    orthonormal(V1, V2);

    N = cross(V2, V1);
    N.normalize();

    Vector OP1(O, P1);
    Vector OP2(O, P2);
    Vector OP3(O, P3);

    double k1 = (N * OP1) / (N * N);
    double k2 = (N * OP2) / (N * N);
    double k3 = (N * OP3) / (N * N);

    double k = (k1 + k2 + k3) / 3.0;

    C = Point{
        static_cast<float>(k * N.x),
        static_cast<float>(k * N.y),
        static_cast<float>(k * N.z)
    };

    R = std::sqrt(1.0 - (C.x * C.x + C.y * C.y + C.z * C.z));

    Vector CP3(C, P3);

    double a = (CP3 * V1) / R;
    double b = (CP3 * V2) / R;

    t3 = std::atan2(b, a) * 180.0 / M_PI;
}

void generate_point(double time_s)
{
    double t_deg = t3 + time_s * omega;
    double t = t_deg * M_PI / 180.0;

    Vector r_t(
        C.x + R * std::cos(t) * V1.x + R * std::sin(t) * V2.x,
        C.y + R * std::cos(t) * V1.y + R * std::sin(t) * V2.y,
        C.z + R * std::cos(t) * V1.z + R * std::sin(t) * V2.z
    );

    Point Pr{r_t.x, r_t.y, r_t.z};
    Sph Pr_s = cart_to_sph(Pr);

    float theta = corectie_inv(Pr_s.theta);
    float psi = Pr_s.psi;

    fout << time_s << ","
        << Pr.x << ","
        << Pr.y << ","
        << Pr.z << ","
        << theta << ","
        << psi
        << "\n";
}

int main()
{
    model_init();

    fout << "time_s,x,y,z,theta_deg,psi_deg\n";

    const double duration_s = 24.0 * 3600.0;
    const double step_s = 60.0;

    for (double t = 0.0; t <= duration_s; t += step_s)
        generate_point(t);

    return 0;
}