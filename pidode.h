#ifndef PIDODE_H
#define PIDODE_H

#include <vector>

#include "odestate.h"

enum targetType { STEP, SIGMOID, SQUAREWAVE };

class PIDEquation {
  public:
    PIDEquation(double Kp, double Ki, double Kd, double mass, double mu, targetType targ);
    void operator()(const ODEState &x, ODEState &dxdt, const double t);

  private:
    double Kp, Ki, Kd, mass, mu;
    targetType targ;
};

struct state_collect {
    std::vector<ODEState> &m_states;
    std::vector<double> &m_times;

    state_collect(std::vector<ODEState> &states, std::vector<double> &times);

    void reset();
    void operator()(const ODEState &x, double t);
};

double step(const double t);
double sigmoid(const double t);
double squarewave(const double t);
#endif // PIDODE_H
