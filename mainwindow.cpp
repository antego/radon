#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturer.h"

#include <QThread>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cameraEnabled(false)
{
    ui->setupUi(this);

    regexp = new QRegularExpression(".*\\((.*)\\)$");

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

    QDir imageDir(folder);
    QStringList nameFilters;
    nameFilters << "*.bmp" << "*.jpg" << "*.png";
    QFileInfoList list = imageDir.entryInfoList(nameFilters, QDir::Files);

    ui->fileListTable->clear();
    for (int i = 0; i < list.size(); i++)
    {
        ui->fileListTable->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(list[i].fileName());
        ui->fileListTable->setItem(i, 0, item);
        QString baseName = list[i].baseName();
        QRegularExpressionMatch match = regexp->match(baseName);
        if (match.hasMatch())
        {
            QTableWidgetItem *item = new QTableWidgetItem(match.captured(1));
            ui->fileListTable->setItem(i, 1, item);
        }
    }
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
    delete regexp;
    delete ui;
}
