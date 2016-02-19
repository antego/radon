#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturer.h"

#include <QThread>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->button, SIGNAL (released()), this, SLOT (handleButton()));
}

void MainWindow::handleButton()
{
    if (!cameraEnabled)
    {
        startCamera();
    }
    else
    {
        stopCamera();
    }
}

void MainWindow::startCamera()
{
    thread = new QThread();
    capturer = new Capturer(0, 640, 480);

    capturer->moveToThread(thread);
    connect(thread, SIGNAL (started()), capturer, SLOT (process()));
    connect(capturer, SIGNAL (cameraStopped()), this, SLOT (confirmCameraStop()));
    connect(capturer, SIGNAL (finished()), thread, SLOT (quit()));
    connect(capturer, SIGNAL (finished()), capturer, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
    ui->button->setText("Stop camera");
    cameraEnabled = true;
}

void MainWindow::stopCamera()
{
    thread->requestInterruption();
}

void MainWindow::confirmCameraStop()
{
    ui->button->setText("Start camera");
    cameraEnabled = false;
}

MainWindow::~MainWindow()
{
    if (cameraEnabled)
    {
        stopCamera();
    }
    delete ui;
}
