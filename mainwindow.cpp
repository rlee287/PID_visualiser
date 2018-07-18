#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // MainWindow takes ownership using QPointer
    // Keep separate in case this is changed later
    checkPIDs = new QDoubleValidator(this);
    checkdt = new QDoubleValidator(this);
    solverThread = new PIDSolver(this);

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
    connect(ui->mass_slider, &QSlider::valueChanged, this, &MainWindow::updateLineEdits);
    connect(ui->mu_slider, &QSlider::valueChanged, this, &MainWindow::updateLineEdits);
    connect(ui->Kp_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->Ki_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->Kd_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->mass_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    connect(ui->mu_text, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    solverThread->start();
}

static float clamp(float in, float lower, float upper) {
    return std::max(lower, std::min(in, upper));
}

void MainWindow::updateLineEdits() {
    if (!changingText) {
        double Kp_val = (double)ui->Kp_slider->value() / (ui->Kp_slider->maximum() + 1);
        double Ki_val = (double)ui->Ki_slider->value() / (ui->Ki_slider->maximum() + 1);
        double Kd_val = (double)ui->Kd_slider->value() / (ui->Kd_slider->maximum() + 1);
        double mass_val = (double)ui->mass_slider->value() / (ui->mass_slider->maximum() + 1);
        double mu_val = (double)ui->mu_slider->value() / (ui->mu_slider->maximum() + 1);

        Kp_val *= MainWindow::Kp_slider_multiplier;
        Ki_val *= MainWindow::Ki_slider_multiplier;
        Kd_val *= MainWindow::Kd_slider_multiplier;
        mass_val *= MainWindow::mass_slider_multiplier;
        mu_val *= MainWindow::mu_slider_multiplier;

        ui->Kp_text->setText(QString::number(Kp_val));
        ui->Ki_text->setText(QString::number(Ki_val));
        ui->Kd_text->setText(QString::number(Kd_val));
        ui->mass_text->setText(QString::number(mass_val));
        ui->mu_text->setText(QString::number(mu_val));
        solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, false);
    }
}

void MainWindow::updateSliders() {
    // Ignore the boxes with invalid inputs
    bool Kp_OK, Ki_OK, Kd_OK, mass_OK, mu_OK;
    double Kp_val = ui->Kp_text->text().toDouble(&Kp_OK);
    double Ki_val = ui->Ki_text->text().toDouble(&Ki_OK);
    double Kd_val = ui->Kd_text->text().toDouble(&Kd_OK);
    double mass_val = ui->mass_text->text().toDouble(&mass_OK);
    double mu_val = ui->mu_text->text().toDouble(&mu_OK);

    if (Kp_OK) {
        double Kp_sl = clamp(Kp_val, 0, MainWindow::Kp_max);
        ui->Kp_text->setText(QString::number(Kp_sl));
        Kp_sl /= MainWindow::Kp_slider_multiplier;
        Kp_sl *= (ui->Kp_slider->maximum() + 1);
        changingText = true;
        ui->Kp_slider->setValue((int)Kp_sl);
        changingText = false;
    }
    if (Ki_OK) {
        double Ki_sl = clamp(Ki_val, 0, MainWindow::Ki_max);
        ui->Ki_text->setText(QString::number(Ki_sl));
        Ki_sl /= MainWindow::Ki_slider_multiplier;
        Ki_sl *= (ui->Ki_slider->maximum() + 1);

        changingText = true;
        ui->Ki_slider->setValue((int)Ki_sl);
        changingText = false;
    }
    if (Kd_OK) {
        double Kd_sl = clamp(Kd_val, 0, MainWindow::Kd_max);
        ui->Kd_text->setText(QString::number(Kd_sl));
        Kd_sl /= MainWindow::Kd_slider_multiplier;
        Kd_sl *= (ui->Kd_slider->maximum() + 1);

        changingText = true;
        ui->Kd_slider->setValue((int)Kd_sl);
        changingText = false;
    }
    if (mass_OK) {
        double mass_sl = clamp(mass_val, 0, MainWindow::mass_max);
        ui->mass_text->setText(QString::number(mass_sl));
        mass_sl /= MainWindow::mass_slider_multiplier;
        mass_sl *= (ui->mass_slider->maximum() + 1);

        changingText = true;
        ui->mass_slider->setValue((int)mass_sl);
        changingText = false;
    }
    if (mu_OK) {
        double mu_sl = clamp(mu_val, 0, MainWindow::mu_max);
        ui->mu_text->setText(QString::number(mu_sl));
        mu_sl /= MainWindow::mu_slider_multiplier;
        mu_sl *= (ui->mu_slider->maximum() + 1);

        changingText = true;
        ui->mu_slider->setValue((int)mu_sl);
        changingText = false;
    }
    solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, false);
}

MainWindow::~MainWindow() {
    delete ui;
}
