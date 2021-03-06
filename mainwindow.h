#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "pidsolver.h"
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>

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

  private slots:
    void on_save_image_clicked();
    void on_save_data_clicked();
    void resizeEventSlot();

  private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent *event) override;

    PIDSolver *solverThread;
    QtCharts::QChart *pidChart;
    QtCharts::QChart *compChart;
    QtCharts::QValueAxis thePIDX;
    QtCharts::QValueAxis theCompX;
    QtCharts::QValueAxis thePIDY;
    QtCharts::QValueAxis theCompY;
    QGraphicsScene *outScene;
    QGraphicsScene *compScene;

    double *csvexportarray;
    size_t doubleArraySize;
    const size_t numSeries = 7; // update if series changes

    bool changingText = false;
    bool changingSlider = false;

    constexpr static double Kp_slider_multiplier = 2;
    constexpr static double Ki_slider_multiplier = 0.25;
    constexpr static double Kd_slider_multiplier = 1;
    constexpr static double mass_slider_multiplier = 1;
    constexpr static double mu_slider_multiplier = 1.5;
    constexpr static double Kp_max = 3;
    constexpr static double Ki_max = 0.5;
    constexpr static double Kd_max = 1.5;
    constexpr static double mass_min = 0.001;
    constexpr static double mass_max = 1.5;
    constexpr static double mu_max = 2;

    constexpr static double dt_min = 0.001;
    constexpr static double dt_max = 0.1;
};

#endif // MAINWINDOW_H
