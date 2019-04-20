#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "levelmeter.h"
#include "audioengine.h"
#include "frequencyspectrum.h"

#include <QList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
    , ui{new Ui::MainWindow}
    , audioEngine{new AudioEngine}
    , listening{false}
    , splineChartInitialized{false}
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

void MainWindow::noteChanged(const QString note) {
    ui->lbl_note->setText(note);
}
