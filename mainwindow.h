#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qdoubleclipvalidator.h"

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

  private:
    Ui::MainWindow *ui;

    QValidator *checkPIDs;
    QValidator *checkdt;

    bool changingText = false;

    constexpr static double Kp_slider_multiplier = 2;
    constexpr static double Ki_slider_multiplier = 0.5;
    constexpr static double Kd_slider_multiplier = 1;
    constexpr static double Kp_max = 2;
    constexpr static double Ki_max = 0.5;
    constexpr static double Kd_max = 1;
};

#endif // MAINWINDOW_H
