#ifndef PIDSOLVER_H
#define PIDSOLVER_H

#include <QThread>
#include <atomic>
#include <vector>

#include <boost/numeric/odeint.hpp>

#include "odestate.h"
#include "pidode.h"

class PIDSolver : public QThread {
    Q_OBJECT
  public:
    explicit PIDSolver(QObject *parent = 0);
    ~PIDSolver();
    void run() override;
    void update(double kp, double ki, double kd, double m, double Mu, double dt, bool wait);
    bool isCalculating();

  signals:
    void done();

  private:
    double Kp, Ki, Kd, mass, mu, dt;
    std::atomic_bool calculate;
    std::vector<double> timesteps;
    std::vector<ODEState> statevec;
};

#endif // PIDSOLVER_H
