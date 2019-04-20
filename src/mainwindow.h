#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audioinput.h"
#include "levelmeter.h"
#include "audioengine.h"

#include <QMainWindow>
#include <memory>

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
    Ui::MainWindow *ui;             // Qt doesn't like it's ui to be unique_ptr because sizeof() doesn't work..
    std::unique_ptr<AudioEngine> audioEngine;
    bool listening;
    bool splineChartInitialized;

    void levelChanged(qreal rmsLevel, qreal peakLevel);
private slots:
    void toggleListen();
    void selectDevice(int i);
    void noteChanged(const QString note);
};

#endif // MAINWINDOW_H
