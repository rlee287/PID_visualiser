#ifndef ODESTATE_H
#define ODESTATE_H

#include <iostream>

class ODEState {
  public:
    ODEState();
    double response = 0;
    double deriv = 0;
    double integratederror = 0;

    double &operator[](std::size_t idx);
    const double &operator[](std::size_t idx) const;

  private:
    double workaround_temp_refs = 0;
};

enum targetType { STEP, SIGMOID, SQUAREWAVE };

class PIDEquation {
  public:
    PIDEquation(double Kp, double Ki, double Kd, double mass, double mu, targetType targ);
    void operator()(const ODEState &x, ODEState &dxdt, const double t);

  private:
    double Kp, Ki, Kd, mass, mu;
    targetType targ;
};
#endif // ODESTATE_H
