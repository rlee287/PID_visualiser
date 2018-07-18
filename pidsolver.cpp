#include "pidsolver.h"

using namespace boost::numeric::odeint;

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
            ODEState initial;
            PIDEquation pideq(Kp, Ki, Kd, mass, mu, targetType::STEP);
            typedef runge_kutta_dopri5<ODEState, double, ODEState, double, vector_space_algebra>
                stepper;
            size_t steps = integrate<double, PIDEquation, ODEState, double, state_collect>(
                pideq, initial, 0.0, 10.0, 0.1, state_collect(statevec, timesteps));
            // PID calculate here
            emit done();
            calculate = false;
        }
        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }
    }
}

void PIDSolver::update(double kp, double ki, double kd, double m, double Mu, bool wait) {
    Kp = kp;
    Ki = ki;
    Kd = kd;
    mass = m;
    mu = Mu;
    calculate = true;
    if (wait) {
        while (calculate) {
            // Wait for calculate to become false again
        }
    }
}
