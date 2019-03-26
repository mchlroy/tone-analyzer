#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audioinput.h"
#include "levelmeter.h"
#include "audioengine.h"

#include <QMainWindow>
#include <QtCharts/QSplineSeries>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    bool listening;
    bool splineChartInitialized;

    void levelChanged(qreal rmsLevel, qreal peakLevel);
    //AudioInput* audioInput;

    AudioEngine* audioEngine;

    QtCharts::QSplineSeries *series;
private slots:
    void toggleListen();
    void selectDevice(int i);
    void waveFormChanged(const std::vector<float>& data, size_t numSamples, const size_t time);
    void noteChanged(const QString note);
};

#endif // MAINWINDOW_H
