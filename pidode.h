#ifndef PIDODE_H
#define PIDODE_H

#include <cmath>
#include <vector>

typedef std::vector<double> ODEState;

enum setptType { STEP, SIGMOID, RAMP, SQUARESTEP };

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

inline double step(const double t) {
    return 1;
}

inline double ramp(const double t) {
    if (t < 2) {
        return t / 2;
    } else {
        return 1;
    }
}

inline double sigmoid(const double t) {
    double result = 1;
    result += exp(-2 * t + 4);
    result = 1 / result;
    result -= 1 / (exp(4) + 1);
    return result;
}

inline double squarestep(const double t) {
    if (t > 1 && t <= 8) {
        return 1;
    } else {
        return 0;
    }
}

inline double clamp(double in, double lower, double upper) {
    return std::max(lower, std::min(in, upper));
}
#endif // PIDODE_H
