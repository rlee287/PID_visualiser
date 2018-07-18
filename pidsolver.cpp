#include "pidsolver.h"

using namespace boost::numeric::odeint;

#include <stdio.h>
PIDSolver::PIDSolver(QObject *parent) : QThread(parent) {
    calculate = false;
}

PIDSolver::~PIDSolver() {
    quit();
    requestInterruption();
    wait();
}
void PIDSolver::run() {
    while (true) {
        if (calculate) {
            ODEState initial(3);
            initial[0] = 0;
            initial[1] = 0;
            initial[2] = 0;
            PIDEquation pideq(Kp, Ki, Kd, mass, mu, targetType::STEP);
            statevec.clear();
            timesteps.clear();
            integrate(pideq, initial, 0.0, 15.0, dt, state_collect(statevec, timesteps));
            emit done();
            calculate = false;
        } else {
            msleep(50);
        }
        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }
    }
}

void PIDSolver::update(double kp, double ki, double kd, double m, double Mu, double dt, bool wait) {
    while (calculate) {
        // Wait for calculate to become false again
    }
    Kp = kp;
    Ki = ki;
    Kd = kd;
    mass = m;
    mu = Mu;
    dt = dt;
    calculate = true;
    if (wait) {
        while (calculate) {
            // Wait for calculate to become false again
        }
    }
}

std::pair<std::vector<double>, std::vector<ODEState>> PIDSolver::getResults() {
    while (calculate) {
        // Wait
    }
    return std::make_pair(timesteps, statevec);
}
