#ifndef PIDODE_H
#define PIDODE_H

#include <vector>

typedef std::vector<double> ODEState;

enum setptType { STEP, SIGMOID, SQUARESTEP };

class PIDEquation {
  public:
    PIDEquation(double Kp, double Ki, double Kd, double mass, double mu, bool clip, setptType targ);
    void operator()(const ODEState &x, ODEState &dxdt, const double t);

  private:
    double Kp, Ki, Kd, mass, mu;
    bool clip;
    setptType targ;
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
double squarestep(const double t);

double clamp(double in, double lower, double upper);
#endif // PIDODE_H
