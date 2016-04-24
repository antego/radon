#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "radon.h"

#include <QThread>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>

#include <opencv2/highgui/highgui.hpp>

const float PI=3.14159265358979f;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    cameraEnabled(false),
    closeRequested(false),
    regexp(new QRegularExpression(".*\\((.*)\\)$"))
{
    ui.setupUi(this);
    ui.progressBar->hide();

    ui.comboBox->addItem("Vertical");
    ui.comboBox->addItem("Horizontal");

    connect(ui.button, SIGNAL (released()), this, SLOT (handleCamButton()));
    connect(ui.takePictureButton, SIGNAL (released()), this, SLOT (takePicture()));
    connect(ui.chooseFolderButton, SIGNAL (released()), this, SLOT (openFolder()));
    connect(ui.iradonButton, SIGNAL (released()), this, SLOT (doInverseRadon()));
    connect(ui.radonButton, SIGNAL (released()), this, SLOT (doRadon()));
    connect(ui.generateTestData, SIGNAL (released()), this, SLOT (generateTestImages()));
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
    capturer = new Capturer(ui.camSpin->value(), ui.widthSpin->value(), ui.heightSpin->value());
    capThread = new QThread();

    capturer->moveToThread(capThread);
    connect(capThread, SIGNAL (started()), capturer, SLOT (process()));
    connect(capturer, SIGNAL (error(QString)), this, SLOT (handleError(QString)));
    connect(capturer, SIGNAL (cameraStopped()), this, SLOT (confirmCameraStop()));
    connect(capturer, SIGNAL (shotTaken()), this, SLOT (ackShot()));
    connect(capturer, SIGNAL (finished()), this, SLOT (closeIfNeeded()));
    connect(capturer, SIGNAL (finished()), capThread, SLOT (quit()));
    connect(capturer, SIGNAL (finished()), capturer, SLOT (deleteLater()));
    connect(capThread, SIGNAL (finished()), capThread, SLOT (deleteLater()));
    capThread->start();
    ui.button->setText("Stop camera");
    cameraEnabled = true;
}

void MainWindow::stopCamera()
{
    capThread->requestInterruption();
}

void MainWindow::confirmCameraStop()
{
    ui.button->setText("Start camera");
    cameraEnabled = false;
}

void MainWindow::closeIfNeeded()
{
    if (closeRequested)
        close();
}

void MainWindow::openFolder()
{
    //TODO show files
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec())
    {
        QString folder = fileDialog.selectedFiles().first();
        ui.folderNameLabel->setText(folder);
        currentDir.reset(new QDir(folder));
        reloadFolder();
    }
}

void MainWindow::reloadFolder()
{
    QStringList nameFilters;
    nameFilters << "*.bmp" << "*.jpg" << "*.png";
    fileList = currentDir->entryInfoList(nameFilters, QDir::Files);

    ui.fileListTable->setRowCount(0);
    for (int i = 0; i < fileList.size(); i++)
    {
        ui.fileListTable->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(fileList[i].fileName());
        ui.fileListTable->setItem(i, 0, item);
        QString baseName = fileList[i].baseName();
        QRegularExpressionMatch match = regexp->match(baseName);
        if (match.hasMatch())
        {
            QTableWidgetItem *item = new QTableWidgetItem(match.captured(1));
            ui.fileListTable->setItem(i, 1, item);
        }
    }
    ui.fileListTable->resizeColumnsToContents();
}

void MainWindow::takePicture()
{
    if (!cameraEnabled)
    {
        handleError("Start camera before taking picture");
        return;
    }

    QString fileName = ui.fileNameEdit->text();
    if (fileName.isEmpty())
    {
        handleError("Specify filename");
        return;
    }

    if (currentDir.isNull())
    {
        handleError("Specify current directory");
        return;
    }

    double angle = ui.currentAngleSpin->value();
    fileName = fileName + "(" + QString::number(angle) + ").bmp";
    QString fullName = currentDir->absolutePath() + "/" + fileName;
    capturer->requestShot(fullName);
    ui.currentAngleSpin->setValue(angle + ui.stepAngleSpin->value());
}

