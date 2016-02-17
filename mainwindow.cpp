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
    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    connect(ui->startButton, SIGNAL (released()), worker, SLOT (process()));
    connect(ui->stopButton, SIGNAL (released()), this, SLOT (handleStopButton()));
    connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
}

void MainWindow::handleStopButton() {
    worker->stop();
}

MainWindow::~MainWindow()
{
    delete ui;
}
