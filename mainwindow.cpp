#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // MainWindow takes ownership using QPointer
    checkPIDs = new QDoubleClipValidator(0, 2, 10, this);
    checkdt = new QDoubleClipValidator(0.001, 0.1, 10, this);

    ui->setupUi(this);

    ui->unit_select->addItem("1:1");

    ui->target_select->addItem("step");
    ui->target_select->addItem("sigmoid");
    ui->target_select->addItem("squarecycle");

    ui->dt_enter->setText("0.02");
    updateLineEdits();

    ui->dt_enter->setValidator(checkdt);
    ui->Kp_text->setValidator(checkPIDs);
    ui->Ki_text->setValidator(checkPIDs);
    ui->Kd_text->setValidator(checkPIDs);

    connect(ui->Kp_slider, &QSlider::valueChanged, this, &MainWindow::updateLineEdits);
    connect(ui->Ki_slider, &QSlider::valueChanged, this, &MainWindow::updateLineEdits);
    connect(ui->Kd_slider, &QSlider::valueChanged, this, &MainWindow::updateLineEdits);
    connect(ui->Kp_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->Ki_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->Kd_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
}

void MainWindow::updateLineEdits() {
    if (!changingText) {
        double Kp_val = (double)ui->Kp_slider->value() / (ui->Kp_slider->maximum() + 1);
        double Ki_val = (double)ui->Ki_slider->value() / (ui->Ki_slider->maximum() + 1);
        double Kd_val = (double)ui->Kd_slider->value() / (ui->Kd_slider->maximum() + 1);

        Kp_val *= MainWindow::Kp_slider_multiplier;
        Ki_val *= MainWindow::Ki_slider_multiplier;
        Kd_val *= MainWindow::Kd_slider_multiplier;

        ui->Kp_text->setText(QString(std::to_string(Kp_val).c_str()));
        ui->Ki_text->setText(QString(std::to_string(Ki_val).c_str()));
        ui->Kd_text->setText(QString(std::to_string(Kd_val).c_str()));
    }
}
void MainWindow::updateSliders() {
    double Kp_val = ui->Kp_text->text().toDouble();
    double Ki_val = ui->Ki_text->text().toDouble();
    double Kd_val = ui->Kd_text->text().toDouble();

    Kp_val /= MainWindow::Kp_slider_multiplier;
    Ki_val /= MainWindow::Ki_slider_multiplier;
    Kd_val /= MainWindow::Kd_slider_multiplier;

    Kp_val *= (ui->Kp_slider->maximum() + 1);
    Ki_val *= (ui->Ki_slider->maximum() + 1);
    Kd_val *= (ui->Kd_slider->maximum() + 1);

    changingText = true;
    ui->Kp_slider->setValue((int)Kp_val);
    ui->Ki_slider->setValue((int)Ki_val);
    ui->Kd_slider->setValue((int)Kd_val);
    changingText = false;
}

MainWindow::~MainWindow() {
    delete ui;
}
