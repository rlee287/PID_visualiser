#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>

#include <boost/range/combine.hpp>
#include <iostream>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // MainWindow takes ownership using QPointer
    solverThread = new PIDSolver(this);

    ui->setupUi(this);

    csvexportarray = new double[1];

    pidChart = new QChart();
    pidChart->setTitle("PID Simulation");
    // QValueAxis theX;
    thePIDX.setRange(0, 15);
    thePIDX.setTickCount(6);
    // QValueAxis theY;
    thePIDY.setRange(-0.5, 1.5);
    thePIDY.setTickCount(5);
    pidChart->addAxis(&thePIDX, Qt::AlignBottom);
    pidChart->addAxis(&thePIDY, Qt::AlignLeft);

    compChart = new QChart();
    compChart->setTitle("PID Components");
    theCompX.setRange(0, 15);
    theCompX.setTickCount(6);
    theCompY.setRange(-1, 1);
    theCompY.setTickCount(5);
    compChart->addAxis(&theCompX, Qt::AlignBottom);
    compChart->addAxis(&theCompY, Qt::AlignLeft);

    outScene = new QGraphicsScene(ui->out_graph);
    ui->out_graph->setScene(outScene);
    outScene->addItem(pidChart);
    ui->out_graph->setRenderHint(QPainter::Antialiasing);

    compScene = new QGraphicsScene(ui->comp_graph);
    ui->comp_graph->setScene(compScene);
    compScene->addItem(compChart);
    ui->comp_graph->setRenderHint(QPainter::Antialiasing);

    ui->unit_select->addItem("1:1");

    ui->target_select->addItem("step", setptType::STEP);
    ui->target_select->addItem("sigmoid", setptType::SIGMOID);
    ui->target_select->addItem("squarestep", setptType::SQUARESTEP);

    ui->dt_enter->setText(QString::number(0.02));

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
    connect(ui->dt_enter, &QLineEdit::editingFinished, this, &MainWindow::updateSliders);
    connect(ui->target_select,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::updateSliders);
    connect(ui->output_clip, &QCheckBox::toggled, this, &MainWindow::updateSliders);

    connect(solverThread, &PIDSolver::done, this, &MainWindow::updateGraph);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::resizeEventSlot);

    solverThread->start();
    updateLineEdits();
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

        setptType setpt;
        switch (ui->target_select->currentIndex()) {
        case 0:
            setpt = setptType::STEP;
            break;
        case 1:
            setpt = setptType::SIGMOID;
            break;
        case 2:
            setpt = setptType::SQUARESTEP;
            break;
        default:
            setpt = setptType::STEP;
            break;
        }

        solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, dt, setpt,
                             ui->output_clip->isChecked(), false);
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

        setptType setpt;
        switch (ui->target_select->currentIndex()) {
        case 0:
            setpt = setptType::STEP;
            break;
        case 1:
            setpt = setptType::SIGMOID;
            break;
        case 2:
            setpt = setptType::SQUARESTEP;
            break;
        default:
            setpt = setptType::STEP;
            break;
        }

        solverThread->update(Kp_val, Ki_val, Kd_val, mass_val, mu_val, dt, setpt,
                             ui->output_clip->isChecked(), false);
        changingText = false;
    }
}

void MainWindow::updateGraph() {
    // PID output
    QLineSeries *PIDSeries = new QLineSeries(pidChart);
    std::pair<std::vector<double>, std::vector<ODEState>> results = solverThread->getResults();
    size_t len = results.first.size();
    for (size_t i = 0; i < len; i++) {
        PIDSeries->append(results.first[i], results.second[i][0]);
    }
    PIDSeries->setName("PID Controller");
    pidChart->removeAllSeries();
    pidChart->addSeries(PIDSeries);
    PIDSeries->attachAxis(pidChart->axisX());
    PIDSeries->attachAxis(pidChart->axisY());

    // Setpoint
    QLineSeries *setptSeries = new QLineSeries(pidChart);
    QLineSeries *porp = new QLineSeries(pidChart);
    QLineSeries *integ = new QLineSeries(pidChart);
    QLineSeries *deriv = new QLineSeries(pidChart);
    QLineSeries *outpow = new QLineSeries(pidChart);
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
            setp = squarestep(results.first[i]);
            break;
        default:
            setp = 0;
            break;
        }
        setptSeries->append(results.first[i], setp);
        porp->append(results.first[i],
                     ui->Kp_text->text().toDouble() * (setp - results.second[i][0]));
        integ->append(results.first[i], ui->Ki_text->text().toDouble() * results.second[i][2]);
        deriv->append(results.first[i], ui->Kd_text->text().toDouble() * results.second[i][1]);
        double out = (ui->Kp_text->text().toDouble() * (setp - results.second[i][0])) +
                     (ui->Ki_text->text().toDouble() * results.second[i][2]) +
                     (ui->Kd_text->text().toDouble() * results.second[i][1]);
        if (ui->output_clip->isChecked()) {
            out = clamp(out, -1, 1);
        }
        outpow->append(results.first[i], out);
    }
    setptSeries->setName("Setpoint");
    pidChart->addSeries(setptSeries);
    setptSeries->attachAxis(pidChart->axisX());
    setptSeries->attachAxis(pidChart->axisY());

    porp->setName("Proportional term");
    integ->setName("Integral term");
    deriv->setName("Derivative term");
    outpow->setName("Total output power");

    compChart->removeAllSeries();
    compChart->addSeries(porp);
    compChart->addSeries(integ);
    compChart->addSeries(deriv);
    compChart->addSeries(outpow);
    porp->attachAxis(compChart->axisX());
    porp->attachAxis(compChart->axisY());
    integ->attachAxis(compChart->axisX());
    integ->attachAxis(compChart->axisY());
    deriv->attachAxis(compChart->axisX());
    deriv->attachAxis(compChart->axisY());
    outpow->attachAxis(compChart->axisX());
    outpow->attachAxis(compChart->axisY());

    // Update csvexportarray
    doubleArraySize = len * numSeries;
    delete[] csvexportarray;
    csvexportarray = new double[doubleArraySize];
    std::cout << doubleArraySize << std::endl;
    for (size_t i = 0; i < len; i++) {
        std::cout << i << std::endl;
        csvexportarray[i * numSeries] = results.first[i];
    }
    size_t i = 1;
    QList<QPointF> pts = PIDSeries->points();
    size_t j = 0;
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }
    i++;
    j = 0;
    pts = setptSeries->points();
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }
    i++;
    j = 0;
    pts = porp->points();
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }
    i++;
    j = 0;
    pts = integ->points();
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }
    i++;
    j = 0;
    pts = deriv->points();
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }
    i++;
    j = 0;
    pts = outpow->points();
    for (auto it = pts.cbegin(); it != pts.cend(); it++) {
        size_t index = i + j * numSeries;
        csvexportarray[index] = (*it).y();
        j++;
    }

    // Trigger resize event to properly redraw the graph
    resizeEvent(nullptr);
}

