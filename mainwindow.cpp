#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // MainWindow takes ownership using QPointer
    // Keep separate in case this is changed later
    checkPIDs = new QDoubleValidator(this);
    checkdt = new QDoubleValidator(this);

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

static float clamp(float in, float lower, float upper) {
    return std::max(lower, std::min(in, upper));
}

void MainWindow::updateLineEdits() {
    if (!changingText) {
        double Kp_val = (double)ui->Kp_slider->value() / (ui->Kp_slider->maximum() + 1);
        double Ki_val = (double)ui->Ki_slider->value() / (ui->Ki_slider->maximum() + 1);
        double Kd_val = (double)ui->Kd_slider->value() / (ui->Kd_slider->maximum() + 1);

        Kp_val *= MainWindow::Kp_slider_multiplier;
        Ki_val *= MainWindow::Ki_slider_multiplier;
        Kd_val *= MainWindow::Kd_slider_multiplier;

        ui->Kp_text->setText(QString::number(Kp_val));
        ui->Ki_text->setText(QString::number(Ki_val));
        ui->Kd_text->setText(QString::number(Kd_val));
    }
}

void MainWindow::updateSliders() {
    // Ignore the boxes with invalid inputs
    bool Kp_OK, Ki_OK, Kd_OK;
    double Kp_val = ui->Kp_text->text().toDouble(&Kp_OK);
    double Ki_val = ui->Ki_text->text().toDouble(&Ki_OK);
    double Kd_val = ui->Kd_text->text().toDouble(&Kd_OK);

    if (Kp_OK) {
        Kp_val = clamp(Kp_val, 0, 2);
        ui->Kp_text->setText(QString::number(Kp_val));
        Kp_val /= MainWindow::Kp_slider_multiplier;
        Kp_val *= (ui->Kp_slider->maximum() + 1);
        changingText = true;
        ui->Kp_slider->setValue((int)Kp_val);
        changingText = false;
    }
    if (Ki_OK) {
        Ki_val = clamp(Ki_val, 0, 0.5);
        ui->Ki_text->setText(QString::number(Ki_val));
        Ki_val /= MainWindow::Ki_slider_multiplier;
        Ki_val *= (ui->Ki_slider->maximum() + 1);

        changingText = true;
        ui->Ki_slider->setValue((int)Ki_val);
        changingText = false;
    }
    if (Kd_OK) {
        Kd_val = clamp(Kd_val, 0, 1);
        ui->Kd_text->setText(QString::number(Kd_val));
        Kd_val /= MainWindow::Kd_slider_multiplier;
        Kd_val *= (ui->Kd_slider->maximum() + 1);

        changingText = true;
        ui->Kd_slider->setValue((int)Kd_val);
        changingText = false;
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
