#ifndef PIDSOLVER_H
#define PIDSOLVER_H

#include <QThread>
#include <atomic>
#include <vector>

#include <boost/numeric/odeint.hpp>

using namespace boost::numeric::odeint;

class PIDSolver : public QThread {
    Q_OBJECT
  public:
    explicit PIDSolver(QObject *parent = 0);
    ~PIDSolver();
    void run() override;
    void update(bool wait);
    bool isCalculating();

  signals:
    void done();

  private:
    std::atomic_bool calculate;
    std::vector<double> timesteps;
    std::vector<double[3]> state;
};

#endif // PIDSOLVER_H
