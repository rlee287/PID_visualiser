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
            initial[1] = 1;
            initial[2] = 2;
            PIDEquation pideq(Kp, Ki, Kd, mass, mu, targetType::STEP);
            /*size_t steps = integrate<double, PIDEquation, ODEState, double, state_collect>(
                pideq, initial, 0.0, 10.0, 0.1, state_collect(statevec, timesteps));*/
            integrate(pideq, initial, 0.0, 10.0, dt, state_collect(statevec, timesteps));
            printf("p %f i %f d %f mass %f mu %f > final %f\n", Kp, Ki, Kd, mass, mu, initial[0]);
            std::cout.flush();
            // PID calculate here
            emit done();
            calculate = false;
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
