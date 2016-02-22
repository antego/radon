#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturer.h"

#include <QThread>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cameraEnabled(false)
{
    ui->setupUi(this);

    connect(ui->button, SIGNAL (released()), this, SLOT (handleCamButton()));
    connect(ui->chooseFolderButton, SIGNAL (released()), this, SLOT (openFolder()));
}

void MainWindow::handleCamButton()
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
    capturer = new Capturer(0, 1280, 720);

    capturer->moveToThread(thread);
    connect(thread, SIGNAL (started()), capturer, SLOT (process()));
    connect(capturer, SIGNAL (error(QString)), this, SLOT (handleError(QString)));
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

void MainWindow::openFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Choose image folder");
    ui->folderNameLabel->setText(folder);
}

void MainWindow::takePicture()
{

}

void MainWindow::handleError(QString errorCaption)
{
    QMessageBox::information(this, "Info", errorCaption, QMessageBox::Ok);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (cameraEnabled)
    {
        stopCamera();
    }
    event->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}
