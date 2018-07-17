#include "pidode.h"

#include <cmath>

ODEState::ODEState() {}

double &ODEState ::operator[](std::size_t idx) {
    switch (idx) {
    case 0:
        return response;
        break;
    case 1:
        return deriv;
        break;
    case 2:
        return integratederror;
        break;
    default:
        std::cerr << "Error: attempted to access ODEState element " << idx << std::endl;
        return workaround_temp_refs;
        break;
    }
}

const double &ODEState ::operator[](std::size_t idx) const {
    switch (idx) {
    case 0:
        return response;
        break;
    case 1:
        return deriv;
        break;
    case 2:
        return integratederror;
        break;
    default:
        std::cerr << "Error: attempted to access ODEState element " << idx << std::endl;
        return workaround_temp_refs;
        break;
    }
}

PIDEquation::PIDEquation(double Kp, double Ki, double Kd, double mass, double mu, targetType targ)
    : Kp(Kp), Ki(Ki), Kd(Kd), mass(mass), mu(mu), targ(targ) {}

static double step(const double t) {
    return 1;
}

static double sigmoid(const double t) {
    double result = 1;
    result += exp(-2 * t + 4);
    result = 1 / result;
    result -= 1 / (exp(4) + 1);
    return result;
}

static double squarewave(const double t) {
    if (t > 2 && t <= 4) {
        return 1;
    } else if (t > 4 && t <= 6) {
        return -1;
    } else if (t > 6 && t <= 8) {
        return 1;
    } else {
        return 0;
    }
}
void PIDEquation::operator()(const ODEState &x, ODEState &dxdt, const double t) {
    double SP;
    switch (targ) {
    case targetType::STEP:
        SP = step(t);
        break;
    case targetType::SIGMOID:
        SP = sigmoid(t);
        break;
    case targetType::SQUAREWAVE:
        SP = squarewave(t);
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