void MainWindow::ackShot()
{
    reloadFolder();
    ui.statusBar->showMessage("Picture saved", 1000);
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
    if (fileList.size() == 0)
    {
        handleError("Select files for processing");
        return;
    }
    std::vector <float> angles;
    for (int i = 0; i < 180; i+=ui.deltaTheta->value())
    {
        angles.push_back((double) i / 180 * PI);
    }
    ui.iradonButton->setEnabled(false);
    ui.radonButton->setEnabled(false);
    ui.progressBar->show();

    radonScanner = new RadonScanner(ui.deltaTheta->value(), ui.zScale->value(), angles, fileList);
    scanThread = new QThread();

    radonScanner->moveToThread(scanThread);

    connect(radonScanner, SIGNAL(setStepsCount(int)), ui.progressBar, SLOT(setMaximum(int)));
    connect(radonScanner, SIGNAL(setCurrentCount(int)), ui.progressBar, SLOT(setValue(int)));
    connect(radonScanner, SIGNAL(error(QString)), this, SLOT(handleError(QString)));
    connect(radonScanner, SIGNAL(finished()), this, SLOT(handleRadonFinish()));
    connect(radonScanner, SIGNAL(finished()), radonScanner, SLOT(deleteLater()));
    connect(radonScanner, SIGNAL(finished()), scanThread, SLOT(quit()));
    connect(scanThread, SIGNAL(started()), radonScanner, SLOT(scan()));
    connect(scanThread, SIGNAL(finished()), scanThread, SLOT(deleteLater()));
    scanThread->start();
}

void MainWindow::doInverseRadon()
{
    if (fileList.size() == 0)
    {
        handleError("Select files for processing");
        return;
    }
    std::vector<float> angles;
    for (int i = 0; i < ui.fileListTable->rowCount(); i++)
    {
        bool parseOk = true;
        angles.push_back(ui.fileListTable->item(i, 1)->text().toFloat(&parseOk));
        if (!parseOk)
        {
            handleError("Invalid angle text");
            return;
        }
    }
    ui.iradonButton->setEnabled(false);
    ui.radonButton->setEnabled(false);
    ui.progressBar->show();

    InverseRadonScanner::shaftOrientation orientation;
    if (ui.comboBox->currentText() == "Vertical")
        orientation = InverseRadonScanner::shaftOrientation::VERTICAL;
    else
        orientation = InverseRadonScanner::shaftOrientation::HORIZONTAL;

    inverseRadonScanner = new InverseRadonScanner(ui.deltaKSpin->value(), ui.deltaRhoSpin->value(), fileList, angles, orientation);
    scanThread = new QThread();

    inverseRadonScanner->moveToThread(scanThread);

    connect(inverseRadonScanner, SIGNAL(setStepsCount(int)), ui.progressBar, SLOT(setMaximum(int)));
    connect(inverseRadonScanner, SIGNAL(setCurrentCount(int)), ui.progressBar, SLOT(setValue(int)));
    connect(inverseRadonScanner, SIGNAL(error(QString)), this, SLOT(handleError(QString)));
    connect(inverseRadonScanner, SIGNAL(finished()), this, SLOT(handleRadonFinish()));
    connect(inverseRadonScanner, SIGNAL(finished()), inverseRadonScanner, SLOT(deleteLater()));
    connect(inverseRadonScanner, SIGNAL(finished()), scanThread, SLOT(quit()));
    connect(scanThread, SIGNAL(started()), inverseRadonScanner, SLOT(scan()));
    connect(scanThread, SIGNAL(finished()), scanThread, SLOT(deleteLater()));
    scanThread->start();
}

void MainWindow::handleRadonFinish()
{
    ui.radonButton->setEnabled(true);
    ui.iradonButton->setEnabled(true);
    ui.progressBar->hide();
    ui.progressBar->reset();
}

void MainWindow::generateTestImages()
{
    if (currentDir.isNull())
    {
        handleError("Specify current directory");
        return;
    }

    for (int i = 1; i <= 20; i++)
    {
        cv::Mat image(100, 100, CV_8UC1, cv::Scalar(255));
        cv::circle(image, cv::Point2i(70, 70), i, cv::Scalar(0, 0, 0), -1);
        cv::imwrite(QString("%1/test_image%2.bmp").arg(currentDir->absolutePath()).arg(i, 2, 10, QChar('0')).toStdString(), image);
    }
}

MainWindow::~MainWindow()
{
}
