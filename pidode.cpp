#include "pidode.h"

#include <cmath>

PIDEquation::PIDEquation(double Kp, double Ki, double Kd, double mass, double mu, setptType targ)
    : Kp(Kp), Ki(Ki), Kd(Kd), mass(mass), mu(mu), targ(targ) {}

state_collect::state_collect(std::vector<ODEState> &states, std::vector<double> &times)
    : m_states(states), m_times(times) {}

void state_collect::operator()(const ODEState &x, double t) {
    m_states.push_back(x);
    m_times.push_back(t);
}

void state_collect::reset() {
    m_states.clear();
    m_times.clear();
}

double step(const double t) {
    return 1;
}

double sigmoid(const double t) {
    double result = 1;
    result += exp(-2 * t + 4);
    result = 1 / result;
    result -= 1 / (exp(4) + 1);
    return result;
}

double squarestep(const double t) {
    if (t > 1 && t <= 8) {
        return 1;
    } else {
        return 0;
    }
}

void PIDEquation::operator()(const ODEState &x, ODEState &dxdt, const double t) {
    double SP;
    switch (targ) {
    case setptType::STEP:
        SP = step(t);
        break;
    case setptType::SIGMOID:
        SP = sigmoid(t);
        break;
    case setptType::SQUARESTEP:
        SP = squarestep(t);
        break;
    default:
        SP = 0;
        break;
    }
    double error = SP - x[0];
    dxdt[0] = x[1];
    dxdt[2] = error;
    dxdt[1] = Kp * error + Ki * x[2] - (Kd + mu) * x[1];
    dxdt[1] /= mass;
}
