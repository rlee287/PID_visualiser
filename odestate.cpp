#include "odestate.h"

// Portions adapted from the Point3D boost example

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

ODEState &ODEState::operator+=(const ODEState &p) {
    response += p.response;
    deriv += p.deriv;
    integratederror += p.integratederror;
    return *this;
}
ODEState &ODEState::operator+=(const double a) {
    response += a;
    deriv += a;
    integratederror += a;
    return *this;
}
ODEState &ODEState::operator*=(const double a) {
    response *= a;
    deriv *= a;
    integratederror *= a;
    return *this;
}
ODEState operator/(const ODEState &p1, const ODEState &p2) {
    ODEState result;
    result.response = p1.response / p2.response;
    result.deriv = p1.deriv / p2.deriv;
    result.integratederror = p1.integratederror / p2.integratederror;
    return result;
}
ODEState abs(const ODEState &p) {
    ODEState result;
    result.response = abs(p.response);
    result.deriv = abs(p.deriv);
    result.integratederror = abs(p.integratederror);
    return result;
}
