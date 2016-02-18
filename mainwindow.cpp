#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"

#include <QThread>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startButton, SIGNAL (released()), this, SLOT (handleStartButton()));
    connect(ui->stopButton, SIGNAL (released()), this, SLOT (handleStopButton()));
}

void MainWindow::handleStartButton()
{
    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    connect(thread, SIGNAL (started()), worker, SLOT (process()));
    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
}

void MainWindow::handleStopButton()
{
    thread->requestInterruption();
}

MainWindow::~MainWindow()
{
    delete ui;
}
