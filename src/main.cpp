#include "math_model.hpp"
#include <Arduino.h>
#include <SoftwareSerial.h>

#define __BAUD__ 9600

using namespace math;


const float factor_de_corectie = 0.01f;

// corectie ?????
void corectie_ref_atm(Sph &s)
{
    s.theta = s.theta - (1 / tan((s.theta + 7.31 / (s.theta + 4.4)) * M_PI / 180)) / 60;
}

float corectie_ref_atm(double theta)
{
    return theta - (1 / tan((theta + 7.31 / (theta + 4.4)) * M_PI / 180)) / 60;
}

float corectie_inv(double theta)
{
    float h_ap = theta;

    while (corectie_ref_atm(h_ap) < theta)
        h_ap = h_ap + factor_de_corectie;

    return h_ap;
}

Point O{0.0f, 0.0f, 0.0f}, 
      C{0.0f, 0.0f, 0.0f};

Vector V1, V2, N;

float R, t3;
const float omega = 360.00 / (24.00 * 3600.00);

unsigned long start_millis = 0,
              last_print_millis = 0;

const unsigned long update_ms = 1000; // 1s

void model_init(void)
{
    Sph S1(revert({14, 57, 23}), revert({48, 11, 56})),  // se primeste dt1 si dt2
        S2(revert({18, 53, 19}), revert({52, 43, 36})), 
        S3(revert({23, 3, 57}),  revert({57, 7, 2}));

    corectie_ref_atm(S1);
    corectie_ref_atm(S2);
    corectie_ref_atm(S3);

    Point P1 = sph_to_cart(S1), P2 = sph_to_cart(S2), P3 = sph_to_cart(S3);

    V1 = Vector(P1, P2);
    V2 = Vector(P1, P3);

    orthonormal(V1, V2);

    N = cross(V2, V1);
    N.normalize();

    Vector OP1(O, P1),
           OP2(O, P2),
           OP3(O, P3);

    float k1 = (N * OP1) / (N * N),
          k2 = (N * OP2) / (N * N),
          k3 = (N * OP3) / (N * N); 

    float k = (k1 + k2 + k3) / 3.0f;

    C = Point{k * N.x, k * N.y, k * N.z};

    R = sqrt(1 - (C.x * C.x + C.y * C.y + C.z * C.z));

    float rhs = (P3.y - C.y) / R - (P3.x - C.x) * V1.y / (R * V1.x),
          lhs = V2.y - V1.y * V2.x / V1.x;

    float b = rhs / lhs,
          a = (P3.x - C.x) / (R * V1.x) - b * V2.x / V1.x;

    t3 = atan2(b, a) * 180.00 / M_PI;

    rhs = (P2.y - C.y) / R - (P2.x - C.x) * V1.y / (R * V1.x);
    lhs = V2.y - V1.y * V2.x / V1.x;

    b = rhs / lhs;
    a = (P2.x - C.x) / (R * V1.x) - b * V2.x / V1.x;

    float t2 = atan2(b, a) * 180.00 / M_PI;

    Vector CP1(C, P1), CP2(C, P2);
    double Ctheta1 = (CP1 * CP2) / (CP1.norm() * CP2.norm());
    double theta1 = acos(Ctheta1) * 180 / M_PI;

    // double omega1 = theta1 / dt1;

    Vector CP3(C, P3);
    double Ctheta2 = (CP2 * CP3) / (CP2.norm() * CP3.norm());
    double theta2 = acos(Ctheta2) * 180 / M_PI;

    // std::cout << theta2 << " .... " << t2 - t3 << std::endl; 

    // double omega2 = theta2 / dt2;

    // omega = (dt1 * omega1 + dt2 * omega2) / (dt1 + dt2)
}

void update_tracking(void)
{
    unsigned long now = millis();
    float dt = (now - start_millis) / 1000.0f; // sec. reale de la pornire

    float t = t3 + dt * omega;
    t = t * M_PI / 180.0f;

    Vector r_t(C.x + R * cos(t) * V1.x + R * sin(t) * V2.x,
               C.y + R * cos(t) * V1.y + R * sin(t) * V2.y,
               C.z + R * cos(t) * V1.z + R * sin(t) * V2.z
        );

    Point Pr{r_t.x, r_t.y, r_t.z};
    Sph Pr_s = cart_to_sph(Pr);

    Angle theta = convert(corectie_inv(Pr_s.theta)), 
            psi = convert(Pr_s.psi);
}

SoftwareSerial hc06(2, 3);
String rez = "";
int pos = 0;

void setup()
{
    pinMode(7, OUTPUT);
    Serial.begin(__BAUD__); // de schimbat

    while (!Serial) { }

    hc06.begin(__BAUD__);
    


    model_init();
    start_millis = millis();
    last_print_millis = 0;
}

void loop()
{
    while (hc06.available() > 0)
        rez += (char)hc06.read();

    if (rez == "ok")
    {
        
    }

    unsigned long now = millis();

    if (now - last_print_millis >= update_ms) // verif daca a trecut 1s
    {
        last_print_millis = now;
        update_tracking();
    }

    delay(100);
}