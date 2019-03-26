#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "audioinput.h"
#include "levelmeter.h"
#include "audioengine.h"
#include "frequencyspectrum.h"

#include <QtCharts/QSplineSeries>
#include <QList>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    listening{false},
    splineChartInitialized{false},
    audioEngine{new AudioEngine}
{
    ui->setupUi(this);

    // Fill the combobox filled with the available audio input devices on the machine
    const auto devices = audioEngine->getAvailableAudioInputDevices();
    for (size_t i = 0; i < devices.size(); ++i) ui->cb_devices->addItem(devices.at(i).deviceName(), QVariant::fromValue(i));

    ui->pb_toggle_listen->setText(tr("Start"));

    // Connect the events
    connect(ui->pb_toggle_listen, &QPushButton::clicked, this, &MainWindow::toggleListen);
    connect(ui->cb_devices, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::selectDevice);

    connect(audioEngine->getAudioAnalyzerThread(), &AudioAnalyzerThread::levelChanged, ui->w_level, &LevelMeter::levelChanged);
    connect(audioEngine->getAudioAnalyzerThread(), &AudioAnalyzerThread::frequenciesChanged, ui->w_spectrum, &FrequencySpectrum::frequenciesChanged);
    connect(audioEngine->getAudioAnalyzerThread(), &AudioAnalyzerThread::noteChanged, this, &MainWindow::noteChanged);

    selectDevice(0);

    series = new QSplineSeries();
    series->setName("waveform");
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Waveform");
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(-1, 1);
    chart->axes(Qt::Horizontal).first()->setRange(0, 100);

    ui->g_waveform->setChart(chart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleListen() {
    listening = !listening;

    if (listening) {
        audioEngine->startListening();
        ui->pb_toggle_listen->setText(tr("Stop"));
    }
    else {
        audioEngine->stopListening();
        ui->pb_toggle_listen->setText(tr("Start"));
    }
}

void MainWindow::selectDevice(int index) {
    audioEngine->setAudioInputDevice(static_cast<size_t>(index));
}

void MainWindow::waveFormChanged(const std::vector<float>& data, size_t numSamples, const size_t time) {

    qDebug() << "OH" << numSamples;
    QList<QPointF> points;

    float mult = static_cast<float>(time) / numSamples;
    for (size_t i = 0; i < numSamples; i++) {
        points.append(QPointF(static_cast<double>(i * mult), static_cast<double>(data[i])));
        qDebug() << "ADDED POINTS " << static_cast<double>(i * mult) << ", " << data[i] << "  " << mult;
    }

    if (splineChartInitialized) {
        series->replace(points);
    }
    else {
        series->append(points);
        splineChartInitialized = true;
    }


//    const int graphic_width = ui->g_waveform->width();



//    for (size_t i = 0; i < data->size(); i++) {

//        int x = i % (data->size() / time);

//    }
//    points.append()
//    if (!splineChartInitialized) {
//        series->
//    }

//    for (size_t i = 0; i < time; i++) {
//        size_t current_index = i * data->size() / time;
//        size_t end = (i+1) * data->size() / time;

//        float sum = 0;
//        while(current_index < end) {
//            sum += data->at(current_index);
//            current_index++;
//        }

//        splin
//    }
}


void MainWindow::noteChanged(const QString note) {
    ui->lbl_note->setText(note);
}