void MainWindow::resizeEventSlot() {
    resizeEvent(nullptr);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    // PID Chart
    pidChart->setMargins(QMargins(0, 0, 0, 0));
    pidChart->setPos(0, 0);
    QSize outSize = ui->out_graph->size();
    // Shrink because this size is slightly too big
    outSize -= QSize(20, 30);
    pidChart->resize(outSize);
    // Set manually in order to force shrinkage when necessary
    outScene->setSceneRect(0, 0, outSize.width(), outSize.height());

    // Component chart
    compChart->setMargins(QMargins(0, 0, 0, 0));
    compChart->setPos(0, 0);
    outSize = ui->comp_graph->size();
    // Shrink because this size is slightly too big
    outSize -= QSize(20, 30);
    compChart->resize(outSize);
    // Set manually in order to force shrinkage when necessary
    compScene->setSceneRect(0, 0, outSize.width(), outSize.height());
}

MainWindow::~MainWindow() {
    delete ui;
    delete pidChart;
    delete[] csvexportarray;
}

void MainWindow::on_save_image_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save graph image", ".", "PNG (*.png)");
    if (!fileName.isNull()) {
        resizeEvent(nullptr);
        ui->out_graph->repaint();
        QPixmap imagePixels = ui->out_graph->grab();
        imagePixels.save(fileName);
    }
}

void MainWindow::on_save_data_clicked() {
    QString fileName =
        QFileDialog::getSaveFileName(this, "Save simulation data", ".", "CSV (*.csv)");
    if (!fileName.isNull()) {
        QFile dataFile(fileName);
        if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }
        /*QList<QAbstractSeries *> pidQSer = pidChart->series();
        QList<QAbstractSeries *> compQSer = compChart->series();
        std::list<QLineSeries *> serList;
        for (auto it = pidQSer.cbegin(); it != pidQSer.cend(); ++it) {
            serList.push_back(static_cast<QLineSeries *>(*it));
        }
        for (auto it = compQSer.cbegin(); it != compQSer.cend(); ++it) {
            serList.push_back(static_cast<QLineSeries *>(*it));
        }
        // They should all have the same size and same x coordinates
        size_t num_points = serList.back()->points().size();
        size_t numSeries = serList.size();
        size_t doubleArraySize = num_points * (numSeries + 1);
        double *pts = new double[doubleArraySize];
        std::cout << doubleArraySize;
        size_t serIndex = 0;
        for (auto it = serList.cbegin(); it != serList.cend(); ++it) {
            size_t pointIndex = 0;
            QList<QPointF> ptlist = (*it)->points();
            for (auto itpt = ptlist.cbegin(); itpt != ptlist.cend(); ++itpt) {
                size_t index = serIndex + numSeries * pointIndex;
                if (pointIndex == 0) {
                    // pts[index] = (*itpt).x();
                    pointIndex++;
                    index += numSeries;
                    // std::cout << index << std::endl;
                }
                // pts[index] = (*itpt).y();
                // std::cout << index << std::endl;
                pointIndex++;
            }
            serIndex++;
        }*/
        dataFile.write(
            "Time,PIDController,Setpoint,Proportional,Integral,Derivative,OutputPower\n");
        for (size_t i = 0; i < doubleArraySize; i++) {
            std::cout << csvexportarray[i] << std::endl;
            dataFile.write(QString::number(csvexportarray[i]).toLatin1());
            if (i > 0 && i % numSeries == 0) {
                dataFile.write("\n");
            } else {
                dataFile.write(",");
            }
        }
        std::cout.flush();
        dataFile.flush();
        // delete[] pts;
        dataFile.close();
    }
    /*if (it != serList.cend() && it == std::prev(serList.cend())) {
        // no comma
    }*/
}
