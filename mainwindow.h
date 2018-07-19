#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pidsolver.h"
#include <QChart>
#include <QLineSeries>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  public slots:
    void updateSliders();
    void updateLineEdits();
    void updateGraph();

  private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent *event) override;

    PIDSolver *solverThread;
    QtCharts::QChart *pidChart;
    QtCharts::QChart *compChart;
    QGraphicsScene *outScene;
    QGraphicsScene *compScene;

    bool changingText = false;
    bool changingSlider = false;

    constexpr static double Kp_slider_multiplier = 2;
    constexpr static double Ki_slider_multiplier = 0.1;
    constexpr static double Kd_slider_multiplier = 1;
    constexpr static double mass_slider_multiplier = 1;
    constexpr static double mu_slider_multiplier = 1.5;
    constexpr static double Kp_max = 3;
    constexpr static double Ki_max = 0.2;
    constexpr static double Kd_max = 1.5;
    constexpr static double mass_max = 1.5;
    constexpr static double mu_max = 2;

    constexpr static double dt_min = 0.001;
    constexpr static double dt_max = 0.1;
};

#endif // MAINWINDOW_H
