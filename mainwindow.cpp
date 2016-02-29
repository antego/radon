#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "capturer.h"

#include "radon.h"

#include <QThread>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>

#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentDir(NULL),
    cameraEnabled(false),
    closeRequested(false)
{
    ui->setupUi(this);

    regexp = new QRegularExpression(".*\\((.*)\\)$");

    connect(ui->button, SIGNAL (released()), this, SLOT (handleCamButton()));
    connect(ui->takePictureButton, SIGNAL (released()), this, SLOT (takePicture()));
    connect(ui->chooseFolderButton, SIGNAL (released()), this, SLOT (openFolder()));
    connect(ui->radonButton, SIGNAL (released()), this, SLOT (doRadon()));
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
    connect(capturer, SIGNAL (shotTaken()), this, SLOT (ackShot()));
    connect(capturer, SIGNAL (finished()), this, SLOT (closeIfNeeded()));
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

void MainWindow::closeIfNeeded()
{
    if (closeRequested)
        close();
}

void MainWindow::openFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Choose image folder");
    ui->folderNameLabel->setText(folder);

    currentDir = new QDir(folder);
    reloadFolder();
}

void MainWindow::reloadFolder()
{
    QStringList nameFilters;
    nameFilters << "*.bmp" << "*.jpg" << "*.png";
    QFileInfoList list = currentDir->entryInfoList(nameFilters, QDir::Files);

    ui->fileListTable->setRowCount(0);
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
    if (!cameraEnabled)
    {
        handleError("Start camera before taking picture");
        return;
    }

    QString fileName = ui->fileNameEdit->text();
    if (fileName.isEmpty())
    {
        handleError("Specify filename");
        return;
    }

    if (currentDir == NULL)
    {
        handleError("Specify current directory");
        return;
    }

    double angle = ui->currentAngleSpin->value();
    fileName = fileName + "(" + QString::number(angle) + ").bmp";
    QString fullName = currentDir->absolutePath() + "/" + fileName;
    capturer->requestShot(fullName);
    ui->currentAngleSpin->setValue(angle + ui->stepAngleSpin->value());
}

void MainWindow::ackShot()
{
    reloadFolder();
    ui->statusBar->showMessage("Picture saved", 1000);
}

void MainWindow::handleError(QString errorCaption)
{
    QMessageBox::information(this, "Info", errorCaption, QMessageBox::Ok);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (cameraEnabled)
    {
        closeRequested = true;
        stopCamera();
        event->ignore();
    }
    else
    {
        event->accept();
    }
}

void MainWindow::doRadon()
{
    cv::Mat origin;
    origin = cv::imread("/home/anton/qt_projects/qtOpencvSimpleWindow/radon-test2.png", CV_LOAD_IMAGE_GRAYSCALE);
    qDebug(QString::number(origin.type()).toLocal8Bit());
    qDebug(QString::number(origin.channels()).toLocal8Bit());
    std::vector <float> angles;
    for (int i = 0; i < 180; i++)
    {
        angles.push_back(i);
    }
    Radon::radonSinc(origin);
}

MainWindow::~MainWindow()
{
    delete regexp;
    delete ui;
    delete currentDir;
}
