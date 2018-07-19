#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QScrollBar>
#include <QValueAxis>

#include <boost/range/combine.hpp>
#include <iostream>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // MainWindow takes ownership using QPointer
    // Keep separate in case this is changed later
    checkPIDs = new QDoubleValidator(this);
    checkdt = new QDoubleValidator(this);
    solverThread = new PIDSolver(this);

    ui->setupUi(this);

    pidChart = new QChart();
    pidChart->setTitle("PID Simulation");
    QValueAxis *theX = new QValueAxis(pidChart);
    theX->setRange(0, 15);
    theX->setTickCount(6);
    QValueAxis *theY = new QValueAxis(pidChart);
    theY->setRange(-0.5, 1.5);
    theY->setTickCount(5);
    pidChart->setAxisX(theX);
    pidChart->setAxisY(theY);

    outScene = new QGraphicsScene(ui->out_graph);
    ui->out_graph->setScene(outScene);
    outScene->addItem(pidChart);
    ui->out_graph->setRenderHint(QPainter::Antialiasing);

    ui->unit_select->addItem("1:1");

    ui->target_select->addItem("step", targetType::STEP);
    ui->target_select->addItem("sigmoid", targetType::SIGMOID);
    ui->target_select->addItem("squarecycle", targetType::SQUAREWAVE);

    ui->dt_enter->setText("0.02");

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

    // Update Sliders is nondestructive
    connect(ui->dt_enter, &QLineEdit::textChanged, this, &MainWindow::updateSliders);
    // connect(ui->target_select, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSliders()));
    connect(ui->target_select,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::updateSliders);

    connect(solverThread, &PIDSolver::done, this, &MainWindow::updateGraph);

    solverThread->start();
    // usleep(20000);
    updateLineEdits();
}

static float clamp(float in, float lower, float upper) {
    return std::max(lower, std::min(in, upper));
}

void MainWindow::updateLineEdits() {
    if (!changingText) {
        changingSlider = true;
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

        double dt = ui->dt_enter->text().toDouble();
        dt = clamp(dt, MainWindow::dt_min, MainWindow::dt_max);
        ui->dt_enter->setText(QString::number(dt));

        targetType setpt;
        switch (ui->target_select->currentIndex()) {
        case 0:
            setpt = targetType::STEP;
            break;
        case 1:
            setpt = targetType::SIGMOID;
            break;
        case 2:
            setpt = targetType::SQUAREWAVE;
            break;
        default:
            setpt = targetType::STEP;
            break;
        }

        solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, dt, setpt, false);
        changingSlider = false;
    }
}

void MainWindow::updateSliders() {
    if (!changingSlider) {
        // Ignore the boxes with invalid inputs
        bool Kp_OK, Ki_OK, Kd_OK, mass_OK, mu_OK;
        double Kp_val = ui->Kp_text->text().toDouble(&Kp_OK);
        double Ki_val = ui->Ki_text->text().toDouble(&Ki_OK);
        double Kd_val = ui->Kd_text->text().toDouble(&Kd_OK);
        double mass_val = ui->mass_text->text().toDouble(&mass_OK);
        double mu_val = ui->mu_text->text().toDouble(&mu_OK);

        changingText = true;
        if (Kp_OK) {
            double Kp_sl = clamp(Kp_val, 0, MainWindow::Kp_max);
            ui->Kp_text->setText(QString::number(Kp_sl));
            Kp_sl /= MainWindow::Kp_slider_multiplier;
            Kp_sl *= (ui->Kp_slider->maximum() + 1);
            ui->Kp_slider->setValue((int)Kp_sl);
        }
        if (Ki_OK) {
            double Ki_sl = clamp(Ki_val, 0, MainWindow::Ki_max);
            ui->Ki_text->setText(QString::number(Ki_sl));
            Ki_sl /= MainWindow::Ki_slider_multiplier;
            Ki_sl *= (ui->Ki_slider->maximum() + 1);

            ui->Ki_slider->setValue((int)Ki_sl);
        }
        if (Kd_OK) {
            double Kd_sl = clamp(Kd_val, 0, MainWindow::Kd_max);
            ui->Kd_text->setText(QString::number(Kd_sl));
            Kd_sl /= MainWindow::Kd_slider_multiplier;
            Kd_sl *= (ui->Kd_slider->maximum() + 1);

            ui->Kd_slider->setValue((int)Kd_sl);
        }
        if (mass_OK) {
            double mass_sl = clamp(mass_val, 0, MainWindow::mass_max);
            ui->mass_text->setText(QString::number(mass_sl));
            mass_sl /= MainWindow::mass_slider_multiplier;
            mass_sl *= (ui->mass_slider->maximum() + 1);

            ui->mass_slider->setValue((int)mass_sl);
        }
        if (mu_OK) {
            double mu_sl = clamp(mu_val, 0, MainWindow::mu_max);
            ui->mu_text->setText(QString::number(mu_sl));
            mu_sl /= MainWindow::mu_slider_multiplier;
            mu_sl *= (ui->mu_slider->maximum() + 1);

            ui->mu_slider->setValue((int)mu_sl);
        }

        double dt = ui->dt_enter->text().toDouble();
        dt = clamp(dt, MainWindow::dt_min, MainWindow::dt_max);
        ui->dt_enter->setText(QString::number(dt));

        targetType setpt;
        switch (ui->target_select->currentIndex()) {
        case 0:
            setpt = targetType::STEP;
            break;
        case 1:
            setpt = targetType::SIGMOID;
            break;
        case 2:
            setpt = targetType::SQUAREWAVE;
            break;
        default:
            setpt = targetType::STEP;
            break;
        }

        solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, dt, setpt, false);
        changingText = false;
    }
}

void MainWindow::updateGraph() {
    // PID output
    QLineSeries *series = new QLineSeries(pidChart);
    std::pair<std::vector<double>, std::vector<ODEState>> results = solverThread->getResults();
    size_t len = results.first.size();
    for (size_t i = 0; i < len; i++) {
        series->append(results.first[i], results.second[i][0]);
    }
    series->setName("PID Controller");
    pidChart->removeAllSeries();
    pidChart->addSeries(series);
    series->attachAxis(pidChart->axisX());
    series->attachAxis(pidChart->axisY());

    // Setpoint
    QLineSeries *targSeries = new QLineSeries(pidChart);
    for (size_t i = 0; i < len; i++) {
        int setpt_index = ui->target_select->currentIndex();
        double setp;
        switch (setpt_index) {
        case 0:
            setp = step(results.first[i]);
            break;
        case 1:
            setp = sigmoid(results.first[i]);
            break;
        case 2:
            setp = squarewave(results.first[i]);
            break;
        default:
            setp = 0;
            break;
        }
        targSeries->append(results.first[i], setp);
    }
    targSeries->setName("Setpoint");
    pidChart->addSeries(targSeries);
    targSeries->attachAxis(pidChart->axisX());
    targSeries->attachAxis(pidChart->axisY());

    // Trigger resize event to properly redraw the graph
    resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    pidChart->setMargins(QMargins(0, 0, 0, 0));
    pidChart->setPos(0, 0);
    QSize outSize = ui->out_graph->size();
    outSize -= QSize(20, 30);
    pidChart->resize(outSize);
    // ui->out_graph->size();
    outScene->setSceneRect(0, 0, outSize.width(), outSize.height());
}

MainWindow::~MainWindow() {
    delete ui;
    delete pidChart;
}
