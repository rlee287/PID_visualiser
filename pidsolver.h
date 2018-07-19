#ifndef PIDSOLVER_H
#define PIDSOLVER_H

#include <QThread>
#include <atomic>
#include <utility>
#include <vector>

#include <boost/numeric/odeint.hpp>

#include "pidode.h"

class PIDSolver : public QThread {
    Q_OBJECT
  public:
    explicit PIDSolver(QObject *parent = 0);
    ~PIDSolver();
    void run() override;
    void update(double kp, double ki, double kd, double m, double Mu, double Dt, setptType setpoint,
                bool wait);
    std::pair<std::vector<double>, std::vector<ODEState>> getResults();

  signals:
    void done();

  private:
    double Kp, Ki, Kd, mass, mu, dt;
    setptType typ;
    std::atomic_bool calculate;
    std::vector<double> timesteps;
    std::vector<ODEState> statevec;
};

#endif // PIDSOLVER_H
