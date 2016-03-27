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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentDir(NULL),
    cameraEnabled(false),
    closeRequested(false)
{
    ui->setupUi(this);
    ui->progressBar->hide();

    ui->comboBox->addItem("Vertical");
    ui->comboBox->addItem("Horizontal");
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
    capturer = new Capturer(ui->camSpin->value(), ui->widthSpin->value(), ui->heightSpin->value());
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
    ui->button->setText("Stop camera");
    cameraEnabled = true;
}

void MainWindow::stopCamera()
{
    capThread->requestInterruption();
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
    fileList = currentDir->entryInfoList(nameFilters, QDir::Files);

    ui->fileListTable->setRowCount(0);
    for (int i = 0; i < fileList.size(); i++)
    {
        ui->fileListTable->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(fileList[i].fileName());
        ui->fileListTable->setItem(i, 0, item);
        QString baseName = fileList[i].baseName();
        QRegularExpressionMatch match = regexp->match(baseName);
        if (match.hasMatch())
        {
            QTableWidgetItem *item = new QTableWidgetItem(match.captured(1));
            ui->fileListTable->setItem(i, 1, item);
        }
    }
    ui->fileListTable->resizeColumnsToContents();
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
    if (fileList.size() == 0)
    {
        handleError("Select files for processing");
        return;
    }
    std::vector <float> angles;
    QStringList fileNames;
    for (int i = 0; i < ui->fileListTable->rowCount(); i++)
    {
        bool parseOk = true;
        angles.push_back(ui->fileListTable->item(i, 1)->text().toFloat(&parseOk));
        if (!parseOk)
        {
            handleError("Invalid angle text");
            return;
        }

    }
    ui->radonButton->setEnabled(false);
    ui->progressBar->show();

    Scanner::shaftOrientation orientation;
    if (ui->comboBox->currentText() == "Vertical")
        orientation = Scanner::shaftOrientation::VERTICAL;
    else
        orientation = Scanner::shaftOrientation::HORIZONTAL;

    scanner = new Scanner(ui->deltaKSpin->value(), ui->deltaRhoSpin->value(), fileList, angles, orientation);
    scanThread = new QThread();

    scanner->moveToThread(scanThread);

    connect(scanner, SIGNAL(setStepsCount(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(scanner, SIGNAL(setCurrentCount(int)), ui->progressBar, SLOT(setValue(int)));
    connect(scanner, SIGNAL(error(QString)), this, SLOT(handleError(QString)));
    connect(scanner, SIGNAL(finished()), this, SLOT(handleRadonFinish()));
    connect(scanner, SIGNAL(finished()), scanner, SLOT(deleteLater()));
    connect(scanner, SIGNAL(finished()), scanThread, SLOT(quit()));
    connect(scanThread, SIGNAL(started()), scanner, SLOT(scan()));
    connect(scanThread, SIGNAL(finished()), scanThread, SLOT(deleteLater()));
    scanThread->start();
}

void MainWindow::handleRadonFinish()
{
    ui->radonButton->setEnabled(true);
    ui->progressBar->hide();
    ui->progressBar->reset();
}

MainWindow::~MainWindow()
{
    delete regexp;
    delete ui;
    delete currentDir;
}
